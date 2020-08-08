#include "headers/object.h"
#include "headers/data.h"
#include "headers/spline.h"

// ========================================

vec3  Object::getDefPos()                   {return this->defPos;}
void  Object::setDefPos(vec3 defPos)        {this->defPos = defPos;}

vec3  Object::getPos()                      {return this->pos;}
void  Object::setPos(vec3 pos)              {this->pos = pos;}

vec3  Object::getDir()                      {return this->dir;}
void  Object::setDir(vec3 direction)        {this->dir = direction;}

vec3  Object::getSize()                     {return this->size;}
void  Object::setSize(vec3 size)            {this->size = size;}

vec3  Object::getBoundBox()                 {return this->boundBox;}
void  Object::setBoundBox(vec3 boundBox)    {this->boundBox = boundBox;}

vec3  Object::getAngle()                    {return this->angle;}
void  Object::setAngle(vec3 angle)          {this->angle = angle;}

float Object::getCurrSpeed()                {return this->currSpeed;}
void  Object::setCurrSpeed(float currSpeed) {this->currSpeed = currSpeed;}

float Object::getMaxSpeed()                 {return this->maxSpeed;}
void  Object::setMaxSpeed(float maxSpeed)   {this->maxSpeed = maxSpeed;}

float Object::getAccel()                    {return this->accel;}
void  Object::setAccel(float accel)         {this->accel = accel;}

float Object::getStartTime()                {return this->startTime;}
void  Object::setStartTime(float startTime) {this->startTime = startTime;}

float Object::getCurrTime()                 {return this->currTime;}
void  Object::setCurrTime(float currTime)   {this->currTime = currTime;}

// ========================================

/** Checks whether the player is nearby the object (e.g. plane). */
bool Object::isPlayerNearby(vec3 myPos)
{
	const float x = abs(myPos.x - this->pos.x);
	const float y = abs(myPos.y - this->pos.y);
	const float z = abs(myPos.z - this->pos.z);

	if ((x <= (this->boundBox.x + 0.5f)) &&
		  (y <= (this->boundBox.y + 0.5f)) &&
		  (z <= (this->boundBox.z + 0.5f)))
		return true; // player is near the plane

	return false; // player is too far
} // IS PLAYER NEARBY

// ========================================

/** Updates moving object on the curve. */
void Object::update(State* state, const vec3* curveData, size_t curveSize)
{
	float time = this->currSpeed * (this->currTime - this->startTime);

	this->setPos(evaluateClosedCurve(curveData, curveSize, time));
	this->setDir(normalize(evaluateClosedCurve_1stDerivative(curveData, curveSize, time)));
} // UPDATE

// ========================================

void Object::draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix)
{
	glUseProgram(program);

	// transform model
	mMatrix = translate(mMatrix, this->pos);
	mMatrix = rotate(mMatrix, radians(this->angle.z), Z_AXIS);
	mMatrix = rotate(mMatrix, radians(this->angle.y), Y_AXIS);
	mMatrix = rotate(mMatrix, radians(this->angle.x), X_AXIS);
	mMatrix = scale(mMatrix, this->size);

	mat4 nMatrix = transpose(inverse(mat4(mMatrix[0], mMatrix[1], mMatrix[2], vec4(0.0f, 0.0f, 0.0f, 1.0f))));

	// send data to vertex shader
	glUniformMatrix4fv(glGetUniformLocation(program, P_MAT_VAR), 1, GL_FALSE, value_ptr(pMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, V_MAT_VAR), 1, GL_FALSE, value_ptr(vMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, M_MAT_VAR), 1, GL_FALSE, value_ptr(mMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, N_MAT_VAR), 1, GL_FALSE, value_ptr(nMatrix));

	for (size_t i = 0; i < model.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(program, VERT_AMB_VAR), 1, value_ptr(model[i]->getAmbient()));
		glUniform3fv(glGetUniformLocation(program, VERT_DIF_VAR), 1, value_ptr(model[i]->getDiffuse()));
		glUniform3fv(glGetUniformLocation(program, VERT_SPE_VAR), 1, value_ptr(model[i]->getSpecular()));
		glUniform1f(glGetUniformLocation(program, VERT_SHI_VAR), model[i]->getShininess());

		glUniform1i(glGetUniformLocation(program, TEX_ON_VAR), 0);
		if (model[i]->getTexture() != 0)
		{
			// draw texture
			glUniform1i(glGetUniformLocation(program, TEX_SAM_VAR), 0);
			glUniform1i(glGetUniformLocation(program, TEX_ON_VAR), 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model[i]->getTexture());
		} // if

		// draw vertices
		glBindVertexArray(model[i]->getVao());
		glDrawElements(GL_TRIANGLES /* mode */, model[i]->getNumTriangles() * 3 /* count */, GL_UNSIGNED_INT /* type */, nullptr /* indices */);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	} // for

	glUseProgram(0);
} // DRAW

// ========================================

void Helicopter::draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix)
{
	glUseProgram(program);

	// transform model
	mMatrix = translate(mMatrix, this->pos);
	mMatrix = alignObject(this->pos, this->dir, Y_AXIS);
	mMatrix = rotate(mMatrix, radians(-25.0f), X_AXIS);
	mMatrix = rotate(mMatrix, radians(-90.0f), Y_AXIS);
	mMatrix = scale(mMatrix, this->size);

	mat4 nMatrix = transpose(inverse(mat4(mMatrix[0], mMatrix[1], mMatrix[2], vec4(0.0f, 0.0f, 0.0f, 1.0f))));

	// send data to vertex shader
	glUniformMatrix4fv(glGetUniformLocation(program, P_MAT_VAR), 1, GL_FALSE, value_ptr(pMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, V_MAT_VAR), 1, GL_FALSE, value_ptr(vMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, M_MAT_VAR), 1, GL_FALSE, value_ptr(mMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, N_MAT_VAR), 1, GL_FALSE, value_ptr(nMatrix));

	for (size_t i = 0; i < model.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(program, VERT_AMB_VAR), 1, value_ptr(model[i]->getAmbient()));
		glUniform3fv(glGetUniformLocation(program, VERT_DIF_VAR), 1, value_ptr(model[i]->getDiffuse()));
		glUniform3fv(glGetUniformLocation(program, VERT_SPE_VAR), 1, value_ptr(model[i]->getSpecular()));
		glUniform1f(glGetUniformLocation(program, VERT_SHI_VAR), model[i]->getShininess());

		glUniform1i(glGetUniformLocation(program, TEX_ON_VAR), 0);
		if (model[i]->getTexture() != 0)
		{
			// draw texture
			glUniform1i(glGetUniformLocation(program, TEX_SAM_VAR), 0);
			glUniform1i(glGetUniformLocation(program, TEX_ON_VAR), 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model[i]->getTexture());
		} // if

		// draw vertices
		glBindVertexArray(model[i]->getVao());
		glDrawElements(GL_TRIANGLES, model[i]->getNumTriangles() * 3, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	} // for

	glUseProgram(0);
} // DRAW

// ========================================

void Explosion::draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE /* rendered fragment */, GL_ONE /* fragment in frame buffer */);
	glUseProgram(program);

	// inverse view rotation
	mat4 explosionRotMat = transpose(mat4(vMatrix[0], vMatrix[1], vMatrix[2], vec4(0.0f, 0.0f, 0.0f, 1.0f)));

	// transform model
	mMatrix = translate(mMatrix, this->pos);
	mMatrix = scale(mMatrix, this->size);
	mMatrix = mMatrix * explosionRotMat; // explosion now face the camera

	// send data to vertex shader
	glUniformMatrix4fv(glGetUniformLocation(program, P_MAT_VAR), 1, GL_FALSE, value_ptr(pMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, V_MAT_VAR), 1, GL_FALSE, value_ptr(vMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, M_MAT_VAR), 1, GL_FALSE, value_ptr(mMatrix));

	glUniform1f(glGetUniformLocation(program, TIME_VAR), this->currTime - this->startTime);

	for (size_t i = 0; i < model.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(program, VERT_AMB_VAR), 1, value_ptr(model[i]->getAmbient()));
		glUniform3fv(glGetUniformLocation(program, VERT_DIF_VAR), 1, value_ptr(model[i]->getDiffuse()));
		glUniform3fv(glGetUniformLocation(program, VERT_SPE_VAR), 1, value_ptr(model[i]->getSpecular()));
		glUniform1f(glGetUniformLocation(program, VERT_SHI_VAR), model[i]->getShininess());

		// draw texture
		glUniform1i(glGetUniformLocation(program, TEX_SAM_VAR), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model[i]->getTexture());

		// draw vertices
		glBindVertexArray(model[i]->getVao());
		glDrawArrays(GL_TRIANGLE_STRIP /* mode */, 0 /* first */, model[i]->getNumTriangles() /* count */);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	} // for

	glUseProgram(0);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
} // DRAW

// ========================================

void GameOver::draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(program);

	// inverse view rotation
	mat4 gameOverRotMat = transpose(mat4(vMatrix[0], vMatrix[1], vMatrix[2], vec4(0.0f, 0.0f, 0.0f, 1.0f)));

	// transform model
	mMatrix = translate(mMatrix, this->pos);
	mMatrix = scale(mMatrix, this->size);
	mMatrix = mMatrix * gameOverRotMat; // game over now face the camera

	// send data to vertex shader
	glUniformMatrix4fv(glGetUniformLocation(program, P_MAT_VAR), 1, GL_FALSE, value_ptr(pMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, V_MAT_VAR), 1, GL_FALSE, value_ptr(vMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, M_MAT_VAR), 1, GL_FALSE, value_ptr(mMatrix));

	glUniform1f(glGetUniformLocation(program, TIME_VAR), this->currTime - this->startTime);

	for (size_t i = 0; i < model.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(program, VERT_AMB_VAR), 1, value_ptr(model[i]->getAmbient()));
		glUniform3fv(glGetUniformLocation(program, VERT_DIF_VAR), 1, value_ptr(model[i]->getDiffuse()));
		glUniform3fv(glGetUniformLocation(program, VERT_SPE_VAR), 1, value_ptr(model[i]->getSpecular()));
		glUniform1f(glGetUniformLocation(program, VERT_SHI_VAR), model[i]->getShininess());

		// draw texture
		glUniform1i(glGetUniformLocation(program, TEX_SAM_VAR), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model[i]->getTexture());

		// draw vertices
		glBindVertexArray(model[i]->getVao());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, model[i]->getNumTriangles());
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	} // for

	glUseProgram(0);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
} // DRAW

// ========================================

void Skybox::draw(GLuint program, vector <Mesh*> &model, mat4 pMatrix, mat4 vMatrix, mat4 mMatrix)
{
	glUseProgram(program);
	glDepthMask(GL_FALSE);

	// send data to vertex shader
	glUniformMatrix4fv(glGetUniformLocation(program, P_MAT_VAR), 1, GL_FALSE, value_ptr(pMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, V_MAT_VAR), 1, GL_FALSE, value_ptr(mat4(mat3(vMatrix)))); // remove translation
	glUniformMatrix4fv(glGetUniformLocation(program, M_MAT_VAR), 1, GL_FALSE, value_ptr(mMatrix));

	for (size_t i = 0; i < model.size(); i++)
	{
		// draw texture
		glUniform1i(glGetUniformLocation(program, TEX_SAM_VAR), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, model[i]->getTexture());

		// draw vertices
		glBindVertexArray(model[i]->getVao());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // unbind texture
	} // for

	glDepthMask(GL_TRUE);
	glUseProgram(0);
} // DRAW
