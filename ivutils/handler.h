#ifndef ivutils_handler_h
#define ivutils_handler_h

#include "ivutils/messages.h"

/// Initialise an interfacing module to control modules
void ivutils_hdlr_init( const char* config_file );
void ivutils_hdlr_send( message_t );
void ivutils_hdlr_probe( message_t, char* );
void ivutils_hdlr_free();

#endif

