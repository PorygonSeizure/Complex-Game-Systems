#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

enum SyncType
{
	POSITION_ONLY,
	LERP,
	INTERPOLATION
};

struct GameObject
{
	unsigned int ownerClientID;

	float redColour;
	float greenColour;
	float blueColour;

	float xPos;
	float zPos;

	float xVelocity;
	float zVelocity;

	SyncType syncType;
};

#endif