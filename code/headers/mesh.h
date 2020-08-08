#pragma once

using namespace glm;

// ========================================

class Mesh
{
	private:

		GLuint vbo, ebo, vao;
		unsigned int numTriangles, texture;
		vec3 ambient, diffuse, specular;
		float shininess;

	public:

		GLuint getVbo();
		GLuint *getAddressVbo();

		GLuint getEbo();
		GLuint *getAddressEbo();

		GLuint getVao();
		GLuint *getAddressVao();

		unsigned int getNumTriangles();
		void setNumTriangles(unsigned int numOfTriangles);

		unsigned int getTexture();
		void setTexture(unsigned int texture);

		vec3 getAmbient();
		void setAmbient(vec3 ambient);

		vec3 getDiffuse();
		void setDiffuse(vec3 diffuse);

		vec3 getSpecular();
		void setSpecular(vec3 specular);

		float getShininess();
		void setShininess(float shininess);

		// ****************************************

		void createSkyboxMesh
		(
			GLuint program,
			const char *right, const char *left,
			const char *top, const char *bottom,
			const char *front, const char *back
		);
		void createSpotLightMesh(GLuint program);
		void createExplosionMesh(GLuint program);
		void createGameOverMesh(GLuint program);
};
