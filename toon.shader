#shader vertex
#version 400 core

in vec4 vPosition;
in vec3 vNormal;

out vec3 fNormal;
out vec3 fToLight;
out vec3 fToEye;

uniform mat4 uViewMatrix;
uniform mat4 uTransformMatrix;
uniform mat4 uProjectionMatrix;

uniform vec3 uLightPosition;
uniform vec3 uEyePosition;

void main()
{
	// World position:
	gl_Position = uProjectionMatrix * uViewMatrix * uTransformMatrix * vPosition;

	fNormal = vNormal;
	fToLight = uLightPosition - vPosition.xyz;
	fToEye = vec3(0., 0., 0.) - gl_Position.xyz;
};


#shader fragment
#version 400 core

in vec3 fNormal;
in vec3 fToLight;
in vec3 fToEye;

uniform float uAmbient;
uniform float uDiffuse;
uniform float uSpecular;
uniform float uShininess;
uniform vec3 uSpecularColor;

out vec4 color;


void main()
{
	// Lighting:
	vec3 unitToLight = normalize(fToLight);
	vec3 unitToEye = normalize(fToEye);

	// Ambient:
	vec3 ambient = uAmbient * vec3(0.0, 1.0, 0.0);

	// Diffuse:
	float d = max(dot(fNormal, unitToLight), 0);
	vec3 diffuse = uDiffuse * d * vec3(0.0, 1.0, 0.0);

	// Specular:
	float s = 0;
	if (dot(fNormal, unitToLight) > 0)
	{
		vec3 ref = normalize(reflect(unitToLight, fNormal));
		s = pow(max(dot(unitToEye, ref), 0), uShininess);
	}
	vec3 specular = uSpecular * s * uSpecularColor;

	color = vec4(ambient + diffuse + specular, 1);
};
