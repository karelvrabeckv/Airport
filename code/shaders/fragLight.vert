#version 400

// uniforms
uniform mat4 pMat;
uniform mat4 vMat;
uniform mat4 mMat;
uniform mat4 nMat;
uniform float mistDen;

// inputs
in vec3 vertPos;
in vec3 vertNor;
in vec2 texCoo;

// outputs
smooth out mat4 vMat_fs;
smooth out vec3 vertPos_fs;
smooth out vec3 vertNor_fs;
smooth out vec2 texCoo_fs;
smooth out float mistFact_fs;

// ========================================

void main()
{
  gl_Position = pMat * vMat * mMat * vec4(vertPos, 1.0); // set the vertex position

	// set position and normal due to fragment lighting
	vec3 position = (vMat * mMat * vec4(vertPos, 1.0)).xyz;
	vec3 normal = normalize((vMat * nMat * vec4(vertNor, 0.0)).xyz);

	// set mist factor
	vec4 pos = vMat * mMat * vec4(vertPos, 1.0);
	float mistFact = clamp(exp(-mistDen * abs(pos.z)), 0.0, 1.0);
	
	vMat_fs = vMat;
	vertPos_fs = position;
	vertNor_fs = normal;
	texCoo_fs = texCoo;
	mistFact_fs = mistFact;
} // MAIN
