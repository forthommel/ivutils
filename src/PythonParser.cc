#include "ivutils/PythonParser.h"
#include <frameobject.h> // Python

#include <stdexcept>
#include <algorithm>
#include <sstream>

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
  cfg_ = PyImport_ImportModule( filename.c_str() ); // new
  if ( !cfg_ )
    throwPythonError( "Failed to parse the configuration card \""+filename+"\" at "+std::string( config_file ) );
}

PythonParser::~PythonParser()
{
  //--- finalisation
  Py_CLEAR( cfg_ );
  if ( Py_IsInitialized() )
    Py_Finalize();
}

template<> int
PythonParser::get<int>( const char* key ) const
{
  PyObject* obj = extract( cfg_, key );
  if ( !obj )
    throwPythonError( "Failed to retrieve integer object!" );
  if ( !
#ifdef PYTHON2
    PyInt_Check( obj )
#else
    PyLong_Check( obj )
#endif
  )
    throwPythonError( "PythonParser:get: Object has invalid type: integer != \""+std::string( obj->ob_type->tp_name )+"\"." );
#ifdef PYTHON2
  return PyInt_AsLong( obj );
#else
  return PyLong_AsLong( obj );
#endif
}

template<> bool
PythonParser::get<bool>( const char* key ) const
{
  PyObject* obj = extract( cfg_, key );
  if ( !obj )
    throwPythonError( "Failed to retrieve boolean object!" );
  if ( !PyBool_Check( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: bool != \""+std::string( obj->ob_type->tp_name )+"\"." );
  return (bool)get<int>( key );
}

template<> double
PythonParser::get<double>( const char* key ) const
{
  PyObject* obj = extract( cfg_, key );
  if ( !obj )
    throwPythonError( "Failed to retrieve float object!" );
  if ( !PyFloat_Check( obj ) )
    throwPythonError( "PythonParser:get: Object has invalid type: double != \""+std::string( obj->ob_type->tp_name )+"\"." );
  return PyFloat_AsDouble( obj );
}

template<> std::string
PythonParser::get<std::string>( const char* key ) const
{
  PyObject* obj = extract( cfg_, key );
  if ( !obj )
    throwPythonError( "Failed to retrieve string object!" );
  if ( !
#ifdef PYTHON2
    PyString_Check( obj )
#else
    PyUnicode_Check( obj )
#endif
  )
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

template<typename T> std::vector<T>
PythonParser::getVector( const char* key ) const
{
  PyObject* obj = extract( cfg_, key );
  if ( !obj )
    throwPythonError( "Failed to retrieve list/tuple object!" );
  //--- check that it is indeed a vector
  if ( !PyTuple_Check( obj ) && !PyList_Check( obj ) )
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

ParametersList&
ParametersList::operator+=( const ParametersList& oth )
{
  param_values_.insert( oth.param_values_.begin(), oth.param_values_.end() );
  int_values_.insert( oth.int_values_.begin(), oth.int_values_.end() );
  dbl_values_.insert( oth.dbl_values_.begin(), oth.dbl_values_.end() );
  str_values_.insert( oth.str_values_.begin(), oth.str_values_.end() );
  vec_param_values_.insert( oth.vec_param_values_.begin(), oth.vec_param_values_.end() );
  vec_int_values_.insert( oth.vec_int_values_.begin(), oth.vec_int_values_.end() );
  vec_dbl_values_.insert( oth.vec_dbl_values_.begin(), oth.vec_dbl_values_.end() );
  vec_str_values_.insert( oth.vec_str_values_.begin(), oth.vec_str_values_.end() );
  return *this;
}

namespace ivutils
{
  std::ostream&
  operator<<( std::ostream& os, const ParametersList& params )
  {
    for ( const auto& kv : params.int_values_ )   os << "\n" << kv.first << ": int(" << kv.second << ")";
    for ( const auto& kv : params.dbl_values_ )   os << "\n" << kv.first << ": double(" << kv.second << ")";
    for ( const auto& kv : params.str_values_ )   os << "\n" << kv.first << ": string(" << kv.second << ")";
    for ( const auto& kv : params.param_values_ ) os << "\n" << kv.first << ": param({" << kv.second << "})";
    for ( const auto& kv : params.vec_int_values_ ) {
      os << "\n" << kv.first << ": vint(";
      bool first = true;
      for ( const auto& v : kv.second ) {
        os << ( first ? "" : ", " ) << v;
        first = false;
      }
      os << ")";
    }
    for ( const auto& kv : params.vec_dbl_values_ ) {
      os << "\n" << kv.first << ": vdouble(";
      bool first = true;
      for ( const auto& v : kv.second ) {
        os << ( first ? "" : ", " ) << v;
        first = false;
      }
      os << ")";
    }
    for ( const auto& kv : params.vec_str_values_ ) {
      os << "\n" << kv.first << ": vstring(";
      bool first = true;
      for ( const auto& v : kv.second ) {
        os << ( first ? "" : ", " ) << v;
        first = false;
      }
      os << ")";
    }
    return os;
  }
}

std::vector<std::string>
ParametersList::keys() const
{
  std::vector<std::string> out;
  for ( const auto& p : param_values_ )     out.emplace_back( p.first );
  for ( const auto& p : int_values_ )       out.emplace_back( p.first );
  for ( const auto& p : dbl_values_ )       out.emplace_back( p.first );
  for ( const auto& p : str_values_ )       out.emplace_back( p.first );
  for ( const auto& p : vec_param_values_ ) out.emplace_back( p.first );
  for ( const auto& p : vec_int_values_ )   out.emplace_back( p.first );
  for ( const auto& p : vec_dbl_values_ )   out.emplace_back( p.first );
  for ( const auto& p : vec_str_values_ )   out.emplace_back( p.first );
  return out;
}

std::string
ParametersList::getString( const std::string& key ) const
{
  std::ostringstream os;
  if ( has<ParametersList>( key ) )   os << "params{" << get<ParametersList>( key ) << "}";
  else if ( has<int>( key ) )         os << get<int>( key );
  else if ( has<double>( key ) )      os << get<double>( key );
  else if ( has<std::string>( key ) ) os << get<std::string>( key );
  else if ( has<std::vector<ParametersList> >( key ) ) {
    bool first = true;
    for ( const auto& p : get<std::vector<ParametersList> >( key ) ) {
      os << ( first ? "" : ", " ) << p;
      first = false;
    }
  }
  else if ( has<std::vector<int> >( key ) ) {
    bool first = true;
    for ( const auto& p : get<std::vector<int> >( key ) ) {
      os << ( first ? "" : ", " ) << p;
      first = false;
    }
  }
  else if ( has<std::vector<double> >( key ) ) {
    bool first = true;
    for ( const auto& p : get<std::vector<double> >( key ) ) {
      os << ( first ? "" : ", " ) << p;
      first = false;
    }
  }
  else if ( has<std::vector<std::string> >( key ) ) {
    bool first = true;
    for ( const auto& p : get<std::vector<std::string> >( key ) ) {
      os << ( first ? "" : ", " ) << p;
      first = false;
    }
  }
  return os.str();
}

//------------------------------------------------------------------
// default template (placeholders)
//------------------------------------------------------------------

template<typename T> bool
ParametersList::has( std::string key ) const
{
  throw std::runtime_error( "ParametersList: Invalid type for key="+key+"!" );
}

template<typename T> T
ParametersList::get( std::string key, const T& def ) const
{
  throw std::runtime_error( "ParametersList: Invalid type for key="+key+"!" );
}

template<typename T> T&
ParametersList::operator[]( std::string key )
{
  throw std::runtime_error( "ParametersList: Invalid type for key="+key+"!" );
}

template<typename T> ParametersList&
ParametersList::set( std::string key, const T& value )
{
  throw std::runtime_error( "ParametersList: Invalid type for key="+key+"!" );
}

//------------------------------------------------------------------
// sub-parameters-type attributes
//------------------------------------------------------------------

template<> ParametersList
ParametersList::get<ParametersList>( std::string key, const ParametersList& def ) const
{
  for ( const auto& kv : param_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> ParametersList&
ParametersList::operator[]<ParametersList>( std::string key )
{
  for ( auto& kv : param_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return param_values_[key];
}

template<> ParametersList&
ParametersList::set<ParametersList>( std::string key, const ParametersList& value )
{
  param_values_[key] = value;
  return *this;
}

template<> std::vector<ParametersList>
ParametersList::get<std::vector<ParametersList> >( std::string key, const std::vector<ParametersList>& def ) const
{
  for ( const auto& kv : vec_param_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> std::vector<ParametersList>&
ParametersList::operator[]<std::vector<ParametersList> >( std::string key )
{
  for ( auto& kv : vec_param_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return vec_param_values_[key];
}

template<> ParametersList&
ParametersList::set<std::vector<ParametersList> >( std::string key, const std::vector<ParametersList>& value )
{
  vec_param_values_[key] = value;
  return *this;
}

//------------------------------------------------------------------
// integer-type attributes
//------------------------------------------------------------------

template<> int
ParametersList::get<int>( std::string key, const int& def ) const
{
  for ( const auto& kv : int_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> int&
ParametersList::operator[]<int>( std::string key )
{
  for ( auto& kv : int_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return int_values_[key];
}

template<> ParametersList&
ParametersList::set<int>( std::string key, const int& value )
{
  int_values_[key] = value;
  return *this;
}

template<> std::vector<int>
ParametersList::get<std::vector<int> >( std::string key, const std::vector<int>& def ) const
{
  for ( const auto& kv : vec_int_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> std::vector<int>&
ParametersList::operator[]<std::vector<int> >( std::string key )
{
  for ( auto& kv : vec_int_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return vec_int_values_[key];
}

template<> ParametersList&
ParametersList::set<std::vector<int> >( std::string key, const std::vector<int>& value )
{
  vec_int_values_[key] = value;
  return *this;
}

//------------------------------------------------------------------
// floating point-type attributes
//------------------------------------------------------------------

template<> double
ParametersList::get<double>( std::string key, const double& def ) const
{
  for ( const auto& kv : dbl_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> double&
ParametersList::operator[]<double>( std::string key )
{
  for ( auto& kv : dbl_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return dbl_values_[key];
}

template<> ParametersList&
ParametersList::set<double>( std::string key, const double& value )
{
  dbl_values_[key] = value;
  return *this;
}

template<> std::vector<double>
ParametersList::get<std::vector<double> >( std::string key, const std::vector<double>& def ) const
{
  for ( const auto& kv : vec_dbl_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> std::vector<double>&
ParametersList::operator[]<std::vector<double> >( std::string key )
{
  for ( auto& kv : vec_dbl_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return vec_dbl_values_[key];
}

template<> ParametersList&
ParametersList::set<std::vector<double> >( std::string key, const std::vector<double>& value )
{
  vec_dbl_values_[key] = value;
  return *this;
}

//------------------------------------------------------------------
// string-type attributes
//------------------------------------------------------------------

template<> std::string
ParametersList::get<std::string>( std::string key, const std::string& def ) const
{
  for ( const auto& kv : str_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> std::string&
ParametersList::operator[]<std::string>( std::string key )
{
  for ( auto& kv : str_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return str_values_[key];
}

template<> ParametersList&
ParametersList::set<std::string>( std::string key, const std::string& value )
{
  str_values_[key] = value;
  return *this;
}

template<> std::vector<std::string>
ParametersList::get<std::vector<std::string> >( std::string key, const std::vector<std::string>& def ) const
{
  for ( const auto& kv : vec_str_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return def;
}

template<> std::vector<std::string>&
ParametersList::operator[]<std::vector<std::string> >( std::string key )
{
  for ( auto& kv : vec_str_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  return vec_str_values_[key];
}

template<> ParametersList&
ParametersList::set<std::vector<std::string> >( std::string key, const std::vector<std::string>& value )
{
  vec_str_values_[key] = value;
  return *this;
}
