#ifndef ivutils_Handler_h
#define ivutils_Handler_h

#include "ivutils/Message.h"

#include <vector>
#include <string>
#include <regex>

namespace ivutils
{
  class Handler
  {
    public:
      Handler( const char* config_file );

      /// Send a message to the module
      /// \param[in] msg Command to be transmitted
      void send( const message_t& msg ) const;
      /// Send a list of messages to the module
      /// \param[in] msg_batch List of commands to be transmitted
      void send( const std::vector<message_t>& msg_batch ) const;
      /// Retrieve data from the module
      /// \param[in] msg Command to be transmitted
      template<typename T> T get( const message_t& msg ) const;

    private:
      std::string fetch() const;
      static const std::regex RGX_STR_ANSW, RGX_INT_ANSW, RGX_FLT_ANSW;
      mutable std::string last_command_;
  };
}

#endif

