#include "EntityManager.h"
#include "GameInstance.h"

void EntityManager::checkActivateGhost()
{
	if (gameInstance.gameState.currentDotCounter >= 0)
	{
		if (activateGhost(ENT_BLINKY))
		{
			return;
		}
		else if (activateGhost(ENT_PINKY))
		{
			return;
		}
	}

	if (gameInstance.gameState.currentDotCounter >= 30)
	{
		if (activateGhost(ENT_INKY))
		{
			return;
		}
	}

	if (gameInstance.gameState.currentDotCounter >= 60)
	{
		activateGhost(ENT_CLYDE);
	}
}

bool EntityManager::activateGhost(EntityType typeToActivate)
{
	for (Ghost* ghost : ghostVector)
	{
		if (ghost->type == typeToActivate && ghost->IsInHouse)
		{
			ghost->CanLeaveHouse = true;
			return true;
		}
		else if (ghost->type == typeToActivate && !ghost->IsInHouse)
		{
			return false;
		}
	}

	return false;
}

void EntityManager::activateScatter()
{
	gameInstance.gameState.ghostScatter = true;
	gameInstance.gameState.ghostScatterTimer = 0.0f;
	for (Ghost* ghost : ghostVector)
	{
		Play::SetSprite(*ghost->obj, "ghostscared", 0.0f);
	}
}

void EntityManager::deactivateScatter()
{
	gameInstance.gameState.ghostScatter = false;
	gameInstance.entityManager.ghostActivationTimer = 0.0f;
	for (Ghost* ghost : ghostVector)
	{
		Play::SetSprite(*ghost->obj, ghost->GhostSpriteMap[ghost->type].data(), 0.0f);
	}
}