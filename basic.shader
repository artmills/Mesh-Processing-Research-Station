#shader vertex
#version 400 core

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
in vec2 vTexture;
in vec3 vBarycentric;

out vec4 fColor;
out vec3 fNormal;
out vec3 fToLight;
out vec2 fTexture;
out vec3 fToEye;
out vec4 fLightSpace;
out vec3 fBarycentric;

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

uniform float uAmbient;
uniform float uDiffuse;
uniform float uSpecular;
uniform float uShininess;
uniform vec3 uSpecularColor;

uniform sampler2D uShadowMap;
const float BIAS = 0.01;

uniform sampler2D uTexture;

const float THICKNESS = 0.001;

out vec4 color;

bool IsInShadow(vec4 lightSpace)
{
	// Convert from light space to shadowmap space:
	vec3 projection = lightSpace.xyz / lightSpace.w;
	projection = 0.5 * projection + 0.5;

	// Closest depth from the light's perspective in the shadow map.
	float nearestDepth = texture(uShadowMap, projection.xy).r;	

	// Depth of the current fragment.
	float currentDepth = projection.z;
	
	// Now do the check:
	bool isInShadow = (currentDepth - BIAS) > nearestDepth;
	return isInShadow;
}

void main()
{
	// Normalize:
	vec3 unitToLight = normalize(fToLight);
	vec3 unitToEye = normalize(fToEye);

	// Ambient:
	vec3 ambient = uAmbient * fColor.xyz;

	// Diffuse:
	float d = max(dot(fNormal, unitToLight), 0);
	float shadowFactor = 1.0;
	if (IsInShadow(fLightSpace))
	{
		shadowFactor = 0.1;	
	}
	vec3 diffuse = uDiffuse * d * shadowFactor * fColor.xyz;

	// Specular:
	float s = 0;
	if (dot(fNormal, unitToLight) > 0)
	{
		vec3 ref = normalize(reflect(unitToLight, fNormal));
		s = pow(max(dot(unitToEye, ref), 0), uShininess);
	}
	vec3 specular = uSpecular * s * uSpecularColor;

	color = fColor;

	float closestEdge = min(fBarycentric.x, min(fBarycentric.y, fBarycentric.z));
	float width = fwidth(closestEdge);
	float edge = smoothstep(THICKNESS, THICKNESS + width, closestEdge);

	//color = vec4(edge, edge, edge, 1);

	//color = vec4(ambient + diffuse + specular, 1);
	color = vec4(edge * vec3(ambient + diffuse + specular), 1);
};
