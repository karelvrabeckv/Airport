#include "pgr.h"
#include "headers/state.h"
#include "headers/data.h"

// ========================================

State::State(int winW, int winH, int numOfLights)
{
	this->winW = winW;
	this->winH = winH;
	this->elapsedTime = 0.0f;
	this->gameOver = false;

	for (int i = 0; i < KEYS_COUNT; i++)
		(this->keys).insert({i, false}); // keys are released by default

	for (int i = 0; i < numOfLights; i++)
		(this->lights).insert({i, true}); // lights are switched on by default
} // CONSTRUCTOR

// ========================================

int             State::getWinW()                         {return this->winW;}
void            State::setWinW(int winW)                 {this->winW = winW;}

int             State::getWinH()                         {return this->winH;}
void            State::setWinH(int winH)                 {this->winH = winH;}

float           State::getElapsedTime()                  {return this->elapsedTime;}
void            State::setElapsedTime(float elapsedTime) {this->elapsedTime = elapsedTime;}

bool            State::isGameOver()                      {return this->gameOver;}
void            State::setGameOver(bool gameOver)        {this->gameOver = gameOver;}

map <int, bool> State::getKeys()                         {return this->keys;}
void            State::setKeys(int key, bool value)      {this->keys[key] = value;}

bool            State::getLight(int id)                  {return this->lights[id];}
void            State::setLight(int id, bool value)      {this->lights[id] = value;}

// ========================================

/** Checks whether the key is pressed or not. */
bool State::isPressed(int key)
{
	return this->keys[key];
} // IS PRESSED

// ========================================

/** Checks whether at least one key is pressed. */
bool State::isAnyKeyPressed()
{
	return any_of(begin(this->keys), end(this->keys), [](pair <int, bool> x) {return x.second;});
} // IS ANY KEY PRESSED
