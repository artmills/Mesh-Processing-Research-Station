#shader vertex
#version 400 core

in vec4 vPosition;
in vec3 vNormal;

out vec4 fColor;

uniform mat4 uViewMatrix;
uniform mat4 uTransformMatrix;
uniform mat4 uProjectionMatrix;

uniform float uTranslate;

void main()
{
	// Translate the vertex along its normal vector by number of units given by uTranslate.
	vec3 extruded = vPosition.xyz + vNormal * uTranslate;

	// Set world position.
	gl_Position = uProjectionMatrix * uViewMatrix * uTransformMatrix * vec4(extruded, vPosition.w);

	// Set color to be black.
	fColor = vec4(0.0, 0.0, 0.0, 1.0);
};


#shader fragment
#version 400 core

in vec4 fColor;

out vec4 color;


void main()
{
	color = fColor;
};
