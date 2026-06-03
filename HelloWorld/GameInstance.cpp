#include "GameInstance.h"

GameInstance::GameInstance()
{
	entityManager = EntityManager();
	grid = Grid();
	gameState = GameState();
}

int GameInstance::randomIntInRange(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}
