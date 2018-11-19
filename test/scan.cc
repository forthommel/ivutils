#include "ivutils/IVScanner.h"
#include <iostream>

int main( int argc, char* argv[] )
{
  if ( argc < 2 )
    throw std::runtime_error( "Usage: "+std::string( argv[0] )+" config_file" );

  ivutils::IVScanner scanner( argv[1] );
  scanner.configure();
  scanner.test();
  //scanner.stabilityTest();

  return 0;
}
