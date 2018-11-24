#ifndef ivutils_Utils_h
#define ivutils_Utils_h

#include <cmath>
#include <numeric>

namespace ivutils
{
  //--- helper function
  inline std::vector<std::string>
  split( const std::string& str, char delimiter = ',' )
  {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream is( str );
    while (std::getline( is, token, delimiter ) )
      tokens.emplace_back( token );
    return tokens;
  }

  template<typename T> inline T
  mean( const std::vector<T>& vec )
  {
    return std::accumulate( vec.begin(), vec.end(), 0. )/vec.size();
  }

  template<typename T> inline T
  stdev( const std::vector<T>& vec, T mean )
  {
    //const double sq_sum = std::inner_product( vec.begin(), vec.end(), vec.begin(), 0. )/vec.size();
    //const double stdev_i = std::sqrt( sq_sum-mean*mean );
    std::vector<double> diff( vec.size() );
    std::transform( vec.begin(), vec.end(), diff.begin(), [mean]( double x ) { return x-mean; } );
    return std::sqrt( std::inner_product( diff.begin(), diff.end(), diff.begin(), 0. )/vec.size() );
  }
}

#endif
