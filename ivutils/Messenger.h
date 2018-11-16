#ifndef ivutils_Messenger_h
#define ivutils_Messenger_h

#include <array>
#include <vector>
#include <string>
#include <regex>

namespace ivutils
{
  class ParametersList;
  /// Basic communication protocol handler
  class Messenger
  {
    public:
//      Messenger() = default;
      /// Build a messenger at a given address
      explicit Messenger( int prim_addr, int second_addr = 0 );

      /// Send a message to the module
      /// \param[in] msg Command to be transmitted
      void send( const std::string& msg ) const;
      /// Interrogate the module
      /// \param[in] msg Command to be transmitted
      std::vector<std::string> fetch( const std::string& msg ) const;

    private:
      /// Retrieve data from the module
      std::vector<std::string> receive() const;

      static const std::regex RGX_STR_ANSW, RGX_INT_ANSW, RGX_FLT_ANSW;
      mutable std::string last_command_;
      std::vector<std::string> configCommands_;
      std::vector<std::string> operationCommands_;
      std::vector<std::string> closingCommands_;
      // device handlers
      int device_; ///< Device descriptor
      std::array<char,100> buffer_;
  };
}

#endif
