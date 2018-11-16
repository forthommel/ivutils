#include "ivutils/ParametersList.h"
#include <sstream>

using namespace ivutils;

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
  if ( hasParameter<ParametersList>( key ) )   os << "params{" << getParameter<ParametersList>( key ) << "}";
  else if ( hasParameter<int>( key ) )         os << getParameter<int>( key );
  else if ( hasParameter<double>( key ) )      os << getParameter<double>( key );
  else if ( hasParameter<std::string>( key ) ) os << getParameter<std::string>( key );
  else if ( hasParameter<std::vector<ParametersList> >( key ) ) {
    bool first = true;
    for ( const auto& p : getParameter<std::vector<ParametersList> >( key ) ) {
      os << ( first ? "" : ", " ) << p;
      first = false;
    }
  }
  else if ( hasParameter<std::vector<int> >( key ) ) {
    bool first = true;
    for ( const auto& p : getParameter<std::vector<int> >( key ) ) {
      os << ( first ? "" : ", " ) << p;
      first = false;
    }
  }
  else if ( hasParameter<std::vector<double> >( key ) ) {
    bool first = true;
    for ( const auto& p : getParameter<std::vector<double> >( key ) ) {
      os << ( first ? "" : ", " ) << p;
      first = false;
    }
  }
  else if ( hasParameter<std::vector<std::string> >( key ) ) {
    bool first = true;
    for ( const auto& p : getParameter<std::vector<std::string> >( key ) ) {
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
ParametersList::hasParameter( std::string key ) const
{
  throw std::runtime_error( "ParametersList: Invalid type for key="+key+"!" );
}

template<typename T> T
ParametersList::getParameter( std::string key ) const
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
ParametersList::getParameter<ParametersList>( std::string key ) const
{
  for ( const auto& kv : param_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
}

template<> ParametersList&
ParametersList::set<ParametersList>( std::string key, const ParametersList& value )
{
  param_values_[key] = value;
  return *this;
}

template<> std::vector<ParametersList>
ParametersList::getParameter<std::vector<ParametersList> >( std::string key ) const
{
  for ( const auto& kv : vec_param_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
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
ParametersList::getParameter<int>( std::string key ) const
{
  for ( const auto& kv : int_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
}

template<> ParametersList&
ParametersList::set<int>( std::string key, const int& value )
{
  int_values_[key] = value;
  return *this;
}

template<> std::vector<int>
ParametersList::getParameter<std::vector<int> >( std::string key ) const
{
  for ( const auto& kv : vec_int_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
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
ParametersList::getParameter<double>( std::string key ) const
{
  for ( const auto& kv : dbl_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
}

template<> ParametersList&
ParametersList::set<double>( std::string key, const double& value )
{
  dbl_values_[key] = value;
  return *this;
}

template<> std::vector<double>
ParametersList::getParameter<std::vector<double> >( std::string key ) const
{
  for ( const auto& kv : vec_dbl_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
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
ParametersList::getParameter<std::string>( std::string key ) const
{
  for ( const auto& kv : str_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
}

template<> ParametersList&
ParametersList::set<std::string>( std::string key, const std::string& value )
{
  str_values_[key] = value;
  return *this;
}

template<> std::vector<std::string>
ParametersList::getParameter<std::vector<std::string> >( std::string key ) const
{
  for ( const auto& kv : vec_str_values_ )
    if ( kv.first.compare( key ) == 0 )
      return kv.second;
  throw std::runtime_error( "Failed to retrieve parameter with key="+key+"!" );
}

template<> ParametersList&
ParametersList::set<std::vector<std::string> >( std::string key, const std::vector<std::string>& value )
{
  vec_str_values_[key] = value;
  return *this;
}
