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

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
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

const float PI = 3.14159265359;

// calculate the approximated fresnel term
// this funciton is refering to learnopengl site
// https://learnopengl.com/PBR/Lighting
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// calculate the distribution term
// this funciton is refering to learnopengl site
// https://learnopengl.com/PBR/Lighting
float DistributionGGX(vec3 N, vec3 H, float roughtness)
{
	float a = roughtness * roughtness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

// calculate the geometry term
// this funciton is refering to learnopengl site
// https://learnopengl.com/PBR/Lighting
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

// calculate the geometry term
// this funciton is refering to learnopengl site
// https://learnopengl.com/PBR/Lighting
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// calculate results of directional lights
vec3 CalcLightDirectional(LightDirectional light, vec3 uNormal, vec3 dirToCamera)
{
	vec3 N = uNormal;
	vec3 V = dirToCamera; // vector from the fragment to the camera
	vec3 L = light.dirToLight; // incident direction
	vec3 H = normalize(light.dirToLight + dirToCamera); // half vector
	vec3 radiance = light.color;

	// fresnel term
	vec3 F0 = vec3(0.04);
	F0	= mix(F0, albedo, metallic);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	// distribution term & geometry term
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);

	vec3 kS = F;
	vec3 kD = vec3(1.0) -kS;

	kD *= 1.0 - metallic;

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N, L), 0.0);
	vec3 result = (kD * albedo / PI + specular) * radiance * NdotL;

	vec3 ambient = 0.03 * albedo;
	result += ambient;

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

