#pragma once

#include "headers/object.h"

using namespace glm;

// ========================================

class Camera
{
	private:

		Object **plane;
		vec3 pos, defPos, dir, defDir, up, defUp;
		mat4 vMatrix;
		float speed, angleX, angleY, startTime;

	public:

		Camera(vec3 pos, vec3 dir, vec3 up);

		// ****************************************

		Object **getPlane();
		void setPlane(Object **plane);

		vec3 getPos();
		void setPos(vec3 pos);

		vec3 getDefPos();
		void setDefPos(vec3 defPos);

		vec3 getDir();
		void setDir(vec3 dir);

		vec3 getDefDir();
		void setDefDir(vec3 defDir);

		vec3 getUp();
		void setUp(vec3 up);

		vec3 getDefUp();
		void setDefUp(vec3 defUp);

		mat4 getVMatrix();
		void setVMatrix(vec3 eye, vec3 center, vec3 up);

		float getSpeed();
		void setSpeed(float speed);

		float getAngleX();
		void setAngleX(float angleX);

		float getAngleY();
		void setAngleY(float angleY);

		float getStartTime();
		void setStartTime(float startTime);

		// ****************************************

		bool interactWithPlane(Object **jetPlane, Object **fighterPlane, Object **retroPlane);
		void updatePosWhileWalking(State *state);
		void updatePosWhileFlying(State *state);
		void updateAngle();
		void update(State *state, const vec3 *curveData, size_t curveSize);
};
