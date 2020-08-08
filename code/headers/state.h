#pragma once

#include <map>

using namespace std;

// ========================================

class State
{
	private:

		int winW, winH;
		float elapsedTime;
		bool gameOver;
		map <int, bool> keys, lights;

	public:

		State(int winW, int winH, int numOfLights);

		// ****************************************
		
		int getWinW();
		void setWinW(int winW);
		
		int getWinH();
		void setWinH(int winH);
		
		float getElapsedTime();
		void setElapsedTime(float elapsedTime);
		
		bool isGameOver();
		void setGameOver(bool gameOver);

		map <int, bool> getKeys();
		void setKeys(int key, bool value);

		bool getLight(int id);
		void setLight(int id, bool value);

		// ****************************************

		bool isPressed(int key);
		bool isAnyKeyPressed();
};
