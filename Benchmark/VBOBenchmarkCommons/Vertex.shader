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
struct Particle
{
    vec4 velocity;
    vec4 acceleration;
    float mass ;
    float density ;
    int index;
    int tab;
};

layout( std430, binding=0 ) buffer ParticlesBuffer
{
  Particle particles[ ]; 
};

layout( std430, binding=10 ) buffer DebugBuffer
{
 vec4 debug; 
};
out float dense;
uniform int height;
uniform int width;
void main()
{
	gl_Position =ProjectionMatrix*ViewMatrix*vec4(vertexPosition_modelspace.xyz,1);
		float C = 1,
		near = 0.1,
		far = 10000000.0f;
	//	gl_Position.z = (2*log2(max(1e-6,C*gl_Position.w + 1)) / log2(max(1e-6,C*far + 1) )- 1) * gl_Position.w;
	int indexV=int(vertexPosition_modelspace.w);
	dense = particles[indexV].density;
}