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

uniform float shininess;
uniform float ambinent_weight;
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


// calculate results of directional lights
vec3 CalcLightDirectional(LightDirectional light, vec3 uNormal, vec3 dirToCamera)
{
	// calculate the angle between the normal and the light direction
	float diffIntensity = max(dot(uNormal, light.dirToLight), 0);
	vec3 diffColor = 0.2 * diffIntensity * light.color * vec3(0.5f, 0.5f, 0.5f);
	
	// calculate the angle between the half vector and the normal as shininess
	vec3 H = normalize(light.dirToLight + dirToCamera);
	float specIntensity = pow(max(dot(H, uNormal), 0), shininess);
	vec3 specColor = 0.2 * specIntensity * light.color;

	// simply calcualte the effect of ambinent color
	vec3 ambientColor = ambinent_weight * vec3(0.5f, 0.5f, 0.5f);

	vec3 result = diffColor + specColor + ambientColor;
	
	return result;
};

void main() 
{
	vec3 finalResult = vec3(0, 0, 0);
	vec3 uNormal = normalize(normal);
	vec3 dirToCamera = normalize(viewPos - fragPos);

	// there is only one directional light need to be considered
	finalResult += CalcLightDirectional(lightD, uNormal, dirToCamera);

	FragColor = vec4(finalResult, 1.0);

}

