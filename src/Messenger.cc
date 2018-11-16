#include "ivutils/Messenger.h"
#include "ivutils/ParametersList.h"

#include <exception>
#include <iostream>
#include <thread>
#include <chrono>

#if defined GPIB
# include <gpib/ib.h>
#elif defined NI4882
# include <ni4882.h>
#endif

using namespace ivutils;

const std::regex Messenger::RGX_STR_ANSW( "^(\\w+)A$" );
const std::regex Messenger::RGX_INT_ANSW( "^([0-9]+)A$" );
const std::regex Messenger::RGX_FLT_ANSW( "^([0-9\\.]+)A$" );

Messenger::Messenger( int prim_addr, int second_addr )
{
  if ( prim_addr > 30 || prim_addr < 0 ) {
    std::ostringstream os;
    os << "Primary address must be comprised between 0 and 30. Current value: " << prim_addr << ".";
    throw std::runtime_error( os.str() );
  }
  if ( second_addr > 15 || second_addr < 0 ) {
    std::ostringstream os;
    os << "Secondary address must be comprised between 0 and 15. Current value: " << second_addr << ".";
    throw std::runtime_error( os.str() );
  }
#if defined NI4882 || defined GPIB
  const int board_index = 0, send_eoi = 1, eos_mode = 0;
  const int timeout = T3s; // TNONE?
  device_ = ibdev( board_index, prim_addr, second_addr, timeout, send_eoi, eos_mode );
  if ( device_ < 0 ) {
    std::ostringstream os;
    os << "Failed to initialise the device interface on /dev/gpib" << second_addr << "!";
# if defined GPIB
    os << "\n\t" << gpib_error_string( ThreadIberr() );
# endif
    throw std::runtime_error( os.str() );
  }
  std::cout << "Device is alive and kicking!\n"
    << "  board index: " << board_index << "\n"
    << "  address: " << prim_addr << "/" << second_addr << "." << std::endl;
#endif
}

void
Messenger::send( const std::string& msg ) const
{
  if ( ibwrt( device_, msg.c_str(), msg.size() ) & ERR )
    throw std::runtime_error( "Failed to send the following message:\n  "+msg );
  last_command_ = msg;
}

std::vector<std::string>
Messenger::fetch( const std::string& msg ) const
{
  send( msg );
  std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
  return receive();
}

std::vector<std::string>
Messenger::receive() const
{
  auto start = std::chrono::system_clock::now();

#if defined NI4882 || defined GPIB
  if ( ibrd( device_, (void*)buffer_.data(), buffer_.size() ) & ERR )
    throw std::runtime_error( "Failed to read the board buffer!" );
#else
# error "No IO library found!"
#endif

  std::chrono::duration<double> dur_s = std::chrono::system_clock::now()-start;
  std::cout << "Transferred " << ThreadIbcntl() << " bytes in " << dur_s.count() << " seconds: "
    << ThreadIbcntl()/dur_s.count() << " data throughput." << std::endl;

  std::vector<std::string> ret;
#if defined NI4882 || defined GPIB
  std::string tmp;
  for ( long i = 0; i < ThreadIbcntl(); ++i ) {
    if ( buffer_.at( i ) != '\n' )
      tmp += buffer_.at( i );
    else {
      tmp += '\0'; // terminate the string
      ret.emplace_back( tmp );
      tmp.clear();
    }
  }
#endif
  return ret;
}
