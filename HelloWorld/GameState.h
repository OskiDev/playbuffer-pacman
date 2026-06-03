#pragma once
enum CurrentState
{
	STATE_MENU,
	STATE_READY,
	STATE_PLAY,
	STATE_RESETTING,
	STATE_END
};

class GameState
{
public:
	int score{ 0 };
	int initialDotCounter{ 0 };
	int globalDotCounter{ 0 };
	int currentDotCounter{ 0 };
	int pacmanLivesRemaining{ 3 };
	bool ghostScatter{ false };
	float ghostScatterTimer{ 0.0f };
	float scatterTimerThreshold{ 6.0f };
	enum CurrentState currentState { STATE_MENU };
	float resetStateTimer{ 0.0f };
	float resetTimerThreshold{ 3.0f };
	float readyStateTimer{ 0.0f };
	float readyTimerThreshold{ 3.0f };
	bool shouldResetGrid{ false };
	int currentCycleGhostCount{ 0 };
	int statDotsEaten{ 0 };
	int statFruitsEaten{ 0 };
	int statGhostsEaten{ 0 };
};