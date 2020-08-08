#version 400

// uniforms
uniform float time;
uniform sampler2D texSampler;
uniform ivec2 pattern = ivec2(8, 2);
uniform float frameDuration = 0.1f;

// inputs
smooth in vec2 texCoo_fs;

// outputs
out vec4 color;

// ========================================

void main()
{
  int frame = int(time / frameDuration); // numbers from 0 to 15
  vec2 texCoords = texCoo_fs / pattern /* first frame */ + vec2(frame, int(frame / pattern.x)) /* amount */ * (vec2(1.0) / pattern) /* offset */;

  color = texture(texSampler, texCoords);
} // MAIN
