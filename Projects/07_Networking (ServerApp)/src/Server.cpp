#include "Server.h"

using std::cout;
using std::endl;
using RakNet::SystemAddress;
using RakNet::BitStream;
using RakNet::MessageID;

Server::Server()
{
	//initialize the Raknet peer interface first
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	m_peerInterface->ApplyNetworkSimulator(0.f, 150, 20);

	m_connectionCounter = 1;
	m_objectCounter = 1;
}

void Server::Run()
{
	//startup the server, and start it listening to clients
	cout << "Starting up the server..." << endl;

	//create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	//now call startup - max of 32 connections, on the assigned port
	m_peerInterface->Startup(32, &sd, 1);
	m_peerInterface->SetMaximumIncomingConnections(32);

	HandleNetworkMessages();
}

void Server::HandleNetworkMessages()
{
	RakNet::Packet* packet = nullptr;

	while (true)
	{
		for (packet = m_peerInterface->Receive(); packet; m_peerInterface->DeallocatePacket(packet), packet = m_peerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
			{
				AddNewConnection(packet->systemAddress);
				cout << "A connection is incoming." << endl;
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
				cout << "A client has disconnected." << endl;
				RemoveConnection(packet->systemAddress);
				break;
			case ID_CONNECTION_LOST:
				cout << "A client lost the connection." << endl;
				RemoveConnection(packet->systemAddress);
				break;
			case ID_CLIENT_CREATE_OBJECT:
			{
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				CreateNewObject(bsIn, packet->systemAddress);
				SendAllGameObjectsToClient(packet->systemAddress);
				break;
			}
			case ID_CLIENT_UPDATE_OBJECT_POSITION:
			{
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				UpdateObjectPosition(bsIn, packet->systemAddress);
				break;
			}
			case ID_CLIENT_UPDATE_OBJECT_VELOCITY:
			{
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				UpdateObjectVelocity(bsIn, packet->systemAddress);
				break;
			}
			default:
				cout << "Received a message with a unknown id: " << packet->data[0] << endl;
				break;
			}
		}
	}
}

void Server::AddNewConnection(SystemAddress systemAddress)
{
	ConnectionInfo info;
	info.sysAddress = systemAddress;
	info.connectionID = m_connectionCounter++;
	m_connectedClients[info.connectionID] = info;

	SendClientIDToClient(info.connectionID);
}

void Server::RemoveConnection(SystemAddress systemAddress)
{
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++)
	{
		if (it->second.sysAddress == systemAddress)
		{
			m_connectedClients.erase(it);
			break;
		}
	}
}

unsigned int Server::SystemAddressToClientID(SystemAddress& systemAddress)
{
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++)
	{
		if (it->second.sysAddress == systemAddress)
		{
			return it->first;
		}
	}

	return 0;
}

void Server::SendClientIDToClient(unsigned int clientID)
{
	BitStream bs;
	bs.Write((MessageID)GameMessages::ID_SERVER_CLIENT_ID);
	bs.Write(clientID);

	m_peerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_connectedClients[clientID].sysAddress, false);
}

void Server::CreateNewObject(BitStream& bsIn, SystemAddress& ownerSysAddress)
{
	GameObject newGameObject;

	//Read in the infomation from the packet
	bsIn.Read(newGameObject.xPos);
	bsIn.Read(newGameObject.zPos);
	bsIn.Read(newGameObject.xVelocity);
	bsIn.Read(newGameObject.zVelocity);
	bsIn.Read(newGameObject.redColour);
	bsIn.Read(newGameObject.greenColour);
	bsIn.Read(newGameObject.blueColour);
	bsIn.Read(newGameObject.syncType);

	newGameObject.ownerClientID = SystemAddressToClientID(ownerSysAddress);

	m_gameObjects.push_back(newGameObject);

	SendGameObjectToAllClients(&newGameObject, ownerSysAddress);
}

void Server::UpdateObjectPosition(BitStream& bsIn, SystemAddress& ownerSysAddress)
{
	GameObject updatedGameObject;

	//Read in the infomation from the packet
	//bsIn.Read(updatedGameObject);
	bsIn.Read(updatedGameObject.xPos);
	bsIn.Read(updatedGameObject.zPos);
	bsIn.Read(updatedGameObject.redColour);
	bsIn.Read(updatedGameObject.greenColour);
	bsIn.Read(updatedGameObject.blueColour);
	bsIn.Read(updatedGameObject.syncType);

	updatedGameObject.ownerClientID = SystemAddressToClientID(ownerSysAddress);

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].ownerClientID == updatedGameObject.ownerClientID)
			m_gameObjects[i] = updatedGameObject;
	}

	SendGameObjectPositionToAllClients(&updatedGameObject, ownerSysAddress);
}

void Server::UpdateObjectVelocity(BitStream& bsIn, SystemAddress& ownerSysAddress)
{
	GameObject updatedGameObject;

	//Read in the infomation from the packet
	//bsIn.Read(updatedGameObject);
	bsIn.Read(updatedGameObject.xVelocity);
	bsIn.Read(updatedGameObject.zVelocity);
	bsIn.Read(updatedGameObject.redColour);
	bsIn.Read(updatedGameObject.greenColour);
	bsIn.Read(updatedGameObject.blueColour);
	bsIn.Read(updatedGameObject.syncType);

	updatedGameObject.ownerClientID = SystemAddressToClientID(ownerSysAddress);

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].ownerClientID == updatedGameObject.ownerClientID)
			m_gameObjects[i] = updatedGameObject;
	}

	SendGameObjectVelocityToAllClients(&updatedGameObject, ownerSysAddress);
}

void Server::SendGameObjectToAllClients(GameObject* gameObject, SystemAddress ownerSystemAddress)
{
	BitStream bsOut;
	bsOut.Write((MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject->xPos);
	bsOut.Write(gameObject->zPos);
	bsOut.Write(gameObject->xVelocity);
	bsOut.Write(gameObject->zVelocity);
	bsOut.Write(gameObject->redColour);
	bsOut.Write(gameObject->greenColour);
	bsOut.Write(gameObject->blueColour);
	bsOut.Write(gameObject->ownerClientID);
	bsOut.Write(gameObject->syncType);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Server::SendGameObjectPositionToAllClients(GameObject* gameObject, SystemAddress ownerSystemAddress)
{
	BitStream bsOut;
	bsOut.Write((MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject->xPos);
	bsOut.Write(gameObject->zPos);
	bsOut.Write(gameObject->redColour);
	bsOut.Write(gameObject->greenColour);
	bsOut.Write(gameObject->blueColour);
	bsOut.Write(gameObject->ownerClientID);
	bsOut.Write(gameObject->syncType);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Server::SendGameObjectVelocityToAllClients(GameObject* gameObject, SystemAddress ownerSystemAddress)
{
	BitStream bsOut;
	bsOut.Write((MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject->xVelocity);
	bsOut.Write(gameObject->zVelocity);
	bsOut.Write(gameObject->redColour);
	bsOut.Write(gameObject->greenColour);
	bsOut.Write(gameObject->blueColour);
	bsOut.Write(gameObject->ownerClientID);
	bsOut.Write(gameObject->syncType);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Server::SendAllGameObjectsToClient(SystemAddress ownerSystemAddress)
{
	BitStream bsOut;
	bsOut.Write((MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		bsOut.Write(m_gameObjects[i].xPos);
		bsOut.Write(m_gameObjects[i].zPos);
		bsOut.Write(m_gameObjects[i].xVelocity);
		bsOut.Write(m_gameObjects[i].zVelocity);
		bsOut.Write(m_gameObjects[i].redColour);
		bsOut.Write(m_gameObjects[i].greenColour);
		bsOut.Write(m_gameObjects[i].blueColour);
		bsOut.Write(m_gameObjects[i].ownerClientID);
		bsOut.Write(m_gameObjects[i].syncType);
	}

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ownerSystemAddress, false);
}