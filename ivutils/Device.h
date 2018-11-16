#ifndef ivutils_Device_h
#define ivutils_Device_h

#include "ivutils/Messenger.h"

#include <vector>
#include <string>

namespace ivutils
{
  class ParametersList;
  /// Basic communication protocol handler
  class Device : private Messenger
  {
    public:
      static const std::string M_DEVICE_ID;

      Device() = default;
      /// Build a messenger at a list of parameters
      explicit Device( const ParametersList& params );

    private:
      std::vector<std::string> configCommands_;
      std::vector<std::string> operationCommands_;
      std::vector<std::string> closingCommands_;
  };
}

#endif
