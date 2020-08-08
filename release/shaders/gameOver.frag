#version 400

// uniforms
uniform sampler2D texSampler;

// inputs
smooth in vec2 texCoo_fs;

// outputs
out vec4 color;

// ========================================

void main()
{
  color = texture(texSampler, texCoo_fs);
} // MAIN
