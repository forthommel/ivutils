#ifndef ivutils_IVScanner_h
#define ivutils_IVScanner_h

#include "ivutils/PythonParser.h"
#include "ivutils/Device.h"

namespace ivutils
{
  class IVScanner
  {
    public:
      IVScanner( const char* config_file );

      void rampDown();
      void scan();

    private:
      void configure();

      PythonParser parser_;
      /// SourceMeter communication module
      Device srcmeter_;
      /// Ammeter communication module
      Device ammeter_;

      bool ramp_down_;
      std::vector<int> ramping_stages_;
      double v_test_; ///< Voltage to test stability (abs value)
      size_t num_repetitions_; ///< current values per voltage
      unsigned int stable_time_; ///< time for stabilizing after changing voltage (in seconds)
      unsigned int time_at_test_; ///< timein stability test at voltage V_test (in seconds)

  };
}

#endif

