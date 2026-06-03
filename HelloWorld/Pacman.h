#pragma once
#include "Entity.h"

class Pacman : public Entity
{
public:
	void selectNewLocation() override;
	void handlePacmanOnGridSegment();
	void evaluateQueuedDirection();
	void requestChangeDirection(MoveDirection requestedDirection);
};