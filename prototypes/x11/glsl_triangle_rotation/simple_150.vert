#version 150
uniform mat4 ModelViewProjection;
in vec4 Vertex;
in vec3 inColor;
out vec3 outColor;

void main(void)
{
	outColor = inColor;
	gl_Position = ModelViewProjection * Vertex;
}