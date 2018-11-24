#include "ivutils/Device.h"
#include "ivutils/ParametersList.h"
#include "ivutils/Utils.h"
#include <iostream>

using namespace ivutils;

const std::string Device::M_DEVICE_ID = "*IDN?";
const std::string Device::M_RESET = "*RST";
const std::string Device::M_READ = ":READ?";

const std::regex Device::RGX_STR_ANSW( "^(\\w+)A$" );
const std::regex Device::RGX_NUM_ANSW( "^([\\+\\-]?[0-9E\\.\\+\\-]+)(.)?$" );

Device::Device( const ParametersList& params ) :
  Messenger( params.getParameter<int>( "address" ),
             params.hasParameter<int>( "secondaryAddress" ) ? params.getParameter<int>( "secondaryAddress" ) : 0 ),
  configCommands_   ( params.getParameter<std::vector<std::string> >( "configCommands" ) ),
  operationCommands_( params.getParameter<std::vector<std::string> >( "operationCommands" ) ),
  closingCommands_  ( params.getParameter<std::vector<std::string> >( "closingCommands" ) )
{
  reset();
  //const auto& dev_id = fetch( M_DEVICE_ID );
}

Device::~Device()
{
  for ( const auto& c : closingCommands_ )
    send( c );
}

void
Device::reset() const
{
  send( M_RESET );
}

void
Device::initialise() const
{
  for ( const auto& c : configCommands_ )
    send( c );
  for ( const auto& c : operationCommands_ )
    send( c );
}

std::pair<unsigned long, double>
Device::readValue( const std::string command, std::string unit ) const
{
  const auto& rd = fetch( command );
  if ( rd.size() != 1 )
    throw std::runtime_error( "Invalid values read from device!" );

  const auto values = split( rd.at( 0 ), ',' );

  std::smatch res;
  if ( !std::regex_match( values.at( 0 ), res, RGX_NUM_ANSW ) || res.empty() ) {
    throw std::runtime_error( "Failed to parse the value from device: "+values.at( 0 ) );}
  if ( !unit.empty() && res.size() > 1 && res[2] != unit )
    throw std::runtime_error( "Failed to extract value with unit ["+unit+"]" );
  const double value = std::stod( res[1] );

  unsigned long timestamp = 0;
  if ( values.size() > 1 ) {
    //--- at this point, the readback value might be:
    // * value
    // * timestamp
    // * 256
    if ( !std::regex_match( values.at( 1 ), res, RGX_NUM_ANSW ) || res.empty() )
      throw std::runtime_error( "Failed to parse the timestamp from device: "+values.at( 1 ) );
    timestamp = std::stod( res[1] );
  }

  return std::make_pair( timestamp, value );
}
