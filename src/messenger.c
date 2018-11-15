#include "ivutils/messenger.h"
#include "ni488.h"

void
ivutils_messenger_init( module_t* mod, unsigned short addr )
{
  int status = ibsic(addr);
}

void
ivutils_messenger_send( module_t mod, message_t msg )
{
}

void
ivutils_messenger_get( module_t mod, message_t msg, char* out )
{
}

void
ivutils_messenger_free( module_t mod )
{
}

