#include "ivutils/PythonParser.h"
#include <frameobject.h> // Python

#include <sstream>
#include <stdexcept>
#include <algorithm>

#if PY_MAJOR_VERSION < 3
# define PYTHON2
#endif

using namespace ivutils;

PythonParser::PythonParser( const char* config_file )
{
  setenv( "PYTHONPATH", ".:..:Cards", 1 );
  std::string filename = pythonPath( config_file );
  const size_t fn_len = filename.length()+1;

#ifdef PYTHON2
  char* sfilename = new char[fn_len];
  snprintf( sfilename, fn_len, "%s", filename.c_str() );
#else
  wchar_t* sfilename = new wchar_t[fn_len];
  swprintf( sfilename, fn_len, L"%s", filename.c_str() );
#endif
  if ( sfilename )
    Py_SetProgramName( sfilename );
  Py_InitializeEx( 1 );
  if ( sfilename )
    delete [] sfilename;
  if ( !Py_IsInitialized() )
    throw std::runtime_error( "PythonParser: Failed to initialise the Python cards parser!" );
  PyObject* cfg = PyImport_ImportModule( filename.c_str() ); // new
  if ( !cfg )
    throwPythonError( "Failed to parse the configuration card \""+filename+"\" at "+std::string( config_file ) );

  PyObject* config = PyObject_GetAttrString( cfg, "config" ); // new
  if ( !config )
    throwPythonError( "Failed to extract a \"config\" keyword from the configuration card!" );
  ParametersList::operator+=( get<ParametersList>( config ) );

  //--- finalisation
  Py_CLEAR( config );
  Py_CLEAR( cfg );
}

PythonParser::~PythonParser()
{
  if ( Py_IsInitialized() )
    Py_Finalize();
}

template<> bool
PythonParser::is<int>( PyObject* obj ) const
{
#ifdef PYTHON2
  return PyInt_Check( obj );
#else
  return PyLong_Check( obj );
#endif
}

template<> int
PythonParser::get<int>( PyObject* obj ) const
{
  if ( !obj )
    throwPythonError( "Failed to retrieve integer object!" );
  if ( !is<int>( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: integer != \""+std::string( obj->ob_type->tp_name )+"\"." );
#ifdef PYTHON2
  return PyInt_AsLong( obj );
#else
  return PyLong_AsLong( obj );
#endif
}

template<> bool
PythonParser::is<bool>( PyObject* obj ) const
{
  return PyBool_Check( obj );
}

template<> bool
PythonParser::get<bool>( PyObject* obj ) const
{
  if ( !obj )
    throwPythonError( "Failed to retrieve boolean object!" );
  if ( !is<bool>( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: bool != \""+std::string( obj->ob_type->tp_name )+"\"." );
  return (bool)get<int>( obj );
}

template<> bool
PythonParser::is<double>( PyObject* obj ) const
{
  return PyFloat_Check( obj );
}

template<> double
PythonParser::get<double>( PyObject* obj ) const
{
  if ( !obj )
    throwPythonError( "Failed to retrieve float object!" );
  if ( !is<double>( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: double != \""+std::string( obj->ob_type->tp_name )+"\"." );
  return PyFloat_AsDouble( obj );
}

template<> bool
PythonParser::is<std::string>( PyObject* obj ) const
{
#ifdef PYTHON2
  return PyString_Check( obj );
#else
  return PyUnicode_Check( obj );
#endif
}

template<> std::string
PythonParser::get<std::string>( PyObject* obj ) const
{
  if ( !obj )
    throwPythonError( "Failed to retrieve string object!" );
  if ( !is<std::string>( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: string != \""+std::string( obj->ob_type->tp_name )+"\"." );
#ifdef PYTHON2
  const std::string out = PyString_AsString( obj ); // deprecated in python v3+
#else
  PyObject* pstr = PyUnicode_AsEncodedString( obj, "utf-8", "strict" ); // new
  if ( !pstr )
    throwPythonError( "Failed to decode a Python object!" );
  const std::string out = PyBytes_AS_STRING( pstr );
  Py_CLEAR( pstr );
#endif
  return out;
}

template<typename T> bool
PythonParser::isVector( PyObject* obj ) const
{
  if ( !obj )
    return false;
  if ( !PyTuple_Check( obj ) && !PyList_Check( obj ) )
    return false;
  PyObject* pfirst = PyTuple_Check( obj ) ? PyTuple_GetItem( obj, 0 ) : PyList_GetItem( obj, 0 );
  if ( !is<T>( pfirst ) )
    return false;
  return true;
}

template<typename T> std::vector<T>
PythonParser::getVector( PyObject* obj ) const
{
  if ( !obj )
    throwPythonError( "Failed to retrieve list/tuple object!" );
  //--- check that it is indeed a vector
  if ( !isVector<T>( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: list/tuple != \""+std::string( obj->ob_type->tp_name )+"\"." );
  std::vector<T> vec;
  const bool tuple = PyTuple_Check( obj );
  const Py_ssize_t num_entries = tuple ? PyTuple_Size( obj ) : PyList_Size( obj );
  //--- check every single element inside the list/tuple
  for ( Py_ssize_t i = 0; i < num_entries; ++i ) {
    PyObject* pit = tuple ? PyTuple_GetItem( obj, i ) : PyList_GetItem( obj, i );
    vec.emplace_back( get<T>( pit ) );
  }
  return vec;
}

template<> bool
PythonParser::is<ParametersList>( PyObject* obj ) const
{
  if ( !obj )
    throwPythonError( "Failed to retrieve parameters list object!" );
  return PyDict_Check( obj );
}

template<> ParametersList
PythonParser::get<ParametersList>( PyObject* obj ) const
{
  if ( !obj )
    throwPythonError( "Failed to retrieve parameters list object!" );
  if ( !is<ParametersList>( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: parameters list != \""+std::string( obj->ob_type->tp_name )+"\"." );
  ParametersList out;
  PyObject* pkey = nullptr, *pvalue = nullptr;
  Py_ssize_t pos = 0;
  while ( PyDict_Next( obj, &pos, &pkey, &pvalue ) ) {
    const std::string skey = is<std::string>( pkey )
      ? get<std::string>( pkey )
      : is<int>( pkey )
        ? std::to_string( get<int>( pkey ) ) // integer-type key
        : "invalid";
    if ( is<int>( pvalue ) )
      out.set<int>( skey, get<int>( pvalue ) );
    else if ( is<double>( pvalue ) )
      out.set<double>( skey, get<double>( pvalue ) );
    else if ( is<std::string>( pvalue ) )
      out.set<std::string>( skey, get<std::string>( pvalue ) );
    else if ( is<ParametersList>( pvalue ) )
      out.set<ParametersList>( skey, get<ParametersList>( pvalue ) );
    else if ( PyTuple_Check( pvalue ) || PyList_Check( pvalue ) ) { // vector
      if ( isVector<int>( pvalue ) )
        out.set<std::vector<int> >( skey, getVector<int>( pvalue ) );
      else if ( isVector<double>( pvalue ) )
        out.set<std::vector<double> >( skey, getVector<double>( pvalue ) );
      else if ( isVector<std::string>( pvalue ) )
        out.set<std::vector<std::string> >( skey, getVector<std::string>( pvalue ) );
      else if ( isVector<ParametersList>( pvalue ) )
        out.set<std::vector<ParametersList> >( skey, getVector<ParametersList>( pvalue ) );
    }
    else
      throwPythonError( "Invalid object retrieved as parameters list value!" );
  }
  return out;
}

std::string
PythonParser::pythonPath( const char* file )
{
  std::string s_filename = file;
  s_filename = s_filename.substr( 0, s_filename.find_last_of( "." ) ); // remove the extension
  std::replace( s_filename.begin(), s_filename.end(), '/', '.' ); // replace all '/' by '.'
  return s_filename;
}

void
PythonParser::throwPythonError( const std::string& message )
{
  PyObject* ptype = nullptr, *pvalue = nullptr, *ptraceback_obj = nullptr;
  // retrieve error indicator and clear it to handle ourself the error
  PyErr_Fetch( &ptype, &pvalue, &ptraceback_obj );
  PyErr_Clear();
  // ensure the objects retrieved are properly normalised and point to compatible objects
  PyErr_NormalizeException( &ptype, &pvalue, &ptraceback_obj );
  std::ostringstream oss; oss << message;
  if ( ptype != nullptr ) { // we can start the traceback
    oss << "\n\tError: "
#ifdef PYTHON2
        << PyString_AsString( PyObject_Str( pvalue ) ); // deprecated in python v3+
#else
        << PyUnicode_AsUTF8( PyObject_Str( pvalue ) );
#endif
    PyTracebackObject* ptraceback = (PyTracebackObject*)ptraceback_obj;
    std::string tabul = "↪ ";
    if ( ptraceback != nullptr ) {
      while ( ptraceback->tb_next != nullptr ) {
        PyFrameObject* pframe = ptraceback->tb_frame;
        if ( pframe != nullptr ) {
          int line = PyCode_Addr2Line( pframe->f_code, pframe->f_lasti );
#ifdef PYTHON2
          const char* filename = PyString_AsString( pframe->f_code->co_filename );
          const char* funcname = PyString_AsString( pframe->f_code->co_name );
#else
          const char* filename = PyUnicode_AsUTF8( pframe->f_code->co_filename );
          const char* funcname = PyUnicode_AsUTF8( pframe->f_code->co_name );
#endif
          oss << "\n\t" << tabul << funcname << " on " << filename << " (line " << line << ")";
        }
        else
          oss << "\n\t" << tabul << " issue in line " << ptraceback->tb_lineno;
        tabul = std::string( "  " )+tabul;
        ptraceback = ptraceback->tb_next;
      }
    }
  }
  Py_Finalize();
  throw std::runtime_error( "PythonParser:error: "+oss.str() );
}

PyObject*
PythonParser::encode( const char* str )
{
  PyObject* obj = PyUnicode_FromString( str ); // new
  if ( !obj )
    throwPythonError( "Failed to encode the following string:\n\t"+std::string( str ) );
  return obj;
}

PyObject*
PythonParser::extract( PyObject* obj, const char* key )
{
  PyObject* pout = nullptr, *nink = encode( key );
  if ( !nink )
    return pout;
  pout = PyDict_GetItem( obj, nink ); // borrowed
  Py_CLEAR( nink );
  return pout;
}
