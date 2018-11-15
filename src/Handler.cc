#include "ivutils/Handler.h"

#include <exception>

using namespace ivutils;

const std::regex Handler::RGX_STR_ANSW( "^(\\w+)A$" );
const std::regex Handler::RGX_INT_ANSW( "^([0-9]+)A$" );
const std::regex Handler::RGX_FLT_ANSW( "^([0-9\\.]+)A$" );

Handler::Handler( const char* config_file )
{
}

void
Handler::send( const message_t& msg ) const
{
  last_command_ = msg;
}

#include <vector>
void
Handler::send( const std::vector<message_t>& msg_batch ) const
{
}

template<> std::string
Handler::get( const message_t& msg ) const
{
  send( msg );
  const auto& ret = fetch();
  std::smatch match;
  if ( !std::regex_search( ret, match, RGX_STR_ANSW ) )
    throw std::runtime_error( "Return value \""+ret+"\" is of invalid type!" );
  return match.str( 1 );
}

template<> int
Handler::get( const message_t& msg ) const
{
  send( msg );
  const auto& ret = fetch();
  std::smatch match;
  if ( !std::regex_search( ret, match, RGX_INT_ANSW ) )
    throw std::runtime_error( "Return value \""+ret+"\" is of invalid type!" );
  return std::stoi( match.str( 1 ) );
}

template<> double
Handler::get( const message_t& msg ) const
{
  send( msg );
  const auto& ret = fetch();
  std::smatch match;
  if ( !std::regex_search( ret, match, RGX_FLT_ANSW ) )
    throw std::runtime_error( "Return value \""+ret+"\" is of invalid type!" );
  return std::stod( match.str( 1 ) );
}

std::string
Handler::fetch() const
{
  std::vector<std::string> ret; //FIXME to be populated!
  if ( ret.size() < 3 )
    throw std::runtime_error( "Invalid size retrieved after sending command"+last_command_+"!" );
  return ret.at( 0 );
}
