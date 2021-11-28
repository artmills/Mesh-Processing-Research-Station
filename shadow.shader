#shader vertex
#version 400 core

in vec4 vPosition;

uniform mat4 uLightPerspectiveMatrix;
uniform mat4 uLightViewMatrix;
uniform mat4 uTransformMatrix;

void main()
{
	gl_Position = uLightPerspectiveMatrix * uLightViewMatrix * uTransformMatrix * vPosition;
};

#shader fragment
#version 400 core

out vec4 color;

void main()
{
	color = vec4(1.0);
};
