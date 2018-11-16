#ifndef ivutils_ParametersList_h
#define ivutils_ParametersList_h

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace ivutils
{
  /// Parameters container
  class ParametersList
  {
    public:
      ParametersList() = default;
      ~ParametersList() = default; // required for unique_ptr initialisation!
      /// Check if a given parameter is handled in this list
      template<typename T> bool hasParameter( std::string key ) const;
      /// Get a parameter value
      template<typename T> T getParameter( std::string key ) const;
      /// Set a parameter value
      template<typename T> ParametersList& set( std::string key, const T& value );
      /// Concatenate two parameters containers
      ParametersList& operator+=( const ParametersList& oth );

      /// List of keys handled in this list of parameters
      std::vector<std::string> keys() const;
      /// Get a string-converted version of a value
      std::string getString( const std::string& key ) const;

      /// Human-readable version of a parameters container
      friend std::ostream& operator<<( std::ostream& os, const ParametersList& );

    private:
      std::map<std::string,ParametersList> param_values_;
      std::unordered_map<std::string,int> int_values_;
      std::unordered_map<std::string,double> dbl_values_;
      std::unordered_map<std::string,std::string> str_values_;
      std::unordered_map<std::string,std::vector<ParametersList> > vec_param_values_;
      std::unordered_map<std::string,std::vector<int> > vec_int_values_;
      std::unordered_map<std::string,std::vector<double> > vec_dbl_values_;
      std::unordered_map<std::string,std::vector<std::string> > vec_str_values_;
  };
  /// Check if an integer parameter is handled
  template<> inline bool ParametersList::hasParameter<int>( std::string key ) const { return int_values_.count( key ) != 0; }
  /// Get an integer parameter value
  template<> int ParametersList::getParameter<int>( std::string key ) const;
  /// Set an integer parameter value
  template<> ParametersList& ParametersList::set<int>( std::string key, const int& value );
  /// Check if a vector of integers parameter is handled
  template<> inline bool ParametersList::hasParameter<std::vector<int> >( std::string key ) const { return vec_int_values_.count( key ) != 0; }
  /// Get a vector of integers parameter value
  template<> std::vector<int> ParametersList::getParameter<std::vector<int> >( std::string key ) const;
  /// Set a vector of integers parameter value
  template<> ParametersList& ParametersList::set<std::vector<int> >( std::string key, const std::vector<int>& value );

  /// Check if a boolean parameter is handled
  template<> inline bool ParametersList::hasParameter<bool>( std::string key ) const { return hasParameter<int>( key ); }
  /// Get a boolean parameter value
  template<> inline bool ParametersList::getParameter<bool>( std::string key ) const { return static_cast<bool>( getParameter<int>( key ) ); }
  /// Set a boolean parameter value
  template<> inline ParametersList& ParametersList::set<bool>( std::string key, const bool& value ) { return set<int>( key, static_cast<bool>( value ) ); }

  /// Check if a double floating point parameter is handled
  template<> inline bool ParametersList::hasParameter<double>( std::string key ) const { return dbl_values_.count( key ) != 0; }
  /// Get a double floating point parameter value
  template<> double ParametersList::getParameter<double>( std::string key ) const;
  /// Set a double floating point parameter value
  template<> ParametersList& ParametersList::set<double>( std::string key, const double& value );
  /// Check if a vector of double floating point parameter is handled
  template<> inline bool ParametersList::hasParameter<std::vector<double> >( std::string key ) const { return vec_dbl_values_.count( key ) != 0; }
  /// Get a vector of double floating point parameter value
  template<> std::vector<double> ParametersList::getParameter<std::vector<double> >( std::string key ) const;
  /// Set a vector of double floating point parameter value
  template<> ParametersList& ParametersList::set<std::vector<double> >( std::string key, const std::vector<double>& value );

  /// Check if a string parameter is handled
  template<> inline bool ParametersList::hasParameter<std::string>( std::string key ) const { return str_values_.count( key ) != 0; }
  /// Get a string parameter value
  template<> std::string ParametersList::getParameter<std::string>( std::string key ) const;
  /// Set a string parameter value
  template<> ParametersList& ParametersList::set<std::string>( std::string key, const std::string& value );
  /// Check if a vector of strings parameter is handled
  template<> inline bool ParametersList::hasParameter<std::vector<std::string> >( std::string key ) const { return vec_str_values_.count( key ) != 0; }
  /// Get a vector of strings parameter value
  template<> std::vector<std::string> ParametersList::getParameter<std::vector<std::string> >( std::string key ) const;
  /// Set a vector of strings parameter value
  template<> ParametersList& ParametersList::set<std::vector<std::string> >( std::string key, const std::vector<std::string>& value );

  /// Check if a parameters list parameter is handled
  template<> inline bool ParametersList::hasParameter<ParametersList>( std::string key ) const { return param_values_.count( key ) != 0; }
  /// Get a parameters list parameter value
  template<> ParametersList ParametersList::getParameter<ParametersList>( std::string key ) const;
  /// Set a parameters list parameter value
  template<> ParametersList& ParametersList::set<ParametersList>( std::string key, const ParametersList& value );
  /// Check if a vector of parameters lists is handled
  template<> inline bool ParametersList::hasParameter<std::vector<ParametersList> >( std::string key ) const { return vec_param_values_.count( key ) != 0; }
  /// Get a vector of parameters list parameter value
  template<> std::vector<ParametersList> ParametersList::getParameter<std::vector<ParametersList> >( std::string key ) const;
  /// Set a vector of parameters list parameter value
  template<> ParametersList& ParametersList::set<std::vector<ParametersList> >( std::string key, const std::vector<ParametersList>& value );
}

#endif
