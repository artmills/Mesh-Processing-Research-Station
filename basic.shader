#shader vertex
#version 400 core

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
in vec2 vTexture;
in vec3 vBarycentric;
in vec4 vHighlight;

out vec4 gColor;
out vec3 gNormal;
out vec3 gToLight;
out vec2 gTexture;
out vec3 gToEye;
out vec3 gBarycentric;
out vec4 gHighlight;

uniform mat4 uViewMatrix;
uniform mat4 uTransformMatrix;
uniform mat4 uProjectionMatrix;

uniform vec3 uLightPosition;
uniform vec3 uEyePosition;

void main()
{
	// World position:
	gl_Position = uProjectionMatrix * uViewMatrix * uTransformMatrix * vPosition;

	// Check highlight:
	gColor = vColor;
	if (vHighlight.r > 0)
	{
		gColor = vHighlight;
	}
	gNormal = vNormal;
	gTexture = vTexture;
	gToLight = uLightPosition - vPosition.xyz;
	gToEye = vec3(0., 0., 0.) - gl_Position.xyz;
	gBarycentric = vBarycentric;
	gHighlight = vHighlight;
};

#shader geometry
#version 400 core

layout(triangles) in;
layout(triangle_strip, max_vertices=512) out;

in vec4 gColor[];
in vec3 gNormal[];
in vec3 gToLight[];
in vec2 gTexture[];
in vec3 gToEye[];
in vec3 gBarycentric[];
in vec4 gHighlight[];

out vec4 fColor;
out vec3 fNormal;
out vec3 fToLight;
out vec2 fTexture;
out vec3 fToEye;
out vec3 fBarycentric;
out vec4 fHighlight;
noperspective out vec3 fDis; // From the Nvidia solid wireframe algorithm.


void main()
{
	// Project to normalized device coordinates.
	vec3 p0, p1, p2;
	p0 = (gl_in[0].gl_Position / gl_in[0].gl_Position.w).xyz;
	p1 = (gl_in[1].gl_Position / gl_in[1].gl_Position.w).xyz;
	p2 = (gl_in[2].gl_Position / gl_in[2].gl_Position.w).xyz;

	// Get side lengths of triangle.
	float e0 = length(p1 - p0);
	float e1 = length(p2 - p1);
	float e2 = length(p0 - p2);

	// Semiperimeter: preparing to use Heron's formula.
	float s = 0.5 * (e0 + e1 + e2);
	float sa = s - e0;
	float sb = s - e1;
	float sc = s - e2;
	float heron = sqrt(s * sa * sb * sc);

	// Compute heights using Heron's formula:
	// hN is the base of the triangle.
	float h0 = 2.0 * heron / e0;
	float h1 = 2.0 * heron / e1;
	float h2 = 2.0 * heron / e2;

	vec3 h[3];
	h[0] = vec3(0.0, h1, 0.0);
	h[1] = vec3(0.0, 0.0, h2);
	h[2] = vec3(h0, 0.0, 0.0);

	// Emit the vertex.
	for (int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		fColor = gColor[i];
		fNormal = gNormal[i];
		fToLight = gToLight[i];
		fTexture = gTexture[i];
		fToEye = gToEye[i];
		fBarycentric = gBarycentric[i];
		fHighlight = gHighlight[i];
		fDis = h[i];
		EmitVertex();
	}	
};


#shader fragment
#version 400 core

in vec4 fColor;
in vec3 fNormal;
in vec3 fToLight;
in vec2 fTexture;
in vec3 fToEye;
in vec3 fBarycentric;
in vec4 fHighlight;
in vec3 fDis;

uniform float uAmbient;
uniform float uDiffuse;
uniform float uSpecular;
uniform float uShininess;
uniform vec3 uSpecularColor;

uniform int uWireframe;
const float THICKNESS = 0.0001;

out vec4 color;

float Smoothing(float x)
{
	return (2.0, -2.0 * x * x);
}

void main()
{
	// Lighting:
	vec3 unitToLight = normalize(fToLight);
	vec3 unitToEye = normalize(fToEye);

	// Ambient:
	vec3 ambient = uAmbient * fColor.xyz;

	// Diffuse:
	float d = max(dot(fNormal, unitToLight), 0);
	vec3 diffuse = uDiffuse * d * fColor.xyz;

	// Specular:
	float s = 0;
	if (dot(fNormal, unitToLight) > 0)
	{
		vec3 ref = normalize(reflect(unitToLight, fNormal));
		s = pow(max(dot(unitToEye, ref), 0), uShininess);
	}
	vec3 specular = uSpecular * s * uSpecularColor;

	// Wireframe:
	int WIREFRAME = uWireframe < 1 ? 1 : 0;
	float closestEdge = min(fDis[0], min(fDis[1], fDis[2]));
	float width = fwidth(closestEdge);
	float edge = max(WIREFRAME, smoothstep(THICKNESS, THICKNESS + width, closestEdge));
	vec3 edgeVec = vec3(edge);

	color = vec4(min(edgeVec, vec3(ambient + diffuse + specular)), 1);
};
