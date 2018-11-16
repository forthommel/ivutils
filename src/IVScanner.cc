#include "ivutils/IVScanner.h"

using namespace ivutils;

IVScanner::IVScanner( const char* config_file ) :
  parser_( config_file )
  /*bool ramp_down_;
  double v_test_; ///< Voltage to test stability (abs value)
  size_t num_repetitions_; ///< current values per voltage
  unsigned int stable_time_; ///< time for stabilizing after changing voltage (in seconds)
  unsigned int time_at_test_; ///< timein stability test at voltage V_test (in seconds)*/
{}

void
IVScanner::rampDown()
{}

void
IVScanner::scan()
{}

void
IVScanner::configure()
{}
