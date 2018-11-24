#ifndef ivutils_IVScanner_h
#define ivutils_IVScanner_h

#include "ivutils/PythonParser.h"
#include "ivutils/Device.h"

#include "TApplication.h"
#include <fstream>

namespace ivutils
{
  class IVScanner : public TApplication
  {
    public:
      IVScanner( const char* config_file );
      void configure() const;
      void test() const;

      void rampDown() const;
      void scan() const;

    private:
      void stabilityTest( std::vector<double>& i_ramp, std::vector<double>& i_stable ) const;

      PythonParser parser_;
      /// SourceMeter communication module
      Device srcmeter_;
      /// Ammeter communication module
      Device ammeter_;

      bool ramp_down_;
      std::vector<double> ramping_stages_;
      double v_test_; ///< Voltage to test stability (abs value)
      size_t num_repetitions_; ///< current values per voltage
      unsigned int stable_time_; ///< time for stabilizing after changing voltage (in seconds)
      unsigned int time_at_test_; ///< timein stability test at voltage V_test (in seconds)
      double voltage_at_test_;
  };
}

#endif

