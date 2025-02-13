#shader vertex
#version 400 core

in vec4 vPosition;
in vec4 vColor;

out vec4 fColor;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uTransformMatrix;

void main()
{
	// World position:
	gl_Position = uProjectionMatrix * uViewMatrix * uTransformMatrix * vPosition;

	fColor = vColor;
};

#shader fragment
#version 400 core

in vec4 fColor;
out vec4 color;

void main()
{
	color = fColor;
};
