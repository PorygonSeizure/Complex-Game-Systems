#ifndef _GAME_MESSAGE_H_
#define _GAME_MESSAGE_H_

#include <MessageIdentifiers.h>

enum GameMessages
{
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_SERVER_CLIENT_ID = ID_USER_PACKET_ENUM + 2,
	ID_CLIENT_CREATE_OBJECT = ID_USER_PACKET_ENUM + 3,
	ID_SERVER_FULL_OBJECT_DATA = ID_USER_PACKET_ENUM + 4,
	ID_CLIENT_UPDATE_OBJECT_POSITION = ID_USER_PACKET_ENUM + 5,
	ID_CLIENT_UPDATE_OBJECT_VELOCITY = ID_USER_PACKET_ENUM + 6
};

#endif