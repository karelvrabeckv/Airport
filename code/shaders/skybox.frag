#version 400

// uniforms
uniform samplerCube texSam;

// inputs
smooth in vec3 texCoo_fs;

// outputs
out vec4 color;

// ========================================

void main()
{
	color = texture(texSam, texCoo_fs);
} // MAIN
