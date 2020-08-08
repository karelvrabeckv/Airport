#pragma once

#include "pgr.h"
#include "headers/state.h"
#include "headers/mesh.h"

// ========================================

class Object
{
	protected:

		vec3 defPos, pos, defDir, dir, size, boundBox, defAngle, angle;
		float currSpeed, maxSpeed, accel, startTime, currTime;

	public:

		Object
		(
			vec3 pos, vec3 dir, vec3 size, vec3 boundBox, vec3 angle,
			float currSpeed = 0.0f, float maxSpeed = 0.0f, float accel = 0.0f, float startTime = 0.0f
		)
		: defPos(pos), pos(pos), defDir(dir), dir(dir), size(size), boundBox(boundBox), defAngle(angle), angle(angle),
			currSpeed(currSpeed), maxSpeed(maxSpeed), accel(accel), startTime(startTime), currTime(startTime) {};

		// ****************************************

		vec3 getDefPos();
		void setDefPos(vec3 defPos);

		vec3 getPos();
		void setPos(vec3 pos);

		vec3 getDir();
		void setDir(vec3 dir);

		vec3 getSize();
		void setSize(vec3 size);

		vec3 getBoundBox();
		void setBoundBox(vec3 boundBox);

		vec3 getAngle();
		void setAngle(vec3 angle);

		float getCurrSpeed();
		void setCurrSpeed(float currSpeed);

		float getMaxSpeed();
		void setMaxSpeed(float maxSpeed);

		float getAccel();
		void setAccel(float accel);

		float getStartTime();
		void setStartTime(float startTime);

		float getCurrTime();
		void setCurrTime(float currTime);

		// ****************************************

		bool isPlayerNearby(vec3 myPos);
		void update(State* state, const vec3* curveData, size_t curveSize);
		virtual void draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix);
};

// ========================================

class Helicopter : public Object
{
	public:

		Helicopter
		(
			vec3 pos, vec3 dir, vec3 size, vec3 boundBox, vec3 angle,
			float speed = 0.0f, float startTime = 0.0f
		) : Object(pos, dir, size, boundBox, angle, speed, speed, speed, startTime) {};

		// ****************************************

		void draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix);
};

// ========================================

class Explosion : public Object
{
	public:

		Explosion
		(
			vec3 pos, vec3 dir, vec3 size, vec3 boundBox, vec3 angle,
			float speed = 0.0f, float startTime = 0.0f
		) : Object(pos, dir, size, boundBox, angle, speed, speed, speed, startTime) {};

		// ****************************************

		void draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix);
};

// ========================================

class GameOver : public Object
{
	public:

		GameOver
		(
			vec3 pos, vec3 dir, vec3 size, vec3 boundBox, vec3 angle,
			float speed = 0.0f, float startTime = 0.0f
		) : Object(pos, dir, size, boundBox, angle, speed, speed, speed, startTime) {};

		// ****************************************

		void draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix);
};

// ========================================

class Skybox : public Object
{
	public:

		Skybox
		(
			vec3 pos = vec3(0.0f), vec3 dir = vec3(0.0f), vec3 size = vec3(0.0f), vec3 boundBox = vec3(0.0f), vec3 angle = vec3(0.0f)
		) : Object(pos, dir, size, boundBox, angle) {};

		// ****************************************

		void draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix);
};
