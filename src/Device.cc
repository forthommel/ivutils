#include "ivutils/Device.h"
#include "ivutils/ParametersList.h"

using namespace ivutils;

Device::Device( const ParametersList& params ) :
  Messenger( params.getParameter<int>( "address" ) ),
  configCommands_   ( params.getParameter<std::vector<std::string> >( "configCommands" ) ),
  operationCommands_( params.getParameter<std::vector<std::string> >( "operationCommands" ) ),
  closingCommands_  ( params.getParameter<std::vector<std::string> >( "closingCommands" ) )
{}
