#version 150

in vec3 position;
in vec4 color;

uniform mat4 mvp;

smooth out vec4 vertexColor;

void main(void)
{
	gl_Position  = mvp * vec4(position, 1.0);
	vertexColor  = color;

}
