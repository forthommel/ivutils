#include "ivutils/Device.h"
#include "ivutils/ParametersList.h"
#include "ivutils/Utils.h"
#include "ivutils/Logger.h"

#include <iostream>

using namespace ivutils;

const std::string Device::M_DEVICE_ID = "*IDN?";
const std::string Device::M_RESET = "*RST";
const std::string Device::M_READ = ":READ?";

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

  double value = 0.;
  unsigned long timestamp = 0;
  try {
    value = std::stod( values.at( 0 ) );
  } catch ( const std::invalid_argument& ) {
    throw std::runtime_error( "Failed to parse the answer from device: "+values.at( 0 ) );
  }
  if ( values.size() > 1 )
    try {
      timestamp = std::stol( values.at( 1 ) );
    } catch ( const std::invalid_argument& ) {
      throw std::runtime_error( "Failed to parse the timestamp from device: "+values.at( 1 ) );
    }
  return std::make_pair( timestamp, value );
}
