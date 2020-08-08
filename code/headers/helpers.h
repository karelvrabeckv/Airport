#pragma once

#include <iostream>

#include "headers/camera.h"
#include "headers/data.h"

using namespace pgr;
using namespace Assimp;

extern Camera* cam;
extern State* state;

extern vector <Object*> stones;
extern vector <Object*> hangars;
extern vector <Object*> explosions;

extern Object* tower;
extern Object* antenna;
extern Object* jetPlane;
extern Object* fighterPlane;
extern Object* retroPlane;
extern Object* helicopter;
extern Object* gameOver;

extern bool planeModeOn;
extern bool flashlightOn;

// ========================================

/** Deletes any component of the program (e.g. object). */
template <typename T>
void deleteComponent(T **comp)
{
	delete *comp;
	*comp = nullptr;
} // DELETE COMPONENT

// ========================================

/** Deletes any vector of the program and its content. */
template <typename T>
void deleteVector(vector <T*> &vect)
{
	for (auto it : vect)
		delete it;
	vect.clear();
} // DELETE VECTOR

// ========================================

void loadModel(const string &filename, GLuint program, vector <Mesh*> &model);
bool checkComplexCollision(vec3 myPos, vec3 myBoundBox, vec3 objPos, vec3 objBoundBox, vec3 objInBox);
bool checkTrivialCollision(vec3 myPos, vec3 myBoundBox, vec3 objPos, vec3 objBoundBox);
bool checkCollisions(vec3 myPos, vec3 myBoundBox);
bool checkBounds(Object **obj);
