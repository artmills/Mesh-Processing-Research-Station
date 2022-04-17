#shader vertex
#version 400 core

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
in vec2 vTexture;
in vec3 vBarycentric;
in vec4 vHighlight;

out vec4 fColor;
out vec3 fNormal;
out vec3 fToLight;
out vec2 fTexture;
out vec3 fToEye;
out vec4 fLightSpace;
out vec3 fBarycentric;
out vec4 fHighlight;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uTransformMatrix;

uniform vec3 uLightPosition;
uniform vec3 uEyePosition;

uniform mat4 uLightViewMatrix;
uniform mat4 uLightPerspectiveMatrix;

void main()
{
	// World position:
	gl_Position = uProjectionMatrix * uViewMatrix * uTransformMatrix * vPosition;

	// Shadow map position:
	fLightSpace = uLightPerspectiveMatrix * uLightViewMatrix* uTransformMatrix * vPosition;

	fColor = vColor;
	fNormal = vNormal;
	fTexture = vTexture;
	fToLight = uLightPosition - vPosition.xyz;
	fToEye = vec3(0., 0., 0.) - gl_Position.xyz;
	fBarycentric = vBarycentric;
	fHighlight = vHighlight;
};

#shader fragment
#version 400 core

in vec4 fColor;
in vec3 fNormal;
in vec3 fToLight;
in vec2 fTexture;
in vec3 fToEye;
in vec4 fLightSpace;
in vec3 fBarycentric;
in vec4 fHighlight;

uniform float uAmbient;
uniform float uDiffuse;
uniform float uSpecular;
uniform float uShininess;
uniform vec3 uSpecularColor;

uniform int uWireframe;
const float THICKNESS = 0.005;

out vec4 color;

void main()
{
	int WIREFRAME = uWireframe < 1 ? 1 : 0;
	
	vec4 drawColor = (fHighlight.r > 0) ? fHighlight : fColor;
	/*
	if (fHighlight.r > 0)
	{
		fColor = fHighlight;
	}
	*/

	//drawColor = vec4(0, 1, 0.2, 1);

	// Normalize:
	vec3 unitToLight = normalize(fToLight);
	vec3 unitToEye = normalize(fToEye);

	// Ambient:
	vec3 ambient = uAmbient * drawColor.xyz;

	// Diffuse:
	float d = max(dot(fNormal, unitToLight), 0);
	vec3 diffuse = uDiffuse * d * drawColor.xyz;

	// Specular:
	float s = 0;
	if (dot(fNormal, unitToLight) > 0)
	{
		vec3 ref = normalize(reflect(unitToLight, fNormal));
		s = pow(max(dot(unitToEye, ref), 0), uShininess);
	}
	vec3 specular = uSpecular * s * uSpecularColor;

	float closestEdge = min(fBarycentric.x, min(fBarycentric.y, fBarycentric.z));
	float width = fwidth(closestEdge);
	float edge = max(WIREFRAME, smoothstep(THICKNESS, THICKNESS + width, closestEdge));
	//float edge = smoothstep(THICKNESS, THICKNESS + width, closestEdge);
	vec3 edgeVec = vec3(edge);

	color = vec4(min(edgeVec, vec3(ambient + diffuse + specular)), 1);
	//color = vec4(edge * vec3(ambient + diffuse + specular), 1);
	//color = vec4(ambient + specular + diffuse, 1);
};
