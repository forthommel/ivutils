#include "ivutils/Messenger.h"
#include "ivutils/ParametersList.h"

#include <exception>
#include <sstream>
#include <thread>
#include <chrono>

#if defined GPIB
# include <gpib/ib.h>
#elif defined NI4882
# include <ni4882.h>
#endif

using namespace ivutils;

const unsigned short Messenger::ACK_TIME_MS = 20;

Messenger::Messenger( int prim_addr, int second_addr ) :
  device_( -1 )
#ifdef EMULATE
  , cmd_file_( "commands.out", std::ios::out )
#endif
{
  if ( prim_addr < 0 )
    return;
  if ( prim_addr > 30 ) {
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
  char* version_chr;
  ibvers( &version_chr );
  LogMessage( info ) << "GPIB version " << version_chr << " initialised.";
  const int board_index = 0, send_eoi = 1, eos_mode = 0;
  const int timeout = T3s; // TNONE?
  device_ = ibdev( board_index, prim_addr, second_addr, timeout, send_eoi, eos_mode );
  if ( device_ < 0 ) {
    std::ostringstream os;
    os << "Failed to initialise the device interface! ret=" << device_ << ".";
# if defined GPIB
    os << "\n\t" << gpib_error_string( ThreadIberr() );
# endif
    throw std::runtime_error( os.str() );
  }
  clear();
  LogMessage( info ) << "Device is alive and kicking!\n"
    << "  board index: " << board_index << "\n"
    << "  addresses: primary: " << prim_addr << ", secondary: " << second_addr << ".";
#endif
}

Messenger::~Messenger()
{
#if defined NI4882 || defined GPIB
  if ( device_ >= 0 && ibonl( device_, 1 ) & ERR )
    LogMessage( error ) << "Failed to reset the board to its default state!";
#endif
}

void
Messenger::clear() const
{
#if defined NI4882 || defined GPIB
  const int res = ibclr( device_ );
  if ( res & ERR ) {
    std::ostringstream os;
    os
      << "Failed to clear the messenger:\n"
      << "Return value: " << res << ", "
      << "GPIB error: " << gpib_error_string( ThreadIberr() );
    throw std::runtime_error( os.str() );
  }
  LogMessage( info ) << "Device clear sent " << res << ".";
#endif
}

void
Messenger::send( std::string msg ) const
{
#if defined EMULATE
  cmd_file_ << msg << "\n";
#elif defined NI4882 || defined GPIB
  const std::string out_msg = msg+"\n";
  const int res = ibwrt( device_, out_msg.c_str(), out_msg.size() );
  if ( res & ERR ) {
    std::ostringstream os;
    os
      << "Failed to send the following message:\n  "
      << msg << "\n"
      << "Return value: " << res << ", "
      << "GPIB error: " << gpib_error_string( ThreadIberr() );
    throw std::runtime_error( os.str() );
  }
  last_command_ = msg;
#endif
}

std::vector<std::string>
Messenger::fetch( const std::string& msg ) const
{
  send( msg );
  std::this_thread::sleep_for( std::chrono::milliseconds( ACK_TIME_MS ) );
  return receive();
}

std::vector<std::string>
Messenger::receive() const
{
#ifdef EMULATE
  return std::vector<std::string>{ "-1.A,1,dummy" };
#elif defined NI4882 || defined GPIB
  auto start = std::chrono::system_clock::now();

  if ( ibrd( device_, (void*)buffer_.data(), buffer_.size() ) & ERR )
    throw std::runtime_error( "Failed to read the board buffer!" );

  std::chrono::duration<double> dur_s = std::chrono::system_clock::now()-start;
  LogMessage( info ) << "Transferred " << ThreadIbcntl() << " bytes in " << dur_s.count() << " seconds: "
    << ThreadIbcntl()*1.e-3/dur_s.count() << " kb/s data throughput.";

  std::vector<std::string> ret;
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
  return ret;
#else
  throw std::runtime_error( "No communication libraries are linked against this library! Cannot communicate..." );
#endif
}
