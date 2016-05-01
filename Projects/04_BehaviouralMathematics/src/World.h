#ifndef _WORLD_H_
#define _WORLD_H_

#include "glm/glm.hpp"

class World
{
public:
	World();
	~World() {}

	void Render();

	void AddLogToHouse();
	unsigned int GetHouseLogsRequired() const { return m_houseLogsRequired; }
	unsigned int GetCurrentHouseLogs() const { return m_houseCurrentLogs; }
	bool IsHouseBuilt() { return m_houseCurrentLogs == m_houseLogsRequired; }

	glm::vec3 GetHouseLocation() const { return m_houseLocation; }
	glm::vec3 GetTreeLocation() const { return m_treeLocation; }
	glm::vec3 GetWaterLocation() const { return m_waterLocation; }
	glm::vec3 GetFoodLocation() const { return m_foodLocation; }
	glm::vec3 GetRestedLocation() const { return m_restedLocation; }

	bool InteractWithFood();
	bool InteractWithWater();
	bool InteractWithRested();

	bool InteractWithTree();
	bool InteractWithHouse();
private:
	unsigned int m_houseLogsRequired;
	unsigned int m_houseCurrentLogs;

	glm::vec3 m_houseLocation;
	glm::vec3 m_treeLocation;

	glm::vec3 m_waterLocation;
	glm::vec3 m_foodLocation;
	glm::vec3 m_restedLocation;

	float m_houseInteractTime;
	float m_treeInteractTime;
	float m_foodInteractTime;
	float m_waterInteractTime;
	float m_restedInteractTime;

	float m_lastHouseInteractTime;
	float m_lastTreeInteractTime;
	float m_lastWaterInteractTime;
	float m_lastFoodInteractTime;
	float m_lastRestedInteractTime;
};

#endif // !WORLD_H_
