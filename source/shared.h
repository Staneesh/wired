#ifndef SHARED_H
#define SHARED_H

enum MessageType
{
	DISCONNECTED,
};

void set_message(char *client_message, enum MessageType t);

#endif
