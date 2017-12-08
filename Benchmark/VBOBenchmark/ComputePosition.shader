#version 430 
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
layout( local_size_x = 1, local_size_y = 1024, local_size_z = 1) in;
uniform float deltaTime;
void main(void)
{
    uint  gid=uint(gl_GlobalInvocationID.y);
    VBO[gid]=VBO[gid]+deltaTime*particles[gid].velocity;
    VBO[gid].w=gid;
    
}