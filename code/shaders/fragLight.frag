#version 400

// light
struct Light
{
	vec3 pos, dir, amb, dif, spe;
	float cosCutOff, expo;
};

// uniforms
uniform vec3 vertAmb;
uniform vec3 vertDif;
uniform vec3 vertSpe;
uniform float vertShi;
uniform sampler2D texSam;
uniform Light lights[18];
uniform float mistCol;
uniform bool dayOn;
uniform bool flaOn;
uniform bool mistOn;
uniform bool texOn;

// inputs
smooth in mat4 vMat_fs;
smooth in vec3 vertPos_fs;
smooth in vec3 vertNor_fs;
smooth in vec2 texCoo_fs;
smooth in float mistFact_fs;

// outputs
out vec4 color;

// ========================================

/** Computes directional lighting. */
vec4 dirShine(Light light, vec3 vertPos, vec3 vertNor, vec3 vertAmb, vec3 vertDif, vec3 vertSpe, float vertShi)
{
	vec3 result = vec3(0.0f);
	vec3 lightPos = vec3(vMat_fs * vec4(light.pos, 0.0));

	// calculate all important vectors
	vec3 lDir = normalize(lightPos);
	vec3 rDir = reflect(-lDir, vertNor);
	vec3 vDir = normalize(-vertPos);

	// calculate ambient, diffuse and specular components
	result += light.amb * vertAmb;
	result += max(dot(lDir, vertNor), 0.0) * light.dif * vertDif;
	result += pow(max(dot(rDir, vDir), 0.0), vertShi) * light.spe * vertSpe;

	return vec4(result, 1.0);
} // DIR SHINE

// ========================================

/** Computes reflector lighting. */
vec4 reflectorShine(Light light, vec3 vertPos, vec3 vertNor, vec3 vertAmb, vec3 vertDif, vec3 vertSpe, float vertShi)
{
	vec3 result = vec3(0.0f);
	vec3 lightPos = vec3(vMat_fs * vec4(light.pos, 1.0));
	vec3 lightDir = vec3(vMat_fs * vec4(light.dir, 0.0));

	// calculate all important vectors
	vec3 lDir = normalize(lightPos - vertPos);
	vec3 rDir = reflect(-lDir, vertNor);
	vec3 vDir = normalize(-vertPos);

	// calculate ambient, diffuse and specular components
	result += light.amb * vertAmb;
	result += max(dot(lDir, vertNor), 0.0) * light.dif * vertDif;
	result += pow(max(dot(rDir, vDir), 0.0), vertShi) * light.spe * vertSpe;
	
	// calculate light according to the cone
	if (dot(lightDir, -lDir) >= light.cosCutOff)
		result *= pow(max(dot(lightDir, -lDir), 0.0), light.expo);
	else
		result *= 0.0;

	return vec4(result, 1.0);
} // REFLECTOR SHINE

// ========================================

/** Computes spot lighting. */
vec4 spotShine(Light light, vec3 vertPos, vec3 vertNor, vec3 vertAmb, vec3 vertDif, vec3 vertSpe, float vertShi)
{
	vec3 result = vec3(0.0f);
	vec3 lightPos = vec3(vMat_fs * vec4(light.pos, 1.0));

	// calculate all important vectors
	vec3 lDir = normalize(lightPos - vertPos);
	vec3 rDir = reflect(-lDir, vertNor);
	vec3 vDir = normalize(-vertPos);

	// calculate ambient, diffuse and specular components
	result += light.amb * vertAmb;
	result += max(dot(lDir, vertNor), 0.0) * light.dif * vertDif;
	result += pow(max(dot(rDir, vDir), 0.0), vertShi) * light.spe * vertSpe;
	
	// calculate attenuation
	float distance = length(lightPos - vertPos);
	result *= (1.0 / (0.0 + 1.5 * distance + 0.0 * pow(distance, 2)));

	return vec4(result, 1.0);
} // SPOT SHINE

// ========================================

void main()
{
	// set global lighting
  vec3 globalAmbient = vec3(0.25f);
	vec4 lightedColor = vec4(vertAmb * globalAmbient, 0.0);

	// set lighting for each vertex
	if (dayOn)
		lightedColor += dirShine(lights[0], vertPos_fs, normalize(vertNor_fs), vertAmb, vertDif, vertSpe, vertShi);

	if (flaOn)
		lightedColor += reflectorShine(lights[1], vertPos_fs, normalize(vertNor_fs), vertAmb, vertDif, vertSpe, vertShi);

	if (!dayOn || mistOn)
	{
		// three lamps at hangar
		for (int i = 2; i < 5; i++)
			lightedColor += reflectorShine(lights[i], vertPos_fs, normalize(vertNor_fs), vertAmb, vertDif, vertSpe, vertShi);
			
		// three lights on tower, antenna and heliport
		for (int i = 5; i < 8; i++)
			lightedColor += spotShine(lights[i], vertPos_fs, normalize(vertNor_fs), vertAmb, vertDif, vertSpe, vertShi);

		// ten lights at runway
		for (int i = 8; i < 18; i++)
			lightedColor += spotShine(lights[i], vertPos_fs, normalize(vertNor_fs), vertAmb, vertDif, vertSpe, vertShi);
	} // if

	// set the final color
	if (texOn && mistOn)
		color = mistFact_fs * lightedColor * texture(texSam, texCoo_fs) + (1 - mistFact_fs) * mistCol;
	else if (!texOn && mistOn)
		color = mistFact_fs * lightedColor + (1 - mistFact_fs) * mistCol;
	else if (texOn && !mistOn)
		color = lightedColor * texture(texSam, texCoo_fs);
	else
		color = lightedColor;
} // MAIN
