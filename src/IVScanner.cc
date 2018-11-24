#include "ivutils/IVScanner.h"
#include "ivutils/Utils.h"

#include "TSystem.h"
#include "TFile.h"

#include "TCanvas.h"
#include "TPad.h"
#include "TGraphErrors.h"
#include "TH1.h"

#include <iostream>
#include <functional>
#include <thread>
#include <set>

using namespace ivutils;

IVScanner::IVScanner( const char* config_file ) :
  TApplication( "IVScanner:test", nullptr, nullptr ),
  parser_( config_file ),
  srcmeter_( parser_.getParameter<ParametersList>( "vsource" ) ),
  ammeter_ ( parser_.getParameter<ParametersList>( "ammeter" ) ),
  ramp_down_      ( parser_.getParameter<bool>( "rampDown" ) ),
  ramping_stages_ ( parser_.getParameter<std::vector<double> >( "Vramp" ) ),
  num_repetitions_( parser_.getParameter<int>( "numRepetitions" ) ),
  stable_time_    ( parser_.getParameter<int>( "stableTime" ) ),
  time_at_test_   ( parser_.getParameter<int>( "timeAtTest" ) ),
  voltage_at_test_( parser_.getParameter<double>( "Vtest" ) )
{
  //--- first check if the modules are correct
  { //--- check the ammeter
    const auto& mod = ammeter_.fetch( Device::M_DEVICE_ID );
    if ( mod.empty()
    || ( mod.at( 0 ).find( "KEITHLEY" ) == std::string::npos
      && mod.at( 0 ).find( "MODEL 6487" ) == std::string::npos ) )
      throw std::runtime_error( "Expecting KEITHLEY MODEL 6487, found\n  "+mod.at( 0 )+"\ninstead." );
  }
  { // --- check the sourcemeter
    const auto& mod = srcmeter_.fetch( Device::M_DEVICE_ID );
    if ( mod.empty()
    || ( mod.at( 0 ).find( "KEITHLEY" ) == std::string::npos
      && mod.at( 0 ).find( "MODEL 2410" ) == std::string::npos ) )
      throw std::runtime_error( "Expecting KEITHLEY MODEL 2410, found\n  "+mod.at( 0 )+"\ninstead." );
  }
  //const auto& val = ammeter_.readValue();
}

void
IVScanner::rampDown() const
{
  const auto& v_ini = ammeter_.readValue( ":SOUR:VOLT:LEV?" );
  std::cout << v_ini.second << std::endl;
  //--- first build a decreasing list of (unique) voltage values for the ramp down
  std::set<double,std::greater<double> > vtests( ramping_stages_.begin(), ramping_stages_.end() );
  vtests.insert( 0. ); // ensure we finish there...
  {
    std::cout << "RAMPDOWN: will use the following values:";
    for ( const auto& v : vtests )
      std::cout << " " << v;
    std::cout << " V." << std::endl;
  }
  for ( const auto& v : vtests ) {
    //--- build and send the message to set voltage
    std::ostringstream os;
    os << ":SOUR:VOLT:LEV " << v;
    ammeter_.send( os.str() );
    //srcmeter_.send( os.str() );
    std::this_thread::sleep_for( std::chrono::seconds( stable_time_ ) );
    std::cout << "RAMPDOWN: currently at " << v << " V." << std::endl;
  }
  std::cout << "RAMPDOWN: finished!" << std::endl;
}

void
IVScanner::scan() const
{
  std::unique_ptr<TFile> root_file( TFile::Open( "output_ivscan.root", "recreate" ) );
  TGraphErrors gr_meas;
  gr_meas.SetName( "iv_scan" );
  gr_meas.SetTitle( ";Bias (V);Leakage current (A)" );
  gr_meas.Draw( "alp" );
  gr_meas.SetMarkerStyle( 24 );
  gr_meas.SetLineWidth( 2 );

  int i = 0;
  for ( const auto& vr : ramping_stages_ ) {
    std::cout << "RAMPING: currently at " << vr << " V." << std::endl;
    { //--- build and send the message to set voltage
      std::ostringstream os;
      os << ":SOUR:VOLT:LEV " << vr;
      srcmeter_.send( os.str() );
    }

    //--- output values while ramping and at stabilisation time
    std::vector<double> i_ramp, i_stable;
    if ( abs( vr ) == voltage_at_test_ ) //--- measure currents at test voltage
      stabilityTest( i_ramp, i_stable );
    else { //--- measure currents while ramping voltage
      std::this_thread::sleep_for( std::chrono::seconds( stable_time_ ) );
      for ( unsigned short j = 0; j < num_repetitions_; ++j ) {
        //--- read current value
        const auto& val_at_time = ammeter_.readValue( Device::M_READ, "A" );
        i_ramp.emplace_back( val_at_time.second );
      }
    }
    //--- calculate the mean and standard deviation
    const double mean_i = mean( i_ramp ), stdev_i = stdev( i_ramp, mean_i );
    std::cout
      << "Measurement " << i+1 << "/" << ramping_stages_.size() << ": "
      << vr << " V, "
      << "Current = " << mean_i << " +- " << stdev_i << " A."
      << std::endl;
    gr_meas.SetPoint( i, vr, mean_i );
    gr_meas.SetPointError( i, 0., stdev_i );
    gSystem->ProcessEvents();
    gPad->Modified();
    gPad->Update();
    ++i;
  }

  if ( ramp_down_ )
    rampDown();

  gr_meas.Write();
  root_file->Close();
}

void
IVScanner::stabilityTest( std::vector<double>& i_ramp, std::vector<double>& i_stable ) const
{
  std::cout << "Stability test ongoing, please wait:" << std::endl;
  int n = 0;
  auto start = std::chrono::system_clock::now();

  while ( std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now()-start ).count() < time_at_test_ ) {
    //--- necessary wait between two measurements of current value
    std::this_thread::sleep_for( std::chrono::seconds( stable_time_ ) );
    const auto& val_at_time = ammeter_.readValue( Device::M_READ, "A" );
    if ( n++ < num_repetitions_ )
      i_ramp.emplace_back( val_at_time.second );
    else
      i_stable.emplace_back( val_at_time.second );
  }
  std::cout << "Stability test finished!" << std::endl;
}

void
IVScanner::test() const
{
  //--- prepare outputs
  std::ofstream out_file( "test.out" );
  std::unique_ptr<TFile> root_file( TFile::Open( "output.root", "recreate" ) );

  TGraph g_curr;
  g_curr.SetTitle( ";Timestamp (s);Leakage current (pA)" );
  TH1D h_curr( "h_curr", ";Leakage current (pA);Measurements", 100, 0., 0.1 );

  TCanvas c;
  c.Divide( 1, 2 );
  c.cd( 1 );
  g_curr.Draw( "alp" );
  c.cd( 2 );
  h_curr.Draw();

  //--- launch the acquisition
  ammeter_.send( ":SOUR:VOLT:LEV 1.0" );
  for ( unsigned short i = 0; i < 1000; ++i ) {
    const auto& val = ammeter_.readValue();
    out_file << val.first << "\t" << val.second << std::endl;
    g_curr.SetPoint( g_curr.GetN(), val.first, val.second*1.e12 );
    h_curr.Fill( val.second*1.e12 );
    gSystem->ProcessEvents();
    gPad->Modified();
    gPad->Update();
  }
  ammeter_.send( ":SOUR:VOLT:LEV 0" );

  //--- write down everything
  c.Write();
  root_file->Close();
  out_file.close();
}

void
IVScanner::configure() const
{
  ammeter_.initialise();
  //srcmeter_.initialise();
}
