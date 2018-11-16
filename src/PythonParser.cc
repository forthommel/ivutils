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
    throw std::runtime_error( "PythonHandler: Failed to initialise the Python cards parser!" );
  cfg_ = PyImport_ImportModule( filename.c_str() ); // new
  if ( !cfg_ )
    throwPythonError( "Failed to parse the configuration card "+std::string( config_file ) );

}

PythonParser::~PythonParser()
{
  //--- finalisation
  Py_CLEAR( cfg_ );
  if ( Py_IsInitialized() )
    Py_Finalize();
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
  throw std::runtime_error( "PythonHandler:error: "+oss.str() );
}

PyObject*
PythonParser::encode( const char* str )
{
  PyObject* obj = PyUnicode_FromString( str ); // new
  if ( !obj )
    throwPythonError( "Failed to encode the following string:\n\t"+std::string( str ) );
  return obj;
}
