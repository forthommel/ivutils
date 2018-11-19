#ifndef ivutils_Utils_h
#define ivutils_Utils_h

namespace ivutils
{
  //--- helper function
  std::vector<std::string>
  split( const std::string& str, char delimiter = ',' )
  {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream is( str );
    while (std::getline( is, token, delimiter ) )
      tokens.emplace_back( token );
    return tokens;
  }
}

#endif
