#include "Pacman.h"
#include "GameInstance.h"

void Pacman::selectNewLocation()
{
	handlePacmanOnGridSegment();
	evaluateQueuedDirection();
	Entity::selectNewLocation();
}

void Pacman::handlePacmanOnGridSegment()
{
	SegmentType& segment = gameInstance.grid.segmentArray[gameInstance.grid.coordsToIndex(targetGridX, targetGridY)];
	switch (segment)
	{
	case SEG_DOT:
		segment = SEG_EMPTY;
		gameInstance.gameState.globalDotCounter++;
		gameInstance.gameState.currentDotCounter++;
		gameInstance.gameState.statDotsEaten++;
		gameInstance.gameState.score += 10;
		break;
	case SEG_POWERDOT:
		segment = SEG_EMPTY;
		gameInstance.gameState.globalDotCounter++;
		gameInstance.gameState.currentDotCounter++;
		gameInstance.gameState.statDotsEaten++;
		gameInstance.entityManager.activateScatter();
		gameInstance.gameState.score += 50;
		break;
	case SEG_FRUIT:
		segment = SEG_EMPTY;
		gameInstance.gameState.globalDotCounter++;
		gameInstance.gameState.currentDotCounter++;
		gameInstance.gameState.statFruitsEaten++;
		gameInstance.gameState.score += 100;
		break;
	}
	
	if (gameInstance.gameState.globalDotCounter == 70)
	{
		gameInstance.grid.spawnFruit();
	}

	if (gameInstance.gameState.globalDotCounter == 170)
	{
		gameInstance.grid.spawnFruit();
	}

	if (gameInstance.gameState.globalDotCounter >= gameInstance.gameState.initialDotCounter)
	{
		gameInstance.gameState.shouldResetGrid = true;
		gameInstance.gameState.currentState = STATE_RESETTING;
	}
}

void Pacman::evaluateQueuedDirection()
{
	if (queuedDirection == MOVE_NONE)
	{
		return;
	}

	if (canMoveToNextGridSpace(true, queuedDirection))
	{
		moveDirection = queuedDirection;
	}
}

void Pacman::requestChangeDirection(MoveDirection requestedDirection)
{
	if (!isTeleporting())
	{
		if (queuedDirection == requestedDirection)
		{
			return;
		}
		else
		{
			queuedDirection = requestedDirection;
			return;
		}
	}
}