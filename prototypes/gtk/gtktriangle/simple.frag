#version 130

// simple.frag

smooth in vec4 vertexColor;

out vec4 outputColor;

void main (void)
{
  // フラグメントの色
	outputColor = vertexColor;
}
