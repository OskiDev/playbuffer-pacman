#pragma once
#include "Play.h"
#include "Entity.h"
#include "Ghost.h"
#include "Pacman.h"

class EntityManager
{
public:
	void checkActivateGhost();
	bool activateGhost(EntityType typeToActivate);
	void activateScatter();
	void deactivateScatter();

	std::vector<Entity*> entityVector{};
	std::vector<Ghost*> ghostVector{};
	float activationTimerThreshold{ 1.0f };
	float ghostActivationTimer{ 0.0f };
};