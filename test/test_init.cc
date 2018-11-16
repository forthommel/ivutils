#include "ivutils/Messenger.h"
#include "ivutils/Device.h"
#include <iostream>

int main( int argc, char* argv[] )
{
  if ( argc < 2 ) {
    std::cerr << "Usage: " << argv[0] << " device_address [secondary_address]" << std::endl;
    exit( 0 );
  }
  const int dev_addr = atoi( argv[1] );
  const int sec_addr = ( argc > 2 ) ? atoi( argv[2] ) : 0;

  ivutils::Messenger mess( dev_addr, sec_addr );
  for ( const auto& answ : mess.fetch( ivutils::Device::M_DEVICE_ID ) )
    std::cout << ">>> " << answ << std::endl;

  return 0;
}
