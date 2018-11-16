#include "ivutils/IVScanner.h"
#include <iostream>

using namespace ivutils;

IVScanner::IVScanner( const char* config_file ) :
  parser_( config_file ),
  srcmeter_( parser_.getParameter<ParametersList>( "vsource" ) ),
  ammeter_ ( parser_.getParameter<ParametersList>( "ammeter" ) ),
  ramp_down_      ( parser_.getParameter<bool>( "rampDown" ) ),
  num_repetitions_( parser_.getParameter<int>( "numRepetitions" ) ),
  stable_time_    ( parser_.getParameter<int>( "stableTime" ) ),
  time_at_test_   ( parser_.getParameter<int>( "timeAtTest" ) )
{
  //std::cout << "---> " << parser_.getParameter<ParametersList>( "ammeter" );
}

void
IVScanner::rampDown()
{}

void
IVScanner::scan()
{}

void
IVScanner::configure()
{}
