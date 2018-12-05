#include "ivutils/Messenger.h"
#include "ivutils/Device.h"
#include "ivutils/Logger.h"

int main( int argc, char* argv[] )
{
  if ( argc < 2 )
    LogMessage( error ) << "Usage: " << argv[0] << " device_address [secondary_address]";

  const int dev_addr = atoi( argv[1] );
  const int sec_addr = ( argc > 2 ) ? atoi( argv[2] ) : 0;

  LogMessage( info ) << "Will fetch device address: " << dev_addr << "|" << sec_addr << ".";

  ivutils::Messenger mess( dev_addr, sec_addr );
  for ( const auto& answ : mess.fetch( ivutils::Device::M_DEVICE_ID ) )
    LogMessage( info ) << "Device ID: " << answ;
  for ( const auto& answ : mess.fetch( ivutils::Device::M_READ/*":READ?"*/ ) )
    LogMessage( info ) << "Read value: " << answ;

  return 0;
}
