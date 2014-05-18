#version 150 core

in Vertex	{
	vec2 texCoord;
	flat vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void)	{	
	gl_FragColor= IN.colour;
	
	//gl_FragColor = vec4(1,1,1,1);
}