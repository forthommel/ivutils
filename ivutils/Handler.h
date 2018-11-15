#ifndef ivutils_Handler_h
#define ivutils_Handler_h

#include "ivutils/Message.h"

#include <string>
#include <regex>

namespace ivutils
{
  class Handler
  {
    public:
      Handler( const char* config_file );

      void send( const message_t& ) const;
      template<typename T> T get( const message_t& ) const;

    private:
      std::string fetch() const;
      static const std::regex RGX_STR_ANSW, RGX_INT_ANSW, RGX_FLT_ANSW;
      mutable std::string last_command_;
  };
}

#endif

