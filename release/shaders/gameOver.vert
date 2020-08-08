#version 400

// uniforms
uniform mat4 pMat;
uniform mat4 vMat;
uniform mat4 mMat;
uniform float time;

// inputs
in vec3 vertPos;
in vec2 texCoo;

// outputs
smooth out vec2 texCoo_fs;

// ========================================

void main()
{
  gl_Position = pMat * vMat * mMat * vec4(vertPos, 1.0); // set the vertex position

  float localTime = 0.05 * time; // slow motion
  vec2 offset = vec2(-3.0 - (floor(localTime) - localTime) * 4, 0.0); // offset

  texCoo_fs = texCoo + offset;
} // MAIN
