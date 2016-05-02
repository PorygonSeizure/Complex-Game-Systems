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

struct Memory
{
	int generation;
	float food;
	float water;
	float enemy;
	float time;
	Death death;
	int rank;
	float rankWeight;
};

#endif