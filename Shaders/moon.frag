#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;


// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;

uniform vec3 lightPos2;
uniform vec4 lightColor2;

uniform vec3 lightPos3;
uniform vec4 lightColor3;

// Gets the position of the camera from the main function
uniform vec3 camPos;

// Additional uniform for sun direction
uniform vec3 sunDirection;


vec4 pointLight()
{	
	vec3 lightVec = lightPos3 - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 8.0;
	float b = 6.7;
	float inten = 16.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.00f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 1.00f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};
	if (sunDirection.y <= 0.0)
   {
        // If the sun is down, point light is on
		return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor3;
        
    }
	// If sun is up, return ambient lighting (0)
	return texture(diffuse0, texCoord) * ambient * lightColor;
}

vec4 direcLight()
{
	// Check if the sun is above the ground (y position > 0)
    if (sunDirection.y <= 0.0)
    {
        // If the sun is down, the moon is shining
        float ambient = 1.00f;
        return texture(diffuse0, texCoord) * ambient * lightColor;
    }
	
	// ambient lighting
	float ambient = 1.00f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(sunDirection);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 1.00f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.00f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos2 - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f) * 0.5f;

	// specular lighting
	float specularLight = 0.20f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	vec4 finalColor = (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor2;

	// Fog calculations
    // float fogStart = 10.0;  // Adjust the starting distance of the fog
    // float fogEnd = 50.0;    // Adjust the ending distance of the fog
    // float fogFactor = clamp((camPos.z - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    // vec3 fogColor = vec3(0.5, 0.5, 0.5);  // Adjust the color of the fog
    
    // if (lightPos2.y > 200){
	//	finalColor.rgb = mix(finalColor.rgb, fogColor, fogFactor);
	// }
	

    return finalColor;
}


void main()
{
	// outputs final color
	FragColor = direcLight() + spotLight() + pointLight();
}