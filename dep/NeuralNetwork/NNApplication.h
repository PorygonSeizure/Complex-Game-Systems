#pragma once

#include "BaseApplication.h"

#include "Agent.h"
#include "Turret.h"
#include "Food.h"
#include "LinearZone.h"

class NNApplication : public BaseApplication {
public:

	NNApplication();
	virtual ~NNApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	void setUpSimulation();
	void setUpAgents();
	void setUpTurrets();
	void setUpFood();
	void setUpSimpleLinearZone();
	void updateAgents(float delta);
	void drawAgents();
	void addAgentGizmos();
	void addTurretWidgets();
	void addFoodWidgets();
	void checkAgentDamage(Agent* agent);
	void checkAgentFood(Agent* agent);

	float simulateTurret(glm::vec2&, float, Agent* agent);

	static const int MAX_AGENTS = 1;
	static const int MAX_TURRETS = 4;
	static const int MAX_FOOD = 4;

	Agent		agents[MAX_AGENTS];
	Turret		turrets[MAX_TURRETS];
	Food		foods[MAX_FOOD];
	LinearZone	linearZone;
};