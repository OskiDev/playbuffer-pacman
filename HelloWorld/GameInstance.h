#pragma once
#include "EntityManager.h"
#include "Grid.h"
#include "GameState.h"
#include <random>

class GameInstance
{
public:
	//functions
	GameInstance();
	int randomIntInRange(int min, int max);

	//variables
	EntityManager entityManager;
	Grid grid;
	GameState gameState;
};

extern GameInstance gameInstance;