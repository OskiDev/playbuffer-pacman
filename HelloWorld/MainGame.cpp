#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "InputManager.h"
#include "GameInstance.h"

#pragma region Variables
#pragma region Collections
std::unordered_map<MoveDirection, float> pacmanRotationMap = { {MOVE_NONE, 0.0f}, {MOVE_UP, 4.71f},{MOVE_LEFT, 0.0f}, {MOVE_RIGHT, 3.14f}, {MOVE_DOWN, 1.57f} };
std::vector<int> topScores;
#pragma endregion

#pragma region Single Types
Pacman Pacman;
Ghost Blinky;
Ghost Inky;
Ghost Pinky;
Ghost Clyde;

InputManager inputManager;
GameInstance gameInstance;
#pragma endregion
#pragma endregion

#pragma region Function Declarations
void PlayerSetup();
void GhostSetup();
void handleAllEntityMovement();
void handleAllEntityCollision();
void drawAllEntities();
void populateEntityManagerVectors();
void drawTopScreenText();
void resetEntities();
void startGame();
#pragma endregion

#pragma region Main Game Code
// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	gameInstance.grid.setup();
	PlayerSetup();
	GhostSetup();
	populateEntityManagerVectors();
	CentreMatchingSpriteOrigins("pacman");
	CentreMatchingSpriteOrigins("ghost");
	inputManager.BindKey(Play::KEY_ENTER, startGame, false);

	std::ifstream scoresFileRead("scores.txt");
	if (scoresFileRead.is_open())
	{
		int scoreNumber;
		while (scoresFileRead >> scoreNumber)
		{
			topScores.push_back(scoreNumber);
		}
	}
	scoresFileRead.close();

	sort(topScores.begin(), topScores.end());
	reverse(topScores.begin(), topScores.end());
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	Play::ClearDrawingBuffer(Play::cBlack);
	inputManager.Update();

	switch (gameInstance.gameState.currentState)
	{
	case STATE_MENU:
		Play::DrawFontText("72px", "PRESS ENTER TO START", { DISPLAY_WIDTH / 2, 80 }, Play::CENTRE);
		Play::DrawFontText("72px", "TOP SCORES", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 100 }, Play::CENTRE);
		Play::DrawFontText("72px", "1. " + std::to_string(topScores[0]), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 220 }, Play::CENTRE);
		Play::DrawFontText("72px", "2. " + std::to_string(topScores[1]), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 320 }, Play::CENTRE);
		Play::DrawFontText("72px", "3. " + std::to_string(topScores[2]), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 420 }, Play::CENTRE);
		break;

	case STATE_READY:
		gameInstance.gameState.readyStateTimer += elapsedTime;
		gameInstance.grid.draw();
		drawAllEntities();
		drawTopScreenText();
		Play::DrawFontText("64px", "Ready?", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 430 }, Play::CENTRE);
		if (gameInstance.gameState.readyStateTimer > gameInstance.gameState.readyTimerThreshold)
		{
			gameInstance.gameState.currentState = STATE_PLAY;
		}
		break;
		
	case STATE_PLAY:
		if (!gameInstance.gameState.ghostScatter)
		{
			gameInstance.entityManager.ghostActivationTimer += elapsedTime;
			if (gameInstance.entityManager.ghostActivationTimer >= gameInstance.entityManager.activationTimerThreshold)
			{
				gameInstance.entityManager.checkActivateGhost();
				gameInstance.entityManager.ghostActivationTimer = 0.0f;
			}
		}

		if (gameInstance.gameState.ghostScatter)
		{
			gameInstance.gameState.ghostScatterTimer += elapsedTime;
			if (gameInstance.gameState.ghostScatterTimer >= gameInstance.gameState.scatterTimerThreshold)
			{
				gameInstance.entityManager.deactivateScatter();
				gameInstance.gameState.currentCycleGhostCount = 0;
			}
		}

		handleAllEntityMovement();
		handleAllEntityCollision();
		gameInstance.grid.draw();
		drawAllEntities();
		drawTopScreenText();
		break;

	case STATE_RESETTING:
		gameInstance.gameState.resetStateTimer += elapsedTime;
		if (gameInstance.gameState.resetStateTimer < gameInstance.gameState.resetTimerThreshold)
		{
			if (frameCount % 20 > 5)
			{
				gameInstance.grid.draw();
				drawAllEntities();
			}
		}
		else
		{
			if (gameInstance.gameState.shouldResetGrid)
			{
				gameInstance.grid.setup();
				gameInstance.gameState.globalDotCounter = 0;
				gameInstance.gameState.ghostScatterTimer = 0.0f;
				gameInstance.gameState.ghostScatter = false; 
				gameInstance.gameState.shouldResetGrid = false;
			}

			if (gameInstance.gameState.pacmanLivesRemaining > 0)
			{
				resetEntities();
				gameInstance.gameState.readyStateTimer = 0.0f;
				gameInstance.gameState.currentDotCounter = 0;
				gameInstance.entityManager.ghostActivationTimer = 0.0f;
				gameInstance.gameState.currentState = STATE_READY;
			}
			else
			{
				std::ofstream topScoreFileWrite("scores.txt", ios::app);
				if (topScoreFileWrite.is_open())
				{
					topScoreFileWrite << gameInstance.gameState.score << '\n';
				}
				topScoreFileWrite.close();
				gameInstance.gameState.currentState = STATE_END;
			}
		}
		drawTopScreenText();
		break;

	case STATE_END:
		Play::DrawFontText("72px", "Game Over", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 100 }, Play::CENTRE);
		Play::DrawFontText("64px", "Dots Eaten: " + std::to_string(gameInstance.gameState.statDotsEaten), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 220 }, Play::CENTRE);
		Play::DrawFontText("64px", "Fruits Eaten: " + std::to_string(gameInstance.gameState.statFruitsEaten), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 320 }, Play::CENTRE);
		Play::DrawFontText("64px", "Ghosts Eaten: " + std::to_string(gameInstance.gameState.statGhostsEaten), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 420 }, Play::CENTRE);
		Play::DrawFontText("72px", "Overall Score: " + std::to_string(gameInstance.gameState.score), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 600 }, Play::CENTRE);

		break;
	}

	//Play::DrawFontText("72px", std::to_string(blinky.canLeaveHouse), {DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80}, Play::CENTRE);

	Play::PresentDrawingBuffer();
	return Play::KeyDown( KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}
#pragma endregion

#pragma region Function Definitions
#pragma region Key Bindings
void bindKeys()
{
	inputManager.BindKey(Play::KEY_UP, []() { Pacman.requestChangeDirection(MOVE_UP); }, true);
	inputManager.BindKey(Play::KEY_LEFT, []() { Pacman.requestChangeDirection(MOVE_LEFT); }, true);
	inputManager.BindKey(Play::KEY_RIGHT, []() { Pacman.requestChangeDirection(MOVE_RIGHT); }, true);
	inputManager.BindKey(Play::KEY_DOWN, []() { Pacman.requestChangeDirection(MOVE_DOWN); }, true);
}

void startGame()
{
	gameInstance.gameState.currentState = STATE_READY;
	inputManager.UnbindKey(Play::KEY_ENTER);
}
#pragma endregion

#pragma region Entity Setup
void PlayerSetup()
{
	Pacman.setup(ENT_PACMAN, "pacman", 14, 7, MOVE_LEFT, MOVEMENT_TIME);
	bindKeys();
}

void GhostSetup()
{
	Blinky.setup(ENT_BLINKY, 12, 19, MOVE_UP, MOVEMENT_TIME, &Pacman, &Blinky);
	Blinky.IsInHouse = false;
	Pinky.setup(ENT_PINKY, 13, 16, MOVE_UP, MOVEMENT_TIME, &Pacman, &Blinky);
	Inky.setup(ENT_INKY, 14, 16, MOVE_UP, MOVEMENT_TIME, &Pacman, &Blinky);
	Clyde.setup(ENT_CLYDE, 15, 16, MOVE_UP, MOVEMENT_TIME, &Pacman, &Blinky);
}

void populateEntityManagerVectors()
{
	gameInstance.entityManager.entityVector.push_back(&Pacman);
	gameInstance.entityManager.entityVector.push_back(&Blinky);
	gameInstance.entityManager.entityVector.push_back(&Pinky);
	gameInstance.entityManager.entityVector.push_back(&Inky);
	gameInstance.entityManager.entityVector.push_back(&Clyde);
	gameInstance.entityManager.ghostVector.push_back(&Blinky);
	gameInstance.entityManager.ghostVector.push_back(&Pinky);
	gameInstance.entityManager.ghostVector.push_back(&Inky);
	gameInstance.entityManager.ghostVector.push_back(&Clyde);
}
#pragma endregion

#pragma region Entity Management
void handleAllEntityMovement()
{
	for (Entity* entity : gameInstance.entityManager.entityVector)
	{
		if (entity->id != ENT_NONE)
		{
			entity->movementTimer--;
			if (entity->movementTimer == 0)
			{
				entity->selectNewLocation();
				entity->movementTimer = entity->defaultMovementTimer;
			}

			if (entity->canMove)
			{
				entity->smoothMovement();
			}
		}
	}
}

void handleAllEntityCollision()
{
	for (Ghost* ghost : gameInstance.entityManager.ghostVector)
	{
		if (IsColliding(*Pacman.obj, *ghost->obj) && gameInstance.gameState.currentState == STATE_PLAY && ghost->type != ENT_NONE && !ghost->FleeingToHouse)
		{
			if (gameInstance.gameState.ghostScatter)
			{
				gameInstance.gameState.statGhostsEaten++;
				gameInstance.gameState.currentCycleGhostCount++;
				ghost->FleeingToHouse = true;
				ghost->queuedMovementTimer = 4;
				switch (gameInstance.gameState.currentCycleGhostCount)
				{
				case 1:
					gameInstance.gameState.score += 200;
					break;
				case 2:
					gameInstance.gameState.score += 400;
					break;
				case 3:
					gameInstance.gameState.score += 800;
					break;
				case 4:
					gameInstance.gameState.score += 1600;
					break;
				default:
					break;
				}
			}
			else
			{
				gameInstance.gameState.pacmanLivesRemaining--;
				gameInstance.gameState.resetStateTimer = 0.0f;
				gameInstance.gameState.currentState = STATE_RESETTING;
			}
		}
	}
}

void drawAllEntities()
{
	for (Entity* entity : gameInstance.entityManager.entityVector)
	{
		if (entity->type != ENT_NONE)
		{
			Play::UpdateGameObject(*entity->obj);
			Play::DrawObjectRotated(*entity->obj);
		}
	}
}

void resetEntities()
{
	Pacman.targetGridX = 14;
	Pacman.targetGridY = 7;
	Pacman.queuedDirection = MOVE_LEFT;
	Blinky.targetGridX = 12;
	Blinky.targetGridY = 19;
	Blinky.IsInHouse = false;
	Pinky.targetGridX = 13;
	Pinky.targetGridY = 16;
	Inky.targetGridX = 14;
	Inky.targetGridY = 16;
	Clyde.targetGridX = 15;
	Clyde.targetGridY = 16;
	for (Entity* entity : gameInstance.entityManager.entityVector)
	{
		entity->selectNewLocation();
	}
}
#pragma endregion

void drawTopScreenText()
{
	Play::DrawFontText("32px", "Score: " + std::to_string(gameInstance.gameState.score), {5 , DISPLAY_HEIGHT - 40}, Play::LEFT);
	Play::DrawFontText("32px", "Highscore : " + std::to_string(gameInstance.gameState.score), { DISPLAY_WIDTH - 7 , DISPLAY_HEIGHT - 40 }, Play::RIGHT);
	Play::DrawFontText("32px", "Lives Remaining: " + std::to_string(gameInstance.gameState.pacmanLivesRemaining), { 5 , DISPLAY_HEIGHT - 85 }, Play::LEFT);
}
#pragma endregion