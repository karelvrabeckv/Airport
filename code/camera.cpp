#include <iostream>

#include "headers/camera.h"
#include "headers/data.h"
#include "headers/spline.h"
#include "headers/helpers.h"

extern bool planeModeOn;
extern bool freeCamOn;

// ========================================

Camera::Camera(vec3 pos, vec3 dir, vec3 up)
{
	this->plane = nullptr;

	this->pos = this->defPos = pos;
	this->dir = this->defDir = dir;
	this->up = this->defUp = up;

	this->vMatrix = lookAt(pos, pos + dir, up);

	this->speed = 0.0f;
	this->angleX = 0.0f;
	this->angleY = 0.0f;
	this->startTime = 0.0f;
} // CONSTRUCTOR

// ========================================

Object** Camera::getPlane()                                 {return this->plane;}
void     Camera::setPlane(Object** plane)                   {this->plane = plane;}

vec3     Camera::getPos()                                   {return this->pos;}
void     Camera::setPos(vec3 pos)                           {this->pos = pos;}

vec3     Camera::getDefPos()                                {return this->defPos;}
void     Camera::setDefPos(vec3 defPos)                     {this->defPos = defPos;}

vec3     Camera::getDir()                                   {return this->dir;}
void     Camera::setDir(vec3 dir)                           {this->dir = dir;}

vec3     Camera::getDefDir()                                {return this->defDir;}
void     Camera::setDefDir(vec3 defDir)                     {this->defDir = defDir;}

vec3     Camera::getUp()                                    {return this->up;}
void     Camera::setUp(vec3 up)                             {this->up = up;}

vec3     Camera::getDefUp()                                 {return this->defUp;}
void     Camera::setDefUp(vec3 defUp)                       {this->defUp = defUp;}

mat4     Camera::getVMatrix()                               {return this->vMatrix;}
void     Camera::setVMatrix(vec3 eye, vec3 center, vec3 up) {this->vMatrix = lookAt(eye, center, up);}

float    Camera::getSpeed()                                 {return this->speed;}
void     Camera::setSpeed(float speed)                      {this->speed = speed;}

float    Camera::getAngleX()                                {return this->angleX;}
void     Camera::setAngleX(float angleX)                    {this->angleX = angleX;}

float    Camera::getAngleY()                                {return this->angleY;}
void     Camera::setAngleY(float angleY)                    {this->angleY = angleY;}

float    Camera::getStartTime()                             {return this->startTime;}
void     Camera::setStartTime(float startTime)              {this->startTime = startTime;}

// ========================================

/** Controls the transfer from the ground onto the plane. */
bool Camera::interactWithPlane(Object** jetPlane, Object** fighterPlane, Object** retroPlane)
{
	// get off only when the plane is on the ground with no speed
	if (planeModeOn && ((*this->plane)->getCurrSpeed() == 0.0f) && ((*this->plane)->getPos().y == (*this->plane)->getDefPos().y))
	{
		this->setPos((*this->plane)->getPos() - 4.0f * normalize((*this->plane)->getDir()));
		this->plane = nullptr;

		return false;
	} // if

	// get on only when the player is near the plane
	if (*jetPlane && (*jetPlane)->isPlayerNearby(this->pos))
		this->plane = jetPlane;
	else if (*fighterPlane && (*fighterPlane)->isPlayerNearby(this->pos))
		this->plane = fighterPlane;
	else if (*retroPlane && (*retroPlane)->isPlayerNearby(this->pos))
		this->plane = retroPlane;

	// player is near the plane
	if (this->plane)
	{
		vec3 planePos = (*this->plane)->getPos();
		vec3 planeDir = normalize((*this->plane)->getDir());

		this->pos = planePos - 4.0f * planeDir + vec3(0.0f, 2.0f, 0.0f);
		this->angleX = degrees(acos(dot(this->defDir, planeDir) / (length(this->defDir) * length(planeDir))));
		this->angleY = 20.0f;

		return true;
	} // if

	return false;
} // INTERACT WITH PLANE

// ========================================

void Camera::updatePosWhileWalking(State* state)
{
	if (state->isGameOver()) return; // not working when game is over

	// ****************************************

	if (state->isPressed(KEY_UP)) // up
		this->setAngleY(this->getAngleY() - ANGLE_INC);

	if (state->isPressed(KEY_DOWN)) // down
		this->setAngleY(this->getAngleY() + ANGLE_INC);

	if (state->isPressed(KEY_LEFT)) // left
		this->setAngleX(this->getAngleX() - ANGLE_INC);

	if (state->isPressed(KEY_RIGHT)) // right
		this->setAngleX(this->getAngleX() + ANGLE_INC);

	// ****************************************

	vec3 moveDir = vec3(0.0f);
	this->speed = freeCamOn ? FREE_CAM_SPEED : REAL_CAM_SPEED;

	// set player movement
	if (freeCamOn) // free camera
	{
		moveDir += state->isPressed(KEY_W) ? this->dir : vec3(0.0f); // forward
		moveDir += state->isPressed(KEY_S) ? -this->dir : vec3(0.0f); // backward
		moveDir += state->isPressed(KEY_E) ? this->up : vec3(0.0f); // up
		moveDir += state->isPressed(KEY_Q) ? -this->up : vec3(0.0f); // down
	} // if
	else // real camera
	{
		vec3 realDir = this->dir;
		realDir.y = 0.0f; // unable to fly up or down according to the direction

		moveDir += state->isPressed(KEY_W) ? realDir : vec3(0.0f); // forward
		moveDir += state->isPressed(KEY_S) ? -realDir : vec3(0.0f); // backward
	} // else

	moveDir += state->isPressed(KEY_D) ? cross(this->dir, this->up) : vec3(0.0f); // to the right
	moveDir += state->isPressed(KEY_A) ? -cross(this->dir, this->up) : vec3(0.0f); // to the left

	vec3 oldPos = this->pos; // save old position
	this->pos += this->speed * moveDir; // update the camera position

	if (!freeCamOn) // real camera
		if (checkCollisions(this->pos, vec3(0.0f)) || checkBounds(nullptr))
			this->pos = oldPos;
} // UPDATE POS WHILE WALKING

// ========================================

void Camera::updatePosWhileFlying(State* state)
{
	if (state->isGameOver()) return; // not working when game is over

	// ****************************************

	if (state->isPressed(KEY_UP)) // up
		this->setAngleY(this->getAngleY() - ANGLE_INC);

	if (state->isPressed(KEY_DOWN)) // down
		this->setAngleY(this->getAngleY() + ANGLE_INC);

	if (state->isPressed(KEY_LEFT)) // left
		this->setAngleX(this->getAngleX() - ANGLE_INC);

	if (state->isPressed(KEY_RIGHT)) // right
		this->setAngleX(this->getAngleX() + ANGLE_INC);

	// ****************************************

	if (state->isPressed(KEY_W)) // speed up
		(*this->plane)->setCurrSpeed(std::min((*this->plane)->getCurrSpeed() + (*this->plane)->getAccel(), (*this->plane)->getMaxSpeed()));

	if (state->isPressed(KEY_S)) // slow down
		(*this->plane)->setCurrSpeed(std::max((*this->plane)->getCurrSpeed() - (*this->plane)->getAccel(), 0.0f));

	// ****************************************

	if (state->isPressed(KEY_A)) // to the left
	{
		vec3 plnAngle = (*this->plane)->getAngle();

		plnAngle.y += ANGLE_INC;
		if (plnAngle.y >= 360.0f)
			plnAngle.y = 0.0f;

		(*this->plane)->setDir(normalize(vec3(sin(radians(plnAngle.y)), 0.0f, cos(radians(plnAngle.y)))));
		(*this->plane)->setAngle(plnAngle);
	} // if

	if (state->isPressed(KEY_D)) // to the right
	{
		vec3 plnAngle = (*this->plane)->getAngle();

		plnAngle.y -= ANGLE_INC;
		if (plnAngle.y <= 0.0f)
			plnAngle.y = 360.0f;

		(*this->plane)->setDir(normalize(vec3(sin(radians(plnAngle.y)), 0.0f, cos(radians(plnAngle.y)))));
		(*this->plane)->setAngle(plnAngle);
	} // if

	// ****************************************

	if ((*this->plane)->getCurrSpeed() >= TAKE_OFF_SPEED) // enough speed to take off
	{
		if (state->isPressed(KEY_E)) // up
			(*this->plane)->setPos((*this->plane)->getPos() + vec3(0.0f, 0.05f, 0.0f));

		if (state->isPressed(KEY_Q)) // down
			(*this->plane)->setPos((*this->plane)->getPos() - vec3(0.0f, 0.05f, 0.0f));
	} // if
	else // low speed to land
		(*this->plane)->setPos((*this->plane)->getPos() - vec3(0.0f, 0.07f, 0.0f));

	// ****************************************

	vec3 oldPos = (*this->plane)->getPos(); // save old position

	(*this->plane)->setPos((*this->plane)->getPos() + (*this->plane)->getCurrSpeed() * normalize((*this->plane)->getDir())); // update plane position
	this->pos = (*this->plane)->getPos() - 4.0f * normalize((*this->plane)->getDir()) + vec3(0.0f, 2.0f, 0.0f); // update camera position
	
	if (checkCollisions((*this->plane)->getPos(), (*this->plane)->getBoundBox()))
	{
		(*this->plane)->setPos(oldPos);
		(*this->plane)->setCurrSpeed(0.0f);
	} // if

	checkBounds(this->plane);
} // UPDATE POS WHILE FLYING

// ========================================

/** Updates angle while moving with mouse. */
void Camera::updateAngle()
{
	// set horizontal and vertical rotation
	mat4 rotateX = rotate(mat4(1.0f), -radians(this->angleX), this->defUp);
	mat4 rotateY = rotate(mat4(1.0f), -radians(this->angleY), cross(this->defDir, this->defUp));

	// set vectors of a new view
	this->dir = normalize(vec3(rotateX * rotateY * vec4(this->defDir, 0.0f)));
	this->up = normalize(vec3(rotateX * rotateY * vec4(this->defUp, 0.0f)));

	this->setVMatrix(this->pos, this->pos + this->dir, this->up); // set the new view
} // UPDATE ANGLE

// ========================================

/** Updates moving camera on the curve. */
void Camera::update(State* state, const vec3* curveData, size_t curveSize)
{
	float time = 0.3f * (state->getElapsedTime() - this->startTime);

	this->setPos(evaluateClosedCurve(curveData, curveSize, time));
	this->setDir(normalize(evaluateClosedCurve_1stDerivative(curveData, curveSize, time)));
	this->setUp(Y_AXIS);

	this->setVMatrix(this->pos, this->pos + this->dir, this->up); // set the new view
} // UPDATE
