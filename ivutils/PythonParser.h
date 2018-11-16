#ifndef ivutils_PythonParser_h
#define ivutils_PythonParser_h

#include <Python.h>
#include <string>

namespace ivutils
{
  /// Parse a Python configuration file
  /// \note Self-stolen from CepGen...
  class PythonParser
  {
    public:
      PythonParser() = default;
      explicit PythonParser( const char* config_file );
      ~PythonParser();

    private:
      static void throwPythonError( const std::string& message );
      static std::string pythonPath( const char* file );
      static PyObject* encode( const char* str );

      PyObject* cfg_;
  };
}

#endif
