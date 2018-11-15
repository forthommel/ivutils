#ifndef ivutils_handler_h
#define ivutils_handler_h

#include "ivutils/messages.h"

typedef unsigned short module_t;

/// Initialise an interfacing module to control modules
void ivutils_messenger_init( module_t* mod, unsigned short addr );
void ivutils_messenger_send( module_t mod, message_t );
void ivutils_messenger_get( module_t mod, message_t, char* );
void ivutils_messenger_free( module_t mod );

#endif

