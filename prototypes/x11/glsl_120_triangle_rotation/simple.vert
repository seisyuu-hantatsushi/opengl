#version 120

void main() {
	 // ftransform() = gl_ModelViewProjection * gl_Vertex
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}
