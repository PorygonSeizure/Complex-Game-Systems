#ifndef _BASIC_NETWORKING_APP_H_
#define _BASIC_NETWORKING_APP_H_

#include "BaseApp.h"
#include <glm/glm.hpp>
#include <vector>
#include "../../07_Files/GameObject.h"
#include <thread>
#include <chrono>

class Camera;

namespace RakNet
{
	class RakPeerInterface;
	class BitStream;
}

class BasicNetworkingApp : public BaseApp
{
public:
	BasicNetworkingApp();
	virtual ~BasicNetworkingApp();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);

	virtual void Draw();

	//Initialize the connection
	void HandleNetworkConnection();
	void InitaliseClientConnection();

	//Handle incoming packets
	void HandleNetworkMessages();
	void MoveServerObjects(float deltaTime);

	void ReadObjectDataFromServer(RakNet::BitStream& bsIn);
	void CreateGameObject();
	void MoveClientObject(float deltaTime);
	void SendUpdatedObjectPositionToServer(GameObject& gameObject);
	void SendUpdatedObjectVelocityToServer(GameObject& gameObject);

private:
	Camera* m_camera;
	glm::vec3 m_pickPosition;
	glm::vec3 m_myColour;
	std::vector<GameObject> m_gameObjects;
	glm::vec3 m_tempPos;
	glm::vec3 m_tempVel;

	RakNet::RakPeerInterface* m_peerInterface;

	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;

	unsigned int m_clientId;
	unsigned int m_clientObjectIndex;

	//std::thread* m_networkThread;
};

#endif