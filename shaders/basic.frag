#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;


out vec4 fColor;

// fog
uniform int fog_enable;
uniform float fog_density;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 64.0f;


//lumina spot
uniform int spot_enable;
float quad_spot = 0.02f;
float liniar_spot = 0.09f;
float constant_spot = 1.0f;
vec3 spot_ambient = vec3(0.0f, 0.0f, 0.0f);
vec3 spot_specular = vec3(1.0f, 1.0f, 1.0f);
vec3 spot_color = vec3(3,1,1);
uniform float lumina_spot1;
uniform float lumina_spot2;
uniform vec3 directieLuminaSpot;
uniform vec3 pozitieLuminaSpot;


//lumina punctiforma
uniform int luminaPunctiformaEnable;
uniform vec3  luminaPunctiforma;
float constant = 1.0f;
float linear = 1.0f;
float quadratic = 1.0;
float ambientPoint = 2.0f;
float specularStrengthPoint = 2.5f;
float shininessPoint = 30.0f;


vec3 computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;

   return min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
}

//pentru umbre
float computeShadow() {
    
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    if (normalizedCoords.z > 1.0f) {
        return 0.0f;
    }
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = 0.005f;
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    return shadow;
}

//lumina pozitionala
vec3 luminaPozitionala(vec4 lumina)
{
	vec3 normalEye = normalize(normalMatrix * fNormal);
	vec3 lightDirN = normalize(lumina.xyz - fragPosEye.xyz);
	vec3 viewDirN = normalize(vec3(0.0f) - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininessPoint);
	vec3 specular = specularStrengthPoint * specCoeff * lightColor;
	float distance = length(lumina.xyz - fragPosEye.xyz);
	float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
	return (ambientPoint * lightColor + max(dot(normalEye, lightDirN), 0.0f) * lightColor + specular) * att * vec3(1.0f,1.0f,1.0f);
}


//lumina de tip spot
vec3 luminaSpot() {
	vec3 lightDir = normalize(pozitieLuminaSpot - fPosition);
	vec3 normalEye = normalize(normalMatrix * fNormal);
	vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));
	vec3 viewDirN = normalize(vec3(0.0f) - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	float spec = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	float distanta = length(pozitieLuminaSpot - fPosition);
	float atenuare = 1.0f / (constant_spot + liniar_spot * distanta + quad_spot * distanta * distanta);

	float intensitate = clamp((dot(lightDir, normalize(-directieLuminaSpot)) - lumina_spot2)/(lumina_spot1 - lumina_spot2), 0.0, 1.0);

	vec3 ambient = spot_color * spot_ambient * vec3(texture(diffuseTexture, fTexCoords))*atenuare * intensitate;
	vec3 diffuse = spot_color * spot_specular * max(dot(fNormal, lightDir), 0.0f) * vec3(texture(diffuseTexture, fTexCoords))*atenuare * intensitate;
	vec3 specular = spot_color * spot_specular * spec * vec3(texture(specularTexture, fTexCoords))*atenuare * intensitate;
	
	return ambient + diffuse + specular;
}

float computeFog()
{

 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fog_density, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    vec3 light = computeDirLight();
    float shadow = computeShadow();

    //lumina de tip spot
	if (spot_enable == 1){
	light += luminaSpot();
	}
    
    //lumina punctiforma
	
	if (luminaPunctiformaEnable ==1 ){
	vec4 lightPosEye1 = view * vec4(luminaPunctiforma, 1.0f);
	light += luminaPozitionala(lightPosEye1);
        vec4 diffuseColor = texture(diffuseTexture, fTexCoords);
          }
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	ambient *= vec3(texture(diffuseTexture, fTexCoords))*2.0f;
	diffuse *= vec3(texture(diffuseTexture, fTexCoords))*2.0f;
	specular *= vec3(texture(specularTexture, fTexCoords))*2.0f;
	
    //compute final vertex color
    vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);
    vec4 colorFinal = vec4(color,1.0f);
	if (fog_enable == 0)
	{
		fColor = min(colorFinal*vec4(light, 1.0f),1.0f);
		
	}
	else
	{
		fColor = mix(fogColor, min(colorFinal*vec4(light, 1.0f),1.0f), fogFactor);	
	}
}
