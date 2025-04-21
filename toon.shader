#shader vertex
#version 400 core

in vec4 vPosition;
in vec3 vNormal;

out vec3 fNormal;
out vec3 fToLight;
out vec3 fToEye;
out float fCurvature;

uniform mat4 uViewMatrix;
uniform mat4 uTransformMatrix;
uniform mat4 uProjectionMatrix;

uniform vec3 uLightPosition;
uniform vec3 uEyePosition;

uniform samplerBuffer uCurvatureData;

void main()
{
	// World position:
	gl_Position = uProjectionMatrix * uViewMatrix * uTransformMatrix * vPosition;

	// Get curvature data.
	fCurvature = texelFetch(uCurvatureData, gl_VertexID).x;

	fNormal = vNormal;
	fToLight = uLightPosition - vPosition.xyz;
	fToEye = vec3(0., 0., 0.) - gl_Position.xyz;
};


#shader fragment
#version 400 core

in vec3 fNormal;
in vec3 fToLight;
in vec3 fToEye;
in float fCurvature;

uniform float uDiffuse;

uniform int uLevels;
float scaleFactor = 1.0 / uLevels;

out vec4 color;


void main()
{
	// Lighting:
	vec3 unitToLight = normalize(fToLight);
	vec3 unitToEye = normalize(fToEye);

	// Diffuse:
	//float d = fCurvature * max(dot(fNormal, unitToLight), 0);
	//float d = max(dot(fNormal, unitToLight), 0);
	float d = fCurvature;

	// Determine which level this diffuse factor is in.
	d = ceil(d * uLevels) * scaleFactor;

	vec3 diffuse = uDiffuse * d * vec3(0.0, 1.0, 0.0);
	color = vec4(diffuse, 1);
};
