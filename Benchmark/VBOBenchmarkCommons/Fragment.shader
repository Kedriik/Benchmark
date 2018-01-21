#version 430

layout( std430, binding=10 ) buffer DebugBuffer
{
 vec4 debug; 
};
layout (location = 0) out vec4 color;
layout (rgba32f)  uniform image2D denseTexture;

uniform int height;
uniform int width;
in float dense;
void main()
{


	float blend=200.0f;
	//color=vec4(0.5f+0.5f*clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,blend)/blend);
	
	//color=vec4(1,clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,1);
	//color=vec4(clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,clamp(dense, 0,255.0f)/255.0f,1);
	color=vec4(1.0);
	vec2 UV=gl_FragCoord.xy;
	float currentDense=imageLoad(denseTexture, ivec2(UV)).y;
	currentDense+=1;
	imageStore(denseTexture, ivec2(UV), vec4(0,currentDense,0,1));

}