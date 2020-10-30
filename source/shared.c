#include "shared.h"

void set_message(char *client_message, enum MessageType t)
{
	client_message[t] = 1;
}
