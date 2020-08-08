#version 400

// uniforms
uniform mat4 pMat;
uniform mat4 vMat;
uniform mat4 mMat;

// inputs
in vec3 vertPos;
in vec3 texCoo;

// outputs
smooth out vec3 texCoo_fs;

// ========================================

void main()
{
  gl_Position = pMat * vMat * mMat * vec4(vertPos, 1.0); // set the vertex position
	
	texCoo_fs = texCoo;
} // MAIN
