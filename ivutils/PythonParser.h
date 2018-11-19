#ifndef ivutils_PythonParser_h
#define ivutils_PythonParser_h

#include <Python.h>
#include "ivutils/ParametersList.h"

namespace ivutils
{
  /// Parse a Python configuration file
  /// \note Self-stolen from CepGen...
  class PythonParser : public ParametersList
  {
    public:
      PythonParser() = default;
      /// Constructor from an external configuration file
      explicit PythonParser( const char* config_file );
      ~PythonParser();

    private:
      static void throwPythonError( const std::string& message );
      static std::string pythonPath( const char* file );
      static PyObject* encode( const char* str );
      static PyObject* extract( PyObject*, const char* key );

      template<typename T> bool is( PyObject* obj ) const;
      template<typename T> T get( PyObject* obj ) const;
      template<typename T> bool isVector( PyObject* obj ) const;
      template<typename T> std::vector<T> getVector( PyObject* obj ) const;
  };
  template<> bool PythonParser::is<int>( PyObject* obj ) const;
  template<> bool PythonParser::is<bool>( PyObject* obj ) const;
  template<> bool PythonParser::is<double>( PyObject* obj ) const;
  template<> bool PythonParser::is<std::string>( PyObject* obj ) const;
  template<> int PythonParser::get<int>( PyObject* obj ) const;
  template<> bool PythonParser::get<bool>( PyObject* obj ) const;
  template<> double PythonParser::get<double>( PyObject* obj ) const;
  template<> std::string PythonParser::get<std::string>( PyObject* obj ) const;
  template<> ParametersList PythonParser::get<ParametersList>( PyObject* obj ) const;
}

#endif
