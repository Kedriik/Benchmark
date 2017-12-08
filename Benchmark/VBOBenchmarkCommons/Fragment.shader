#version 430

out vec4 color;

in float dense;
void main()
{


	float blend=200.0f;
	//color=vec4(0.5f+0.5f*clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,blend)/blend);
	color=vec4(1,clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,1);
}