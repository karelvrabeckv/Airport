// ========================================
// SETUP
// ========================================

#include <iostream>

#include "pgr.h"
#include "headers/camera.h"
#include "headers/data.h"
#include "headers/helpers.h"
#include "headers/light.h"
#include "headers/mesh.h"
#include "headers/object.h"
#include "headers/spline.h"
#include "headers/state.h"

using namespace std;
using namespace glm;
using namespace pgr;

// ========================================
// GLOBAL VARIABLES
// ========================================

// programs
GLuint mainProg      = 0;
GLuint skyboxProg    = 0;
GLuint explosionProg = 0;
GLuint gameOverProg  = 0;

// components
Camera *cam    = nullptr;
State  *state  = nullptr;
Skybox *skybox = nullptr;

// objects
vector <Object*> spotLights;
vector <Object*> hangars;
vector <Object*> lamps;
vector <Object*> stones;
vector <Object*> explosions;

Object* island       = nullptr;
Object* runway       = nullptr;
Object* tower        = nullptr;
Object* antenna      = nullptr;
Object* jetPlane     = nullptr;
Object* fighterPlane = nullptr;
Object* retroPlane   = nullptr;
Object* helicopter   = nullptr;
Object* gameOver     = nullptr;

// lights
vector <Light*> lights;

// models
vector <Mesh*> spotLightModel;
vector <Mesh*> hangarModel;
vector <Mesh*> lampModel;
vector <Mesh*> stoneModel;
vector <Mesh*> explosionModel;
vector <Mesh*> islandModel;
vector <Mesh*> runwayModel;
vector <Mesh*> towerModel;
vector <Mesh*> antennaModel;
vector <Mesh*> jetPlaneModel;
vector <Mesh*> fighterPlaneModel;
vector <Mesh*> retroPlaneModel;
vector <Mesh*> helicopterModel;
vector <Mesh*> gameOverModel;
vector <Mesh*> skyboxDayModel;
vector <Mesh*> skyboxNightModel;

// matrices
mat4 pMat = mat4(1.0f);
mat4 vMat = mat4(1.0f);
mat4 mMat = mat4(1.0f);

// parameters
bool dayOn           = true;
bool flashlightOn    = false;
bool mistOn          = false;
bool planeModeOn     = false;
bool freeCamOn       = false;
bool realCamOn       = true;
bool towerCamOn      = false;
bool runwayCamOn     = false;
bool helicopterCamOn = false;
bool airportExhCamOn = false;

// last free camera position
vec3 lastFreeCamPos     = CAM_DEF_POS;
float lastFreeCamAngleX = 0.0f;
float lastFreeCamAngleY = 0.0f;

// last real camera position
vec3 lastRealCamPos     = CAM_DEF_POS;
float lastRealCamAngleX = 0.0f;
float lastRealCamAngleY = 0.0f;

// ========================================
// INITIALIZATION
// ========================================

void createPrograms()
{
	mainProg = createProgram({createShaderFromFile(GL_VERTEX_SHADER, VS_MAIN_SRC), createShaderFromFile(GL_FRAGMENT_SHADER, FS_MAIN_SRC)});
	skyboxProg = createProgram({createShaderFromFile(GL_VERTEX_SHADER, VS_SKYBOX_SRC), createShaderFromFile(GL_FRAGMENT_SHADER, FS_SKYBOX_SRC)});
	explosionProg = createProgram({createShaderFromFile(GL_VERTEX_SHADER, VS_EXPLOSION_SRC), createShaderFromFile(GL_FRAGMENT_SHADER, FS_EXPLOSION_SRC)});
	gameOverProg = createProgram({createShaderFromFile(GL_VERTEX_SHADER, VS_GAME_OVER_SRC), createShaderFromFile(GL_FRAGMENT_SHADER, FS_GAME_OVER_SRC)});
} // CREATE PROGRAMS

// ========================================

void createModels()
{
	loadModel(ISLAND_MODEL_SRC, mainProg, islandModel);
	loadModel(RUNWAY_MODEL_SRC, mainProg, runwayModel);
	loadModel(HANGAR_MODEL_SRC, mainProg, hangarModel);
	loadModel(TOWER_MODEL_SRC, mainProg, towerModel);
	loadModel(ANTENNA_MODEL_SRC, mainProg, antennaModel);
	loadModel(JET_MODEL_SRC, mainProg, jetPlaneModel);
	loadModel(FIGHTER_MODEL_SRC, mainProg, fighterPlaneModel);
	loadModel(RETRO_MODEL_SRC, mainProg, retroPlaneModel);
	loadModel(HELICOPTER_MODEL_SRC, mainProg, helicopterModel);
	loadModel(STONE_MODEL_SRC, mainProg, stoneModel);
	loadModel(LAMP_MODEL_SRC, mainProg, lampModel);

	// set explosion model
	Mesh* explosionMesh = new Mesh();
	explosionMesh->createExplosionMesh(explosionProg);
	explosionModel.push_back(explosionMesh);

	// set game over model
	Mesh* gameOverMesh = new Mesh();
	gameOverMesh->createGameOverMesh(gameOverProg);
	gameOverModel.push_back(gameOverMesh);

	// set spot light model
	Mesh* spotLightMesh = new Mesh();
	spotLightMesh->createSpotLightMesh(mainProg);
	spotLightModel.push_back(spotLightMesh);

	// set skybox model for day
	Mesh* skyboxDayMesh = new Mesh();
	skyboxDayMesh->createSkyboxMesh
	(
		skyboxProg,
		SKYBOX_DAY_POS_X_SRC, SKYBOX_DAY_NEG_X_SRC,
		SKYBOX_DAY_POS_Y_SRC, SKYBOX_DAY_NEG_Y_SRC,
		SKYBOX_DAY_POS_Z_SRC, SKYBOX_DAY_NEG_Z_SRC
	);
	skyboxDayModel.push_back(skyboxDayMesh);

	// set skybox model for night
	Mesh* skyboxNightMesh = new Mesh();
	skyboxNightMesh->createSkyboxMesh
	(
		skyboxProg,
		SKYBOX_NIGHT_POS_X_SRC, SKYBOX_NIGHT_NEG_X_SRC,
		SKYBOX_NIGHT_POS_Y_SRC, SKYBOX_NIGHT_NEG_Y_SRC,
		SKYBOX_NIGHT_POS_Z_SRC, SKYBOX_NIGHT_NEG_Z_SRC
	);
	skyboxNightModel.push_back(skyboxNightMesh);
} // CREATE MODELS

// ========================================

void createObjects()
{
	lights.push_back(new Light(vec3(-0.5f, 1.0f, -1.0f), vec3(0.0f), vec3(0.0f), vec3(1.0f, 1.0f, 0.5f), vec3(1.0f), 0.0f, 0.0f)); // sun
	lights.push_back(new Light(CAM_DEF_POS, -Z_AXIS, vec3(0.2f), vec3(1.0f), vec3(1.0f), 0.92f, 15.0f)); // flashlight

	// lamp lights
	lights.push_back(new Light(vec3(-7.2f, 4.2f, 3.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f), vec3(1.0f, 1.0f, 0.5f), vec3(1.0f), 0.2f, 2.5f));
	lights.push_back(new Light(vec3(-7.2f, 4.2f, 13.25f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f), vec3(1.0f, 1.0f, 0.5f), vec3(1.0f), 0.2f, 2.5f));
	lights.push_back(new Light(vec3(-7.2f, 4.2f, 23.5f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f), vec3(1.0f, 1.0f, 0.5f), vec3(1.0f), 0.2f, 2.5f));

	// tower, heliport and antenna lights
	lights.push_back(new Light(vec3(-6.0f, 7.4f, 32.6f), vec3(0.0f), vec3(0.7f, 0.7f, 0.2f), vec3(0.7f, 0.7f, 0.2f), vec3(1.0f), 0.0f, 0.0f));
	lights.push_back(new Light(vec3(-9.75f, 0.1f, -10.25f), vec3(0.0f), vec3(0.7f, 0.0f, 0.0f), vec3(0.7f, 0.0f, 0.0f), vec3(1.0f), 0.0f, 0.0f));
	lights.push_back(new Light(vec3(-6.1f, 7.89f, -33.51f), vec3(0.0f), vec3(0.7f, 0.7f, 0.2f), vec3(0.7f, 0.7f, 0.2f), vec3(1.0f), 0.0f, 0.0f));

	for (int i = 0; i < 5; i++) // left runway lights
		lights.push_back(new Light(vec3(7.6f, 0.1f, 39.55f - i * 19.76f), vec3(0.0f), vec3(0.7f, 0.0f, 0.0f), vec3(0.7f, 0.0f, 0.0f), vec3(1.0f), 0.0f, 0.0f));

	for (int i = 0; i < 5; i++) // right runway lights
		lights.push_back(new Light(vec3(14.25f, 0.1f, 39.55f - i * 19.76f), vec3(0.0f), vec3(0.7f, 0.0f, 0.0f), vec3(0.7f, 0.0f, 0.0f), vec3(1.0f), 0.0f, 0.0f));

	// ****************************************

	cam = new Camera(CAM_DEF_POS, CAM_DEF_DIR, CAM_DEF_UP);
	state = new State(WIN_WIDTH, WIN_HEIGHT, lights.size());
	skybox = new Skybox();

	// ****************************************

	island = new Object(vec3(0.0f, -6.0f, 0.0f), vec3(0.0f), vec3(75.5f, 67.5f, 75.5f), vec3(0.0f), vec3(0.0f));
	runway = new Object(vec3(-3.0f, 0.0f, 0.0f), vec3(0.0f), vec3(41.0f), vec3(0.0f), vec3(0.0f));
	tower = new Object(vec3(-6.0f, 4.0f, 32.5f), vec3(0.0f), vec3(4.0f), vec3(1.75f, 4.75f, 1.75f), vec3(0.0f));
	antenna = new Object(vec3(-6.0f, 3.95f, -33.5f), vec3(0.0f), vec3(4.0f), vec3(0.5f, 4.25f, 0.5f), vec3(0.0f));

	hangars.push_back(new Object(vec3(-14.0f, 2.2f, 18.5f), vec3(0.0f), vec3(5.0f), vec3(5.25f, 5.0f, 5.0f), vec3(0.0f, 90.0f, 0.0f)));
	hangars.push_back(new Object(vec3(-14.0f, 2.2f, 8.0f), vec3(0.0f), vec3(5.0f), vec3(5.25f, 5.0f, 5.0f), vec3(0.0f, 90.0f, 0.0f)));

	helicopter = new Helicopter(vec3(0.0f), vec3(0.0f), vec3(2.5f), vec3(2.5f, 1.25f, 2.5f), vec3(0.0f), 0.3f, 0.001f * (float)glutGet(GLUT_ELAPSED_TIME));

	jetPlane = new Object(vec3(-5.25f, 0.6f, 18.9f), XZ_AXIS, vec3(2.5f), vec3(2.75f, 0.75f, 2.75f), vec3(0.0f, 45.0f, 0.0f), 0.0f, 0.5f, 0.001f);
	fighterPlane = new Object(vec3(-5.25f, 0.81f, 12.9f), XZ_AXIS, vec3(2.5f), vec3(2.5f, 0.75f, 2.5f), vec3(0.0f, 45.0f, 0.0f), 0.0f, 0.7f, 0.002f);
	retroPlane = new Object(vec3(-5.25f, 0.5f, 6.9f), XZ_AXIS, vec3(1.5f), vec3(2.0f, 0.75f, 2.0f), vec3(3.0f, 45.0f, 0.0f), 0.0f, 0.3f, 0.0015f);

	stones.push_back(new Object(vec3(-0.5f, 0.0f, 45.0f), vec3(0.0f), vec3(6.0f), vec3(5.5f, 3.25f, 6.25f), vec3(0.0f, 225.0f, 0.0f)));
	stones.push_back(new Object(vec3(-17.0f, -2.5f, -29.5f), vec3(0.0f), vec3(9.0f), vec3(7.0f, 5.25f, 9.25f), vec3(0.0f, 10.0f, 0.0f)));

	lamps.push_back(new Object(vec3(-8.0f, 2.0f, 3.0f), vec3(0.0f), vec3(2.5f), vec3(0.0f), vec3(0.0f)));
	lamps.push_back(new Object(vec3(-8.0f, 2.0f, 13.25f), vec3(0.0f), vec3(2.5f), vec3(0.0f), vec3(0.0f)));
	lamps.push_back(new Object(vec3(-8.0f, 2.0f, 23.5f), vec3(0.0f), vec3(2.5f), vec3(0.0f), vec3(0.0f)));

	spotLights.push_back(new Object(vec3(-6.0f, 7.4f, 32.6f), vec3(0.0f), vec3(0.05f), vec3(0.0f), vec3(0.0f))); // tower
	spotLights.push_back(new Object(vec3(-9.75f, 0.0f, -10.25f), vec3(0.0f), vec3(0.05f), vec3(0.0f), vec3(0.0f))); // heliport
	spotLights.push_back(new Object(vec3(-6.1f, 7.89f, -33.51f), vec3(0.0f), vec3(0.05f), vec3(0.0f), vec3(0.0f))); // antenna

	for (int i = 0; i < 5; i++) // left runway lights
		spotLights.push_back(new Object(vec3(7.6f, 0.0f, 39.55f - i * 19.76f), vec3(0.0f), vec3(0.05f), vec3(0.0f), vec3(0.0f)));

	for (int i = 0; i < 5; i++) // right runway lights
		spotLights.push_back(new Object(vec3(14.25f, 0.0f, 39.55f - i * 19.76f), vec3(0.0f), vec3(0.05f), vec3(0.0f), vec3(0.0f)));
} // CREATE OBJECTS

// ========================================

void init()
{
	glClearColor(MIST_COL, MIST_COL, MIST_COL, 1.0f); // set default ambient color
	glClearStencil(100); // set initial value for stencil buffer
	glEnable(GL_DEPTH_TEST); // enable depth buffer
	glDepthMask(GL_TRUE); // enable modifying depth buffer

	createPrograms();
	createModels();
	createObjects();
} // INIT

// ========================================
// DISPLAY MANAGEMENT
// ========================================

void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear buffers
	vMat = cam->getVMatrix(); // set view matrix

	// position and direction of flashlight
	if (planeModeOn) // player is flying
	{
		lights[1]->setPos((*cam->getPlane())->getPos() + 2.0f * normalize((*cam->getPlane())->getDir()));
		lights[1]->setDir(normalize((*cam->getPlane())->getDir()));
	} // if
	else // player is walking
	{
		lights[1]->setPos(cam->getPos());
		lights[1]->setDir(cam->getDir());
	} // else

	// ****************************************
	
	glUseProgram(mainProg);

	// send lights to vertex shader
	for (int i = 0; i < (int)lights.size(); i++)
	{
		string index = to_string(i);

		if (!state->getLight(i))
		{
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].amb").c_str()), 1, value_ptr(vec3(0.0f)));
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].dif").c_str()), 1, value_ptr(vec3(0.0f)));
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].spe").c_str()), 1, value_ptr(vec3(0.0f)));
		} // if
		else
		{
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].pos").c_str()), 1, value_ptr(lights[i]->getPos()));
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].dir").c_str()), 1, value_ptr(lights[i]->getDir()));
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].amb").c_str()), 1, value_ptr(lights[i]->getAmb()));
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].dif").c_str()), 1, value_ptr(lights[i]->getDif()));
			glUniform3fv(glGetUniformLocation(mainProg, ("lights[" + index + "].spe").c_str()), 1, value_ptr(lights[i]->getSpe()));
			glUniform1f(glGetUniformLocation(mainProg, ("lights[" + index + "].cosCutOff").c_str()), lights[i]->getCosCutOff());
			glUniform1f(glGetUniformLocation(mainProg, ("lights[" + index + "].expo").c_str()), lights[i]->getExpo());
		} // else
	} // for

	// send other data to vertex shader
	glUniform1i(glGetUniformLocation(mainProg, DAY_ON_VAR), dayOn);
	glUniform1i(glGetUniformLocation(mainProg, FLA_ON_VAR), flashlightOn);
	glUniform1i(glGetUniformLocation(mainProg, MIST_ON_VAR), mistOn);
	glUniform1f(glGetUniformLocation(mainProg, MIST_DEN_VAR), MIST_DEN);
	glUniform1f(glGetUniformLocation(mainProg, MIST_COL_VAR), MIST_COL);

	glUseProgram(0);

	// ****************************************
	
	if (dayOn && !mistOn) // draw skybox for day
		skybox->draw(skyboxProg, skyboxDayModel, pMat, vMat, mMat);
	else if (!dayOn && !mistOn) // draw skybox for night
		skybox->draw(skyboxProg, skyboxNightModel, pMat, vMat, mMat);

	island->draw(mainProg, islandModel, pMat, vMat, mMat);
	runway->draw(mainProg, runwayModel, pMat, vMat, mMat);
	tower->draw(mainProg, towerModel, pMat, vMat, mMat);
	antenna->draw(mainProg, antennaModel, pMat, vMat, mMat);

	for (auto it : hangars)
		it->draw(mainProg, hangarModel, pMat, vMat, mMat);

	for (auto it : stones)
		it->draw(mainProg, stoneModel, pMat, vMat, mMat);

	for (auto it : explosions)
		it->draw(explosionProg, explosionModel, pMat, vMat, mMat);

	if (gameOver)
		gameOver->draw(gameOverProg, gameOverModel, pMat, vMat, mMat);
	
	// ****************************************
	
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP /* stencil test failed */, GL_KEEP /* stancil test passed, depth test failed */, GL_REPLACE /* both tests passed */);

	if (helicopter) // draw helicopter if exists
	{
		glStencilFunc(GL_ALWAYS /* stencil test always passes */, 40 /* value in stencil buffer */, 255 /* mask */);
		helicopter->draw(mainProg, helicopterModel, pMat, vMat, mMat);
	} // if

	if (jetPlane) // draw jet plane if exists
	{
		glStencilFunc(GL_ALWAYS, 41, 255);
		jetPlane->draw(mainProg, jetPlaneModel, pMat, vMat, mMat);
	} // if

	if (fighterPlane) // draw fighter plane if exists
	{
		glStencilFunc(GL_ALWAYS, 42, 255);
		fighterPlane->draw(mainProg, fighterPlaneModel, pMat, vMat, mMat);
	} // if

	if (retroPlane) // draw old plane if exists
	{
		glStencilFunc(GL_ALWAYS, 43, 255);
		retroPlane->draw(mainProg, retroPlaneModel, pMat, vMat, mMat);
	} // if

	int id = 2;
	for (auto it : lamps) // draw all lamps
	{
		glStencilFunc(GL_ALWAYS, id++, 255);
		it->draw(mainProg, lampModel, pMat, vMat, mMat);
	} // for

	for (auto it : spotLights) // draw all spot lights
	{
		glStencilFunc(GL_ALWAYS, id++, 255);
		it->draw(mainProg, spotLightModel, pMat, vMat, mMat);
	} // for

	glDisable(GL_STENCIL_TEST);

	// ****************************************

	glutSwapBuffers(); // process next data
} // ON DISPLAY

// ========================================
// CLOSE MANAGEMENT
// ========================================

void onClose()
{
	deleteComponent(&cam);
	deleteComponent(&state);
	deleteComponent(&skybox);

	deleteComponent(&island);
	deleteComponent(&runway);
	deleteComponent(&tower);
	deleteComponent(&antenna);
	deleteComponent(&jetPlane);
	deleteComponent(&fighterPlane);
	deleteComponent(&retroPlane);
	deleteComponent(&helicopter);
	deleteComponent(&gameOver);

	deleteVector(hangars);
	deleteVector(lamps);
	deleteVector(spotLights);
	deleteVector(stones);
	deleteVector(explosions);
	deleteVector(lights);
} // ON CLOSE

// ========================================
// RESHAPE MANAGEMENT
// ========================================

void onReshape(int newW, int newH)
{
	pMat = perspective(radians(60.0f) /* angle */, (float)state->getWinW() / state->getWinH() /* aspect */, 0.1f /* near */, 150.0f /* far */);

	state->setWinW(newW);
	state->setWinH(newH);

	glViewport(0 /* x */, 0 /* y */, (GLsizei)newW /* width */, (GLsizei)newH /* height */);
} // ON RESHAPE

// ========================================
// MOUSE MANAGEMENT
// ========================================

void onMouse(int mouseButton, int mouseState, int mouseX, int mouseY)
{
	if (planeModeOn) return; // not working when flying

	// ****************************************

	if ((mouseButton == GLUT_LEFT_BUTTON) && (mouseState == GLUT_DOWN))
	{
		unsigned char clickedID = 0; // stencil buffer value

		// read value from stencil buffer
		glReadPixels(mouseX /* x */, state->getWinH() - mouseY /* y */, 1 /* width */, 1 /* height */, GL_STENCIL_INDEX /* format */, GL_UNSIGNED_BYTE /* type */, &clickedID /* data */);

		float time = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);
		if ((clickedID >= 2) && (clickedID <= 17)) // lights
			state->setLight(clickedID, !state->getLight(clickedID));
		else if (clickedID == 40) // helicopter
		{
			explosions.push_back(new Explosion(helicopter->getPos(), Z_AXIS, vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
			deleteComponent(&helicopter);
		} // else if
		else if (clickedID == 41) // jet plane
		{
			explosions.push_back(new Explosion(jetPlane->getPos(), Z_AXIS, vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
			deleteComponent(&jetPlane);
		} // else if
		else if (clickedID == 42) // fighter plane
		{
			explosions.push_back(new Explosion(fighterPlane->getPos(), Z_AXIS, vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
			deleteComponent(&fighterPlane);
		} // else if
		else if (clickedID == 43) // old plane
		{
			explosions.push_back(new Explosion(retroPlane->getPos(), Z_AXIS, vec3(2.0f), vec3(0.0f), vec3(0.0f), 0.0f, time));
			deleteComponent(&retroPlane);
		} // else if
	} // if
} // ON MOUSE

// ========================================
// MOTION MANAGEMENT
// ========================================

void onMotion(int mouseX, int mouseY)
{
	if (state->isGameOver()) return; // not working when game is over

	// ****************************************

	if (mouseY != state->getWinH() / 2) // move with camera vertically
	{
		float angleY = cam->getAngleY() + CAMERA_SENSITIVITY * (mouseY - state->getWinH() / 2);
		if (abs(angleY) < CAMERA_ANGLE_MAX) // check top and bottom limit
			cam->setAngleY(angleY);
	} // if

	if (mouseX != state->getWinW() / 2) // move with camera horizontally
	{
		float angleX = cam->getAngleX() + CAMERA_SENSITIVITY * (mouseX - state->getWinW() / 2);
		cam->setAngleX(angleX);
	} // if

	glutWarpPointer(state->getWinW() / 2, state->getWinH() / 2); // bind pointer with screen center
	glutPostRedisplay(); // redraw the window
} // ON MOTION

// ========================================
// TIMER MANAGEMENT
// ========================================

void onTimer(int value)
{
	state->setElapsedTime(0.001f * (float)glutGet(GLUT_ELAPSED_TIME));

	// ****************************************

	if (helicopter) // update helicopter if exists
	{
		helicopter->setCurrTime(state->getElapsedTime());
		helicopter->update(state, helicopterCurveData, helicopterCurveSize);
	} // if

	if (gameOver) // update game over if exists
		gameOver->setCurrTime(state->getElapsedTime());

	// update explosion
	auto it = explosions.begin();
	while (it != explosions.end())
	{
		(*it)->setCurrTime(state->getElapsedTime());
		if ((*it)->getCurrTime() >= (*it)->getStartTime() + 16.0f /* frames */ * 0.1f /* duration */)
			it = explosions.erase(it);
		else
			it++;
	} // while

	// ****************************************

	if (towerCamOn) // tower static camera
	{
		cam->setPos(TOWER_CAM_POS);
		cam->setDir(TOWER_CAM_DIR);
		cam->setUp(TOWER_CAM_UP);
		cam->setVMatrix(cam->getPos(), cam->getPos() + cam->getDir(), cam->getUp());
	} // if
	else if (runwayCamOn) // runway static camera
	{
		cam->setPos(RUNWAY_CAM_POS);
		cam->setDir(RUNWAY_CAM_DIR);
		cam->setUp(RUNWAY_CAM_UP);
		cam->setVMatrix(cam->getPos(), cam->getPos() + cam->getDir(), cam->getUp());
	} // else if
	else if (helicopterCamOn) // helicopter dynamic camera
	{
		cam->setPos(helicopter->getPos() - 4.0f * normalize(helicopter->getDir()) + vec3(0.0f, 0.5f, 0.0f));
		cam->setDir(helicopter->getDir());
		cam->setUp(Y_AXIS);
		cam->setVMatrix(cam->getPos(), cam->getPos() + cam->getDir(), cam->getUp());
	} // else if
	else if (airportExhCamOn) // airport exhibition dynamic camera
		cam->update(state, airportExhCurveData, airportExhCurveSize);
	else // free or real camera
	{
		cam->updateAngle();

		if (planeModeOn) // player is flying
			cam->updatePosWhileFlying(state);
		else // player is walking
			cam->updatePosWhileWalking(state);
	} // else

	// ****************************************

	glutTimerFunc(REFRESH_INTERVAL, onTimer, 0); // register new callback
	glutPostRedisplay(); // redraw the window
} // ON TIMER

// ========================================
// KEYBOARD MANAGEMENT
// ========================================

void onKeyPressed(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27: // escape
			glutLeaveMainLoop(); // finish processing events
			break;
		case 'r': case 'R': // restart
			onClose();
			createObjects();

			// last positions of cameras
			lastFreeCamPos = lastRealCamPos = CAM_DEF_POS;
			lastFreeCamAngleX = lastFreeCamAngleY = lastRealCamAngleX = lastRealCamAngleY = 0.0f;

			// booleans
			realCamOn = dayOn = true;
			flashlightOn = mistOn = planeModeOn = freeCamOn = towerCamOn = runwayCamOn = helicopterCamOn = airportExhCamOn = false;
			break;
		default:
			break;
	} // switch
	if (state->isGameOver() || (!realCamOn && !freeCamOn)) return; // do not check following keys when game is over or other camera modes are on

	// ****************************************

	switch (key)
	{
		case 13: // enter
			planeModeOn = cam->interactWithPlane(&jetPlane, &fighterPlane, &retroPlane);
			break;
		case 'w': case 'W': // forward
			state->setKeys(KEY_W, true);
			break;
		case 's': case 'S': // backward
			state->setKeys(KEY_S, true);
			break;
		case 'a': case 'A': // to the left
			state->setKeys(KEY_A, true);
			break;
		case 'd': case 'D': // to the right
			state->setKeys(KEY_D, true);
			break;
		case 'e': case 'E': // up
			state->setKeys(KEY_E, true);
			break;
		case 'q': case 'Q': // down
			state->setKeys(KEY_Q, true);
			break;
		case 'f': case 'F': // flashlight
			flashlightOn = !flashlightOn;
			break;
		case 'l': case 'L': // day/night
			dayOn = !dayOn;
			break;
		case 'm': case 'M': // mist
			mistOn = !mistOn;
			break;
		default:
			break;
	} // switch
} // ON KEY PRESSED

// ========================================

void onKeyReleased(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w': case 'W':
			state->setKeys(KEY_W, false);
			break;
		case 's': case 'S':
			state->setKeys(KEY_S, false);
			break;
		case 'a': case 'A':
			state->setKeys(KEY_A, false);
			break;
		case 'd': case 'D':
			state->setKeys(KEY_D, false);
			break;
		case 'e': case 'E':
			state->setKeys(KEY_E, false);
			break;
		case 'q': case 'Q':
			state->setKeys(KEY_Q, false);
			break;
		default:
			break;
	} // switch
} // ON KEY RELEASED

// ========================================

void saveOldCamPos()
{
	if (!freeCamOn && !towerCamOn && !runwayCamOn && !helicopterCamOn && !airportExhCamOn) // switch from real camera to another
	{
		lastRealCamPos = cam->getPos();
		lastRealCamAngleX = cam->getAngleX();
		lastRealCamAngleY = cam->getAngleY();
	} // if
	else if (!realCamOn && !towerCamOn && !runwayCamOn && !helicopterCamOn && !airportExhCamOn) // switch from free camera to another
	{
		lastFreeCamPos = cam->getPos();
		lastFreeCamAngleX = cam->getAngleX();
		lastFreeCamAngleY = cam->getAngleY();
	} // else if
} // SAVE OLD CAM POS

// ========================================

void onSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
			state->setKeys(KEY_UP, true);
			break;
		case GLUT_KEY_DOWN:
			state->setKeys(KEY_DOWN, true);
			break;
		case GLUT_KEY_LEFT:
			state->setKeys(KEY_LEFT, true);
			break;
		case GLUT_KEY_RIGHT:
			state->setKeys(KEY_RIGHT, true);
			break;
		default:
			break;
	} // switch

	// ****************************************

	if (planeModeOn || state->isGameOver()) return; // do not check following keys when player is flying or game is over
	switch (key)
	{
		case GLUT_KEY_F1: // real camera
			if (realCamOn) break;
			saveOldCamPos();
			cam->setPos(lastRealCamPos);
			cam->setAngleX(lastRealCamAngleX);
			cam->setAngleY(lastRealCamAngleY);

			realCamOn = true;
			freeCamOn = false;
			towerCamOn = false;
			runwayCamOn = false;
			helicopterCamOn = false;
			airportExhCamOn = false;
			break;
		case GLUT_KEY_F2: // free camera
			if (freeCamOn) break;
			saveOldCamPos();
			cam->setPos(lastFreeCamPos);
			cam->setAngleX(lastFreeCamAngleX);
			cam->setAngleY(lastFreeCamAngleY);

			realCamOn = false;
			freeCamOn = true;
			towerCamOn = false;
			runwayCamOn = false;
			helicopterCamOn = false;
			airportExhCamOn = false;
			break;
		case GLUT_KEY_F3: // tower static camera
			if (towerCamOn) break;
			saveOldCamPos();

			realCamOn = false;
			freeCamOn = false;
			towerCamOn = true;
			runwayCamOn = false;
			helicopterCamOn = false;
			airportExhCamOn = false;
			break;
		case GLUT_KEY_F4: // runway static camera
			if (runwayCamOn) break;
			saveOldCamPos();

			realCamOn = false;
			freeCamOn = false;
			towerCamOn = false;
			runwayCamOn = true;
			helicopterCamOn = false;
			airportExhCamOn = false;
			break;
		case GLUT_KEY_F5: // helicopter dynamic camera
			if (helicopterCamOn || !helicopter) break;
			saveOldCamPos();

			realCamOn = false;
			freeCamOn = false;
			towerCamOn = false;
			runwayCamOn = false;
			helicopterCamOn = true;
			airportExhCamOn = false;
			break;
		case GLUT_KEY_F6: // airport exhibition dynamic camera
			if (airportExhCamOn) break;
			saveOldCamPos();
			cam->setStartTime(state->getElapsedTime());

			realCamOn = false;
			freeCamOn = false;
			towerCamOn = false;
			runwayCamOn = false;
			helicopterCamOn = false;
			airportExhCamOn = true;
			break;
		default:
			break;
	} // switch
} // ON SPECIAL KEY PRESSED

// ========================================

void onSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
			state->setKeys(KEY_UP, false);
			break;
		case GLUT_KEY_DOWN:
			state->setKeys(KEY_DOWN, false);
			break;
		case GLUT_KEY_LEFT:
			state->setKeys(KEY_LEFT, false);
			break;
		case GLUT_KEY_RIGHT:
			state->setKeys(KEY_RIGHT, false);
			break;
		default:
			break;
	} // switch
} // ON SPECIAL KEY RELEASED

// ========================================
// MAIN
// ========================================

int main(int argc, char **argv)
{
	glutInit(&argc, argv); // init GLUT library

	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextVersion(OGL_VER_MAJOR, OGL_VER_MINOR);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GL_MULTISAMPLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(5, 10);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);

	glutCreateWindow(WIN_TITLE);

	// register callbacks
	glutDisplayFunc(onDisplay);
	glutCloseFunc(onClose);
	glutReshapeFunc(onReshape);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutPassiveMotionFunc(onMotion);
	glutTimerFunc(REFRESH_INTERVAL, onTimer, 0);
	glutKeyboardFunc(onKeyPressed);
	glutKeyboardUpFunc(onKeyReleased);
	glutSpecialFunc(onSpecialKeyPressed);
	glutSpecialUpFunc(onSpecialKeyReleased);

	// check version
	if (!initialize(OGL_VER_MAJOR, OGL_VER_MINOR))
		dieWithError("PGR INIT FAILED – REQUIRED OPENGL NOT SUPPORTED?");

	init();

	glutSetCursor(GLUT_CURSOR_NONE); // hide pointer
	glutWarpPointer(state->getWinW()/2, state->getWinH()/2); // bind pointer with screen center
	glutMainLoop(); // start processing events

	return 0;
} // MAIN
