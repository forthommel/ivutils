#ifndef ivutils_Messenger_h
#define ivutils_Messenger_h

#include <array>
#include <vector>
#include <string>

#if defined EMULATE
# include <fstream>
#endif

namespace ivutils
{
  class ParametersList;
  /// Basic communication protocol handler
  class Messenger
  {
    public:
//      Messenger() = default;
      /// Build a messenger at a given address
      explicit Messenger( int prim_addr = -1, int second_addr = 0 );
      ~Messenger();

      /// Send a message to the module
      /// \param[in] msg Command to be transmitted
      void send( std::string msg ) const;
      /// Interrogate the module
      /// \param[in] msg Command to be transmitted
      std::vector<std::string> fetch( const std::string& msg ) const;

    private:
      static const unsigned short ACK_TIME_MS;
      void clear() const;
      /// Retrieve data from the module
      std::vector<std::string> receive() const;

#if defined EMULATE
      mutable std::ofstream cmd_file_;
#endif
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
