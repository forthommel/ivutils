#include "ivutils/Device.h"
#include "ivutils/ParametersList.h"

using namespace ivutils;

const std::string Device::M_DEVICE_ID = "*IDN?";

Device::Device( const ParametersList& params ) :
  Messenger( params.getParameter<int>( "address" ) ),
  configCommands_   ( params.getParameter<std::vector<std::string> >( "configCommands" ) ),
  operationCommands_( params.getParameter<std::vector<std::string> >( "operationCommands" ) ),
  closingCommands_  ( params.getParameter<std::vector<std::string> >( "closingCommands" ) )
{
  const auto& dev_id = fetch( M_DEVICE_ID );
}
