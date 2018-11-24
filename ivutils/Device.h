#ifndef ivutils_Device_h
#define ivutils_Device_h

#include "ivutils/Messenger.h"

#include <vector>
#include <string>
#include <regex>

namespace ivutils
{
  class ParametersList;
  /// Basic communication protocol handler
  class Device : public Messenger
  {
    public:
      static const std::string M_DEVICE_ID, M_RESET, M_READ;

      Device() {}
      ~Device();
      /// Build a messenger at a list of parameters
      explicit Device( const ParametersList& params );
      void reset() const;
      void initialise() const;

      std::pair<unsigned long,double> readValue( const std::string command = M_READ, std::string unit = "" ) const;

    private:
      static const std::regex RGX_STR_ANSW, RGX_NUM_ANSW;
      std::vector<std::string> configCommands_;
      std::vector<std::string> operationCommands_;
      std::vector<std::string> closingCommands_;
  };
}

#endif
