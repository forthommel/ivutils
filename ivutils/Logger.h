#ifndef ivutils_Logger_h
#define ivutils_Logger_h

#include <iostream>
#include <sstream>

namespace ivutils
{
  enum LogMessageType { error, warning, info };
  class LogMessage
  {
    public:
      LogMessage( const LogMessageType& type ) : type_( type ) {}
      ~LogMessage() {
        switch ( type_ ) {
          case error: std::cout << "[ERROR]"; break;
          case warning: std::cout << "[WARNING]"; break;
          case info: std::cout << "[INFO]"; break;
        }
        std::cout << " " << message_.str() << std::endl;
        if ( type_ == error )
          exit( 0 );
      }

      //----- Overloaded stream operators

      /// Generic templated message feeder operator
      template<typename T>
      inline friend const LogMessage& operator<<( const LogMessage& msg, T var ) {
        LogMessage& nc_msg = const_cast<LogMessage&>( msg );
        nc_msg.message_ << var;
        return msg;
      }
      /// Pipe modifier operator
      inline friend const LogMessage& operator<<( const LogMessage& msg, std::ios_base&( *f )( std::ios_base& ) ) {
        LogMessage& nc_msg = const_cast<LogMessage&>( msg );
        f( nc_msg.message_ );
        return msg;
      }

    private:
      LogMessageType type_;
      /// Message to log
      std::ostringstream message_;

  };
}

#endif
