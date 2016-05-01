#include "BasicNetworkingApp.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "../../07_Files/GameMessages.h"

#include <iostream>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include <Gizmos.h>
#include <Camera.h>

using std::cout;
using std::endl;
using glm::vec3;
using glm::vec4;
using RakNet::BitStream;
using RakNet::MessageID;

BasicNetworkingApp::BasicNetworkingApp()
{
	m_peerInterface = nullptr;
	m_myColour = vec3(rand() % 2, rand() % 2, rand() % 2);
	m_clientObjectIndex = 0;
}

BasicNetworkingApp::~BasicNetworkingApp()
{
	delete m_peerInterface;
}

bool BasicNetworkingApp::Startup()
{
	CreateGLFWWindow("Client App", 1280, 720);

	Gizmos::Create();

	m_camera = new Camera(glm::pi<float>() * 0.25f, 16.f / 9.f, 0.1f, 1000.f);
	m_camera->SetLookAtFrom(vec3(10, 10, 10), vec3(0));

	HandleNetworkConnection();

	m_pickPosition = vec3(0);

	//m_networkThread = new std::thread(&HandleNetworkMessages);

	return true;
}

void BasicNetworkingApp::Shutdown()
{
	//delete m_networkThread;
	m_peerInterface->Shutdown(1);

	delete m_camera;
	Gizmos::Destroy();

	DestroyGLFWWindow();
}

bool BasicNetworkingApp::Update(float deltaTime)
{
	if (glfwWindowShouldClose(m_window) || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	m_camera->Update(deltaTime);

	HandleNetworkMessages();
	MoveServerObjects(deltaTime);

	MoveClientObject(deltaTime);

	if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS)
	{
		double x = 0;
		double y = 0;
		glfwGetCursorPos(m_window, &x, &y);

		vec4 plane(0, 1, 0, 0);
		m_pickPosition = m_camera->PickAgainstPlane((float)x, (float)y, plane);
	}

	return true;
}

void BasicNetworkingApp::MoveServerObjects(float deltaTime)
{
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].ownerClientID == m_clientId)
			continue;
		switch (m_gameObjects[i].syncType)
		{
		case POSITION_ONLY:
			break;
		case INTERPOLATION:
			m_tempPos = m_tempPos * m_tempVel * deltaTime;
		case LERP:
			m_gameObjects[i].xPos = glm::lerp(m_gameObjects[i].xPos, m_tempPos.x, 0.5f);
			m_gameObjects[i].zPos = glm::lerp(m_gameObjects[i].zPos, m_tempPos.z, 0.5f);
			break;
		}
	}
}

void BasicNetworkingApp::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::Clear();

	Gizmos::AddTransform(glm::translate(m_pickPosition));

	for (int i = 0; i < 21; ++i)
	{
		Gizmos::AddLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));

		Gizmos::AddLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
	}

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject& obj = m_gameObjects[i];
		Gizmos::AddSphere(vec3(obj.xPos, 2, obj.zPos), 2, 32, 32, vec4(obj.redColour, obj.greenColour, obj.blueColour, 1), nullptr);
	}

	Gizmos::Draw(m_camera->GetProjectionView());

	int width = 0;
	int height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	glm::mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::Draw2D(guiMatrix);
}

void BasicNetworkingApp::HandleNetworkConnection()
{
	//Initialize the Raknet peer interface first
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	InitaliseClientConnection();
}

void BasicNetworkingApp::InitaliseClientConnection()
{
	//Create a socket descriptor to describe this connection
	//No data needed, as we will be connecting to a server
	RakNet::SocketDescriptor sd;
	m_peerInterface->Startup(1, &sd, 1);

	cout << "Connecting to server at: " << IP << endl;

	//Now call connect to attempt to connect to the given server
	RakNet::ConnectionAttemptResult res = m_peerInterface->Connect(IP, PORT, nullptr, 0);

	//Finally, check to see if we connected, and if not, throw a error
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		cout << "Unable to start connection, Error number: " << res << endl;
	}
}

void BasicNetworkingApp::HandleNetworkMessages()
{
	RakNet::Packet* packet = nullptr;

	for (packet = m_peerInterface->Receive(); packet; m_peerInterface->DeallocatePacket(packet), packet = m_peerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			cout << "Another client has disconnected." << endl;
			break;
		case ID_REMOTE_CONNECTION_LOST:
			cout << "Another client has lost the connection." << endl;
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			cout << "Another client has connected." << endl;
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			cout << "Our connection request has been accepted." << endl;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			cout << "The server is full." << endl;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			cout << "We have been disconnected." << endl;
			break;
		case ID_CONNECTION_LOST:
			cout << "Connection lost." << endl;
			break;
		case ID_SERVER_TEXT_MESSAGE:
		{
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));

			RakNet::RakString str;
			bsIn.Read(str);
			cout << str.C_String() << endl;
			break;
		}
		case ID_SERVER_CLIENT_ID:
		{
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(m_clientId);

			cout << "Server has given us an id of: " << m_clientId << endl;

			CreateGameObject();

			break;
		}
		case ID_SERVER_FULL_OBJECT_DATA:
		{
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			ReadObjectDataFromServer(bsIn);
			cout << "Server full object data" << endl;
			break;
		}
		default:
			cout << "Received a message with a unknown id: " << packet->data[0] << endl;
			break;
		}
	}
}

void BasicNetworkingApp::ReadObjectDataFromServer(BitStream& bsIn)
{
	//Create a temp object that we will pull all the object data into
	GameObject tempGameObject;

	//Read in the object data
	bsIn.Read(tempGameObject.syncType);
	if (tempGameObject.syncType == INTERPOLATION)
	{
		bsIn.Read(tempGameObject.xVelocity);
		bsIn.Read(tempGameObject.zVelocity);
		tempGameObject.xPos = 0.0f;
		tempGameObject.zPos = 0.0f;
	}
	else
	{
		bsIn.Read(tempGameObject.xPos);
		bsIn.Read(tempGameObject.zPos);
		tempGameObject.xVelocity = 0.0f;
		tempGameObject.zVelocity = 0.0f;
	}
	bsIn.Read(tempGameObject.redColour);
	bsIn.Read(tempGameObject.greenColour);
	bsIn.Read(tempGameObject.blueColour);
	bsIn.Read(tempGameObject.ownerClientID);

	//Check to see whether or not this object is already stored in our local object list
	bool found = false;
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].ownerClientID == tempGameObject.ownerClientID)
		{
			found = true;

			//Update the game object
			GameObject& obj = m_gameObjects[i];
			obj.redColour = tempGameObject.redColour;
			obj.greenColour = tempGameObject.greenColour;
			obj.blueColour = tempGameObject.blueColour;
			obj.syncType = tempGameObject.syncType;

			switch (obj.syncType)
			{
			case POSITION_ONLY:
				obj.xPos = tempGameObject.xPos;
				obj.zPos = tempGameObject.zPos;
				break;
			case INTERPOLATION:
				m_tempVel.x = tempGameObject.xVelocity;
				m_tempVel.z = tempGameObject.zVelocity;
			case LERP:
				m_tempPos.x = tempGameObject.xPos;
				m_tempPos.z = tempGameObject.zPos;
				break;
			}
		}
	}

	//If we didn't find it, then it is a new object - add it to our object list
	if (!found)
	{
		m_gameObjects.push_back(tempGameObject);
		if (tempGameObject.ownerClientID == m_clientId)
			m_clientObjectIndex = m_gameObjects.size() - 1;
	}
}

void BasicNetworkingApp::CreateGameObject()
{
	//Tell the server we want to create a new game object that will represent us
	BitStream bsOut;

	GameObject tempGameObject;
	tempGameObject.xPos = 0.0f;
	tempGameObject.zPos = 0.0f;
	tempGameObject.xVelocity = 0.0f;
	tempGameObject.zVelocity = 0.0f;
	tempGameObject.redColour = m_myColour.r;
	tempGameObject.greenColour = m_myColour.g;
	tempGameObject.blueColour = m_myColour.b;
	tempGameObject.syncType = POSITION_ONLY;

	//Ensure that the write order is the same as the read order on the server!
	bsOut.Write((MessageID)GameMessages::ID_CLIENT_CREATE_OBJECT);
	bsOut.Write(tempGameObject.xPos);
	bsOut.Write(tempGameObject.zPos);
	bsOut.Write(tempGameObject.xVelocity);
	bsOut.Write(tempGameObject.zVelocity);
	bsOut.Write(tempGameObject.redColour);
	bsOut.Write(tempGameObject.greenColour);
	bsOut.Write(tempGameObject.blueColour);
	bsOut.Write(tempGameObject.syncType);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void BasicNetworkingApp::MoveClientObject(float deltaTime)
{
	//We don't have a valid client ID, so we we have no game object!
	if (m_clientId == 0)
		return;

	//No game objects sent to us, so we don't know who we are yet
	if (m_gameObjects.size() == 0)
		return;
	bool updatedObjectPosition = false;

	GameObject& myClientObject = m_gameObjects[m_clientObjectIndex];

	if (glfwGetKey(m_window, GLFW_KEY_UP))
	{
		myClientObject.zPos += 2.f * deltaTime;
		updatedObjectPosition = true;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN))
	{
		myClientObject.zPos -= 2.f * deltaTime;
		updatedObjectPosition = true;
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT))
	{
		myClientObject.xPos += 2.f * deltaTime;
		updatedObjectPosition = true;
	}
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT))
	{
		myClientObject.xPos -= 2.f * deltaTime;
		updatedObjectPosition = true;
	}

	if (updatedObjectPosition)
	{
		if (myClientObject.syncType == INTERPOLATION)
			SendUpdatedObjectVelocityToServer(myClientObject);
		else
			SendUpdatedObjectPositionToServer(myClientObject);
	}
}

void BasicNetworkingApp::SendUpdatedObjectPositionToServer(GameObject& gameObject)
{
	BitStream bsOut;
	bsOut.Write((MessageID)GameMessages::ID_CLIENT_UPDATE_OBJECT_POSITION);
	bsOut.Write(gameObject.xPos);
	bsOut.Write(gameObject.zPos);
	bsOut.Write(gameObject.redColour);
	bsOut.Write(gameObject.greenColour);
	bsOut.Write(gameObject.blueColour);
	bsOut.Write(gameObject.ownerClientID);
	bsOut.Write(gameObject.syncType);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void BasicNetworkingApp::SendUpdatedObjectVelocityToServer(GameObject& gameObject)
{
	BitStream bsOut;
	bsOut.Write((MessageID)GameMessages::ID_CLIENT_UPDATE_OBJECT_VELOCITY);
	bsOut.Write(gameObject.xVelocity);
	bsOut.Write(gameObject.zVelocity);
	bsOut.Write(gameObject.redColour);
	bsOut.Write(gameObject.greenColour);
	bsOut.Write(gameObject.blueColour);
	bsOut.Write(gameObject.ownerClientID);
	bsOut.Write(gameObject.syncType);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}