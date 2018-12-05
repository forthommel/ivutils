#include "ivutils/IVScanner.h"
#include "ivutils/Logger.h"

int main( int argc, char* argv[] )
{
  if ( argc < 2 )
    ivutils::LogMessage( ivutils::error ) << "Usage: " << argv[0] << " config_file";

  ivutils::IVScanner scanner( argv[1] );
  scanner.configure();
  scanner.scan();
  //scanner.test();

  return 0;
}
