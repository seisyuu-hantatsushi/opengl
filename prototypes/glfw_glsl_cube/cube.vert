void main(void)
{
	gl_Position   = gl_ModelViewMatrix*gl_ProjectionMatrix*gl_Vertex;
	//gl_Position   = gl_Vertex;
	gl_FrontColor = gl_Color;
	//gl_FrontColor = vec4(1.0) - gl_Color;
	//gl_FrontColor = vec4(1.0);
}
