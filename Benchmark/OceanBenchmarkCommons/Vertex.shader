#version 430 
layout(location = 0) in vec4 vertexPosition_modelspace;

layout(std430, binding = 4) buffer PerFrame
{
	mat4 ViewMatrix;
	float time;
	vec3 seed;
};

layout(std430, binding = 5) buffer Constant
{
	mat4 ProjectionMatrix;
};
out float h;
void main()
{
	vec4 position=vec4(vertexPosition_modelspace.xyz,1);
	h=position.y;
	gl_Position =ProjectionMatrix*ViewMatrix*position;
}