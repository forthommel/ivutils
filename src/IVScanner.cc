#include "ivutils/IVScanner.h"
#include <iostream>

using namespace ivutils;

IVScanner::IVScanner( const char* config_file ) :
  parser_( config_file ),
  //srcmeter_( parser_.getParameter<ParametersList>( "vsource" ) ),
  ammeter_ ( parser_.getParameter<ParametersList>( "ammeter" ) ),
  ramp_down_      ( parser_.getParameter<bool>( "rampDown" ) ),
  ramping_stages_ ( parser_.getParameter<std::vector<int> >( "Vramp" ) ),
  num_repetitions_( parser_.getParameter<int>( "numRepetitions" ) ),
  stable_time_    ( parser_.getParameter<int>( "stableTime" ) ),
  time_at_test_   ( parser_.getParameter<int>( "timeAtTest" ) ),
  out_file_( "test.out" )
{
  //--- first check if the modules are correct
  { //--- check the ammeter
    const auto& mod = ammeter_.fetch( Device::M_DEVICE_ID );
    if ( mod.empty()
    || ( mod.at( 0 ).find( "KEITHLEY" ) == std::string::npos
      && mod.at( 0 ).find( "MODEL 6487" ) == std::string::npos ) )
      throw std::runtime_error( "Expecting KEITHLEY MODEL 6487, found\n  "+mod.at( 0 )+"\ninstead." );
  }
  /*{ // --- check the sourcemeter
    const auto& mod = srcmeter_.fetch( Device::M_DEVICE_ID );
    if ( mod.empty()
    || ( mod.at( 0 ).find( "KEITHLEY" ) == std::string::npos
      && mod.at( 0 ).find( "MODEL 2410" ) == std::string::npos ) )
      throw std::runtime_error( "Expecting KEITHLEY MODEL 2410, found\n  "+mod.at( 0 )+"\ninstead." );
  }*/
  //const auto& val = ammeter_.readValue();
}

void
IVScanner::rampDown()
{}

void
IVScanner::scan()
{}

void
IVScanner::test() const
{
  ammeter_.send( ":SOUR:VOLT:LEV 1.0" );
  for ( unsigned short i = 0; i < 40; ++i ) {
    const auto& val = ammeter_.readValue();
    out_file_ << val.first << "\t" << val.second << std::endl;
  }
  ammeter_.send( ":SOUR:VOLT:LEV 0" );
}

void
IVScanner::configure()
{
  ammeter_.initialise();
  //srcmeter_.initialise();
}
