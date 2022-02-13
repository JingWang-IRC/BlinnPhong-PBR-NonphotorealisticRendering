#version 330 core

struct Material{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct LightDirectional{
	vec3 pos;
	vec3 color;
	vec3 dirToLight;
};

struct LightPoint{
	vec3 pos;
	vec3 color;
	vec3 dirToLight;
	float constant;
	float linear;
	float quadratic;
};

struct LightSpot{
vec3 pos;
	vec3 color;
	vec3 dirToLight;
	//float constant;
	//float linear;
	//float quadratic;
	float cosInner;
	float cosOuter;
};

//in vec3 vertexColor;
//in vec2 TexCoord;
in vec3 fragPos;
in vec4 fragPosLightSpace;
in vec3 normal;
in vec2 texCoord;

uniform float diffuse_upper_bound;
uniform float diffuse_lower_bound;
uniform float specular_bound;
uniform Material material;
uniform LightDirectional lightD;
uniform LightDirectional lightD1;
uniform LightPoint lightP0;
uniform LightPoint lightP1;
uniform LightPoint lightP2;
uniform LightPoint lightP3;
uniform LightSpot lightS;
uniform vec3 objectColor;
uniform vec3 ambientColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDirUniform;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

out vec4 FragColor;


vec3 CalcLightDirectional(LightDirectional light, vec3 uNormal, vec3 dirToCamera)
{
	// diffuse max(dot(L, N), 0)
	float diffIntensity = max(dot(uNormal, light.dirToLight), 0);
	vec3 diffColor = 0.2 * diffIntensity * light.color * vec3(0.5f, 0.5f, 0.5f);

	// if the value is larger than the diffuse_upper_bound value, the color will be set to (0.6, 0.6, 0.6)
	// if the value is lower than the diffuse_lower_bound value, the color will be set to (0.1, 0.1, 0.1)
	// if the value is beteen the two bound values, the color will be linearly interpolated
	if (diffColor.r > diffuse_upper_bound)
		diffColor = vec3(0.6f);
		//diffColor = vec3(0.5f, float(diffuse_upper_bound), float(diffuse_upper_bound));
	else if (diffColor.r < diffuse_lower_bound)
		diffColor = vec3(0.1f);
	else
	{
		float r = (diffColor.r - diffuse_lower_bound) / (diffuse_upper_bound - diffuse_lower_bound) * (diffuse_upper_bound - diffuse_lower_bound) + diffuse_lower_bound;
		float g = (diffColor.r - diffuse_lower_bound) / (diffuse_upper_bound - diffuse_lower_bound) * (diffuse_upper_bound - diffuse_lower_bound) + diffuse_lower_bound;
		float b = (diffColor.r - diffuse_lower_bound) / (diffuse_upper_bound - diffuse_lower_bound) * (diffuse_upper_bound - diffuse_lower_bound) + diffuse_lower_bound;
		diffColor = vec3(r, g, b);
	}

	// specular pow(max(dot(R, Cam), 0), shininess)
	vec3 H = normalize(light.dirToLight + dirToCamera);
	float specIntensity = pow(max(dot(H, uNormal), 0), 64);
	vec3 specColor = 0.2 * specIntensity * light.color;

	// similar to the calculation for diffuse reflection, a bound value is created for specular reflection
	if (specColor.r >= specular_bound)
		specColor = vec3(specular_bound);
	else
		specColor = vec3(0.0f);

	//vec3 ambientColor = 0.2 * texture(material.specular, texCoord).rgb;
	vec3 ambientColor = 0.1 * vec3(0.5f, 0.5f, 0.5f);

	vec3 result = diffColor + specColor + ambientColor;
	
	return result;
};

void main() 
{
	vec3 finalResult = vec3(0, 0, 0);
	vec3 uNormal = normalize(normal);
	vec3 dirToCamera = normalize(viewPos - fragPos);

	finalResult += CalcLightDirectional(lightD, uNormal, dirToCamera);

	FragColor = vec4(finalResult, 1.0);



}

