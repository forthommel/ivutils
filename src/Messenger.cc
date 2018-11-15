#include "ivutils/Messenger.h"

#include <exception>

using namespace ivutils;

const std::regex Messenger::RGX_STR_ANSW( "^(\\w+)A$" );
const std::regex Messenger::RGX_INT_ANSW( "^([0-9]+)A$" );
const std::regex Messenger::RGX_FLT_ANSW( "^([0-9\\.]+)A$" );

Messenger::Messenger( unsigned short addr )
{
}

void
Messenger::send( const message_t& msg ) const
{
  if ( false )
    throw std::runtime_error( "Failed to send the following message:\n  "+std::string( msg ) );
  last_command_ = msg;
}

void
Messenger::send( const std::vector<message_t>& msg_batch ) const
{
  for ( const auto& msg : msg_batch )
    send( msg );
}

template<> std::string
Messenger::get( const message_t& msg ) const
{
  send( msg );
  const auto& ret = fetch();
  std::smatch match;
  if ( !std::regex_search( ret.message, match, RGX_STR_ANSW ) )
    throw std::runtime_error( "Return value \""+ret.message+"\" is of invalid type!" );
  return match.str( 1 );
}

template<> int
Messenger::get( const message_t& msg ) const
{
  send( msg );
  const auto& ret = fetch();
  std::smatch match;
  if ( !std::regex_search( ret.message, match, RGX_INT_ANSW ) )
    throw std::runtime_error( "Return value \""+ret.message+"\" is of invalid type!" );
  return std::stoi( match.str( 1 ) );
}

template<> double
Messenger::get( const message_t& msg ) const
{
  send( msg );
  const auto& ret = fetch();
  std::smatch match;
  if ( !std::regex_search( ret.message, match, RGX_FLT_ANSW ) )
    throw std::runtime_error( "Return value \""+ret.message+"\" is of invalid type!" );
  return std::stod( match.str( 1 ) );
}

Messenger::ReturnValue
Messenger::fetch() const
{
  std::vector<std::string> answ( 3 ); //FIXME to be populated!
  if ( answ.size() < 3 )
    throw std::runtime_error( "Invalid size retrieved after sending command"+last_command_+"!" );
  ReturnValue ret{ answ.at( 0 ), answ.at( 1 ) };
  return ret;
}
