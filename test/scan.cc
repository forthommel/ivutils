#include "ivutils/IVScanner.h"
#include <iostream>

int main( int argc, char* argv[] )
{
  if ( argc < 2 ) {
    std::cerr << "Usage: " << argv[0] << " config_file" << std::endl;
    exit( 0 );
  }
  ivutils::IVScanner scanner( argv[1] );

  return 0;
}
