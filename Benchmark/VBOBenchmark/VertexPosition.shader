#version 430
layout(location = 3) in float m_InvocationID;

struct Particle
{
    vec4 velocity;
    vec4 acceleration;
    float mass ;
    float density;
    int index;
    int tab;
};

layout( std430, binding=0 ) buffer ParticlesBuffer
{
  Particle particles[ ]; 
};

layout( std430, binding=1 ) buffer VertBuffer
{
  vec4 VBO[ ]; 
};

layout(std430, binding = 4) buffer PerFrame
{
  mat4 ViewMatrix;
  float time;
  vec3 seed;
};
uniform float deltaTime;
void main(void)
{
    uint  gid=uint(m_InvocationID);
    VBO[gid]=VBO[gid]+deltaTime*particles[gid].velocity;
    VBO[gid].w=gid;
    
}