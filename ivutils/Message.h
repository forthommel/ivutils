#ifndef ivutils_Message_h
#define ivutils_Message_h

namespace ivutils
{
  /// Common type for all command/messages
  typedef const char* message_t;
  /// A useful collection of pre-recorded messages
  namespace msg
  {
    static constexpr message_t reset         = "*RST";
    static constexpr message_t initialise    = "INIT";
    static constexpr message_t read          = ":READ?";
    static constexpr message_t boardId       = "*IDN?";
    static constexpr message_t enableOutput  = ":OUTP ON";
    static constexpr message_t disableOutput = ":OUTP OFF";
  }
}

#endif

