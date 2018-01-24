#version 430 

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
uniform int count;
uniform float deltaTime;
void main(void)
{
    uint  gid=uint(gl_GlobalInvocationID.y);
    float G=0.1;
    float density_r=1;;
    particles[gid].density=0;
    vec4 a=vec4(0.0);
    for(int i=0;i<count;i++)
    {
      if(gid!=i)
      {
          vec4 vector=VBO[i]-VBO[gid];
          vec4 dir=normalize(vector);
          float r=length(vector);
          particles[gid].density+=200.0f*particles[i].mass/(r*r);
          if(r!=0)
            a+=G*particles[i].mass*dir/(r*r);
      }
     
    }
    particles[gid].velocity+=a*deltaTime;
}