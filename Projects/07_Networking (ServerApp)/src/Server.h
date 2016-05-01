#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <string>

#include <thread>
#include <chrono>
#include <unordered_map>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "../../07_Files/GameMessages.h"
#include "../../07_Files/GameObject.h"

class Server
{
public:
	Server();
	~Server() {};

	void Run();

protected:
	void HandleNetworkMessages();
	unsigned int SystemAddressToClientID(RakNet::SystemAddress& systemAddress);

	//connection functions
	void AddNewConnection(RakNet::SystemAddress systemAddress);
	void RemoveConnection(RakNet::SystemAddress systemAddress);

	void SendClientIDToClient(unsigned int clientID);

	void CreateNewObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress);
	void UpdateObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress);
	void UpdateObjectPosition(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress);
	void UpdateObjectVelocity(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress);
	void SendGameObjectToAllClients(GameObject* gameObject, RakNet::SystemAddress ownerSystemAddress);
	void SendGameObjectPositionToAllClients(GameObject* gameObject, RakNet::SystemAddress ownerSystemAddress);
	void SendGameObjectVelocityToAllClients(GameObject* gameObject, RakNet::SystemAddress ownerSystemAddress);
	void SendAllGameObjectsToClient(RakNet::SystemAddress ownerSystemAddress);

private:
	struct ConnectionInfo
	{
		unsigned int connectionID;
		RakNet::SystemAddress sysAddress;
	};

	const unsigned short PORT = 5456;

	RakNet::RakPeerInterface* m_peerInterface;

	unsigned int m_connectionCounter;
	std::unordered_map<unsigned int, ConnectionInfo> m_connectedClients;
	std::vector<GameObject> m_gameObjects;
	unsigned int m_objectCounter;
};

#endif