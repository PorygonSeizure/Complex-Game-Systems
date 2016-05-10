#ifndef _MEMORY_H_
#define _MEMORY_H_

enum Death
{
	NOCONTACT,
	STARVED,
	DEHYDRATED,
	KILLED,
	NOCONTACTCLOSERTOFOOD,
	NOCONTACTCLOSERTOWATER,
	KILLEDCLOSERTOFOOD,
	KILLEDCLOSERTOWATER
};

struct Memory	//memory of single agent's life
{
	int generation;
	float food;
	float water;
	float enemy;
	float time;
	Death death;
	float rankWeight;
};

#endif