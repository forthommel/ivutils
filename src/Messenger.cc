#include "ivutils/Messenger.h"
#include "ivutils/ParametersList.h"

#include <exception>
#include <iostream>
#include <chrono>

#if defined GPIB
# include "gpib/ib.h"
#elif defined NI4882
# error "Not yet supported!"
#endif

using namespace ivutils;

const std::regex Messenger::RGX_STR_ANSW( "^(\\w+)A$" );
const std::regex Messenger::RGX_INT_ANSW( "^([0-9]+)A$" );
const std::regex Messenger::RGX_FLT_ANSW( "^([0-9\\.]+)A$" );

Messenger::Messenger( const ParametersList& params ) :
  configCommands_   ( params.getParameter<std::vector<std::string> >( "configCommands" ) ),
  operationCommands_( params.getParameter<std::vector<std::string> >( "operationCommands" ) ),
  closingCommands_  ( params.getParameter<std::vector<std::string> >( "closingCommands" ) )
{
#if defined GPIB
  int board_index = 0, prim_addr = 1, second_addr = 0, send_eoi = 1, eos_mode = 0;
  device_ = ibdev( board_index, prim_addr, second_addr, TNONE, send_eoi, eos_mode );
  if ( device_ < 0 )
    throw std::runtime_error( "Failed to initialise the device interface!\n\t"+std::string( gpib_error_string( ThreadIberr() ) ) );
  std::cout << "Device is alive and kicking!\n"
    << "  board index: " << board_index << "\n"
    << "  address: " << prim_addr << "/" << second_addr << "." << std::endl;
#endif
}

void
Messenger::send( const message_t& msg ) const
{
  if ( false )
    throw std::runtime_error( "Failed to send the following message:\n  "+std::string( msg ) );

  //...
  last_command_ = msg;
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
  auto start = std::chrono::system_clock::now();

#if defined GPIB
  if ( ibrd( device_, (void*)buffer_.data(), buffer_.size() ) & ERR )
    throw std::runtime_error( "Failed to read the board buffer!" );
#endif

  std::chrono::duration<double> dur_s = std::chrono::system_clock::now()-start;
  std::cout << "Transferred " << ThreadIbcntl() << " bytes in " << dur_s.count() << " seconds: "
    << ThreadIbcntl() / dur_s.count() << " data throughput." << std::endl;

  ReturnValue ret;//{ std::string( buffer_.at( 0 ) ), std::string( buffer_.at( 1 ) ) };
  return ret;
}
