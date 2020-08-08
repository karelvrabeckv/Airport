#include <iostream>

#include "pgr.h"
#include "headers/mesh.h"
#include "headers/data.h"

using namespace std;
using namespace glm;
using namespace pgr;

// ========================================

GLuint       Mesh::getVbo()                                     {return this->vbo;}
GLuint      *Mesh::getAddressVbo()                              {return &this->vbo;}

GLuint       Mesh::getEbo()                                     {return this->ebo;}
GLuint      *Mesh::getAddressEbo()                              {return &this->ebo;}

GLuint       Mesh::getVao()                                     {return this->vao;}
GLuint      *Mesh::getAddressVao()                              {return &this->vao;}

unsigned int Mesh::getNumTriangles()                            {return this->numTriangles;}
void         Mesh::setNumTriangles(unsigned int numOfTriangles) {this->numTriangles = numOfTriangles;}

GLuint       Mesh::getTexture()                                 {return this->texture;}
void         Mesh::setTexture(unsigned int texture)             {this->texture = texture;}

vec3         Mesh::getAmbient()                                 {return this->ambient;}
void         Mesh::setAmbient(vec3 ambient)                     {this->ambient = ambient;}

vec3         Mesh::getDiffuse()                                 {return this->diffuse;}
void         Mesh::setDiffuse(vec3 diffuse)                     {this->diffuse = diffuse;}

vec3         Mesh::getSpecular()                                {return this->specular;}
void         Mesh::setSpecular(vec3 specular)                   {this->specular = specular;}

float        Mesh::getShininess()                               {return this->shininess;}
void         Mesh::setShininess(float shininess)                {this->shininess = shininess;}

// ========================================

void Mesh::createSkyboxMesh
(
	GLuint program,
	const char* right, const char* left,
	const char* top, const char* bottom,
	const char* front, const char* back
)
{
	// create vao
	glGenVertexArrays(1, &this->vao); // create name for array
	glBindVertexArray(this->vao); // bind with array

	// create vbo
	glGenBuffers(1, &this->vbo); // create name for buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo); // bind with buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxData), skyboxData, GL_STATIC_DRAW); // store data

	// transfer texture coordinates to vertex shader
	GLint texCooLoc = glGetAttribLocation(program, TEX_COO_VAR);
	glEnableVertexAttribArray(texCooLoc);
	glVertexAttribPointer(texCooLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// transfer positions to vertex shader
	GLint posLoc = glGetAttribLocation(program, VERT_POS_VAR);
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// close binding with vao and vbo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ****************************************

	glActiveTexture(GL_TEXTURE0); // select texture unit
	glGenTextures(1, &this->texture); // create name for texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture); // bind with texture

	string faces[] =
	{
		right, left,
		top, bottom,
		front, back
	};

	GLuint targets[] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	for (int i = 0; i < 6; i++) // load skybox images
	{
		if (!loadTexImage2D(faces[i], targets[i]))
			dieWithError("Error while creating SKYBOX model.");
		cout << "loading texture: " << faces[i] << endl;
	} // for

	// set texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // create mipmap

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // close binding with texture
} // CREATE SKYBOX MESH

// ========================================

void Mesh::createSpotLightMesh(GLuint program)
{
	// create vao
	glGenVertexArrays(1, &this->vao); // create name for array
	glBindVertexArray(this->vao); // bind with array

	// create vbo
	glGenBuffers(1, &this->vbo); // create name for buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo); // bind with buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(spotLightData), spotLightData, GL_STATIC_DRAW); // store data

	// create ebo
	glGenBuffers(1, &this->ebo); // create name for buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo); // bind with buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spotLightIndices), spotLightIndices, GL_STATIC_DRAW); // store data

	// transfer positions to vertex shader
	GLint posLoc = glGetAttribLocation(program, VERT_POS_VAR);
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3 /* size */, GL_FLOAT /* type */, GL_FALSE, 8 * sizeof(float) /* step */, nullptr /* first */);

	// transfer normals to vertex shader
	GLint norLoc = glGetAttribLocation(program, VERT_NOR_VAR);
	glEnableVertexAttribArray(norLoc);
	glVertexAttribPointer(norLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	// close binding with vao, vbo and ebo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// set remaining parameters
	this->numTriangles = 432;
	this->texture      = 0;
	this->ambient      = vec3(1.0f, 1.0f, 1.0f);
	this->diffuse      = vec3(1.0f, 1.0f, 1.0f);
	this->specular     = vec3(1.0f, 1.0f, 1.0f);
	this->shininess    = 10.0f;
} // CREATE SPOT LIGHT MESH

// ========================================

void Mesh::createExplosionMesh(GLuint program)
{
	// create vao
	glGenVertexArrays(1, &this->vao); // create name for array
	glBindVertexArray(this->vao); // bind with array

	// create vbo
	glGenBuffers(1, &this->vbo); // create name for buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo); // bind with buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(explosionData), explosionData, GL_STATIC_DRAW); // store data

	// transfer positions to vertex shader
	GLint posLoc = glGetAttribLocation(program, VERT_POS_VAR);
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3 /* size */, GL_FLOAT /* type */, GL_FALSE, 5 * sizeof(float) /* step */, nullptr /* first */);

	// transfer texture coordinates to vertex shader
	GLint texCooLoc = glGetAttribLocation(program, TEX_COO_VAR);
	glEnableVertexAttribArray(texCooLoc);
	glVertexAttribPointer(texCooLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// close binding with vao and vbo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// set remaining parameters
	this->numTriangles = 4;
	this->texture      = createTexture(EXPLOSION_TEXTURE_SRC);
	this->ambient      = vec3(1.0f, 1.0f, 1.0f);
	this->diffuse      = vec3(1.0f, 1.0f, 1.0f);
	this->specular     = vec3(1.0f, 1.0f, 1.0f);
	this->shininess    = 10.0f;
} // CREATE EXPLOSION MESH

// ========================================

void Mesh::createGameOverMesh(GLuint program)
{
	// create vao
	glGenVertexArrays(1, &this->vao); // create name for array
	glBindVertexArray(this->vao); // bind with array

	// create vbo
	glGenBuffers(1, &this->vbo); // create name for buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo); // bind with buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(gameOverData), gameOverData, GL_STATIC_DRAW); // store data

	// transfer positions to vertex shader
	GLint posLoc = glGetAttribLocation(program, VERT_POS_VAR);
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3 /* size */, GL_FLOAT /* type */, GL_FALSE, 5 * sizeof(float) /* step */, nullptr /* first */);

	// transfer texture coordinates to vertex shader
	GLint texCooLoc = glGetAttribLocation(program, TEX_COO_VAR);
	glEnableVertexAttribArray(texCooLoc);
	glVertexAttribPointer(texCooLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// close binding with vao and vbo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// set remaining parameters
	this->numTriangles = 4;
	this->texture      = createTexture(GAME_OVER_TEXTURE_SRC);
	this->ambient      = vec3(1.0f, 1.0f, 1.0f);
	this->diffuse      = vec3(1.0f, 1.0f, 1.0f);
	this->specular     = vec3(1.0f, 1.0f, 1.0f);
	this->shininess    = 10.0f;

	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // set no repeat filter to texture
	glBindTexture(GL_TEXTURE_2D, 0);
} // CREATE GAME OVER MESH
