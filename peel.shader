#shader vertex
#version 400 core

in vec4 vPosition;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uTransformMatrix;

void main()
{
	gl_Position = uProjectionMatrix * uViewMatrix * uTransformMatrix * vPosition;
};

#shader fragment
#version 400 core

uniform sampler2D uTexture;

uniform int uWidth;
uniform int uHeight;
uniform float uNear;
uniform float uFar;

const float bias = 0.01;

out vec4 color;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));
}

void main()
{
	float current = LinearizeDepth(gl_FragCoord.z);
	float back = LinearizeDepth(texture(uTexture, gl_FragCoord.xy / vec2(uWidth, uHeight)).r);

	// Depth peeling:
	if (current - bias <= back)
	{
		discard;
	}
	else
	{
		color = vec4(1, 0, 0, 1);
	}
};
