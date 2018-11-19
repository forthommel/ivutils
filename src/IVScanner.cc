#include "ivutils/IVScanner.h"

#include "TSystem.h"
#include "TFile.h"

#include "TCanvas.h"
#include "TPad.h"
#include "TGraphErrors.h"
#include "TH1.h"

#include <iostream>
#include <thread>
#include <numeric>

using namespace ivutils;

IVScanner::IVScanner( const char* config_file ) :
  TApplication( "IVScanner:test", nullptr, nullptr ),
  parser_( config_file ),
  //srcmeter_( parser_.getParameter<ParametersList>( "vsource" ) ),
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
IVScanner::rampDown() const
{}

void
IVScanner::scan() const
{
  TApplication( "IVScanner:scan", nullptr, nullptr );
  std::unique_ptr<TFile> root_file( TFile::Open( "output_ivscan.root", "recreate" ) );
  TCanvas r_canv_iv;

  //--- stability tests
  stabilityTest();

  //srcmeter_.

  if ( ramp_down_ )
    rampDown();

  r_canv_iv.Write();
  root_file->Close();
}

void
IVScanner::stabilityTest() const
{
  unsigned short i = 0;
  for ( const auto& vr : ramping_stages_ ) {
    std::cout << "RAMPING: currently at " << vr << " V." << std::endl;
    //--- build and send the message to set voltage
    std::ostringstream os;
    os << ":SOUR:VOLT:LEV " << vr;
    ammeter_.send( os.str() );
    //srcmeter_.send( os.str() );

    TGraphErrors gr_meas;
    gr_meas.Draw( "alp" );
    //gr_meas.SetLineColor( kRed+1 );
    //gr_meas.SetFillColor( kRed+1 );
    //gr_meas.SetFillStyle( 3010 );

    auto start = std::chrono::system_clock::now();

    int n = 0;
    std::vector<double> i_ramp, i_stable;
    if ( abs( vr ) == voltage_at_test_ ) { //--- measure currents at test voltage
      std::cout << "Stability test ongoing, please wait:" << std::endl;
      while ( ( std::chrono::system_clock::now()-start ).count() < time_at_test_ ) {
        if ( n++ < num_repetitions_ && ( std::chrono::system_clock::now()-start ).count() > stable_time_ ) {
          //--- give command to send current readings shown on screen
          const auto& val_at_time = ammeter_.readValue( Device::M_READ );
          i_ramp.emplace_back( val_at_time.second );
        }
        else {
          //--- give command to send current readings shown on screen
          const auto& val_at_time = ammeter_.readValue( Device::M_READ );
          i_stable.emplace_back( val_at_time.second );
        }
      }
      std::cout << "Stability test finished!" << std::endl;
    }
    else { //--- measure currents while ramping voltage
      std::this_thread::sleep_for( std::chrono::seconds( stable_time_ ) );
      for ( unsigned short j = 0; j < num_repetitions_; ++j ) {
        //--- give command to send current readings shown on screen
        const auto& val_at_time = ammeter_.readValue( Device::M_READ );
        i_ramp.emplace_back( val_at_time.second );
      }
    }
    //--- calculate the mean and standard deviation
    const double mean_i = std::accumulate( i_ramp.begin(), i_ramp.end(), 0. )/i_ramp.size();
    //const double sq_sum = std::inner_product( i_ramp.begin(), i_ramp.end(), i_ramp.begin(), 0. )/i_ramp.size();
    //const double stdev_i = std::sqrt( sq_sum-mean_i*mean_i );
    std::vector<double> diff( i_ramp.size() );
    std::transform( i_ramp.begin(), i_ramp.end(), diff.begin(), [mean_i]( double x ) { return x-mean_i; } );
    const double stdev_i = std::sqrt( std::inner_product( diff.begin(), diff.end(), diff.begin(), 0. )/i_ramp.size() );
    std::cout
      << "Measurement " << i+1 << "/" << ramping_stages_.size() << ": "
      << vr << " V, "
      << "Current = " << mean_i << " +- " << stdev_i << " A."
      << std::endl;
    gr_meas.SetPoint( i, (double)i, mean_i );
    gr_meas.SetPointError( i, 0., stdev_i );
    gSystem->ProcessEvents();
    gPad->Modified();
    gPad->Update();
    ++i;
  }
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
