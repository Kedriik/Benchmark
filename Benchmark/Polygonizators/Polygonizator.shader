#version 430 //core

layout( std430, binding=1 ) buffer VertBuffer
{
  vec4 VBO[ ]; 
};

layout(std430, binding = 2) buffer IndexBuffer
{
  uint Index[];
};

layout (rgba32f)  uniform image2D HeightMap;
layout( local_size_x = 10, local_size_y = 10, local_size_z = 1) in;
uniform int heightMapSize;

void main(void)
{
    uint  textureSize=uint(heightMapSize);
    int  WGidY=int(gl_GlobalInvocationID.y);
    int  WGidX=int(gl_GlobalInvocationID.x);
    uint mapVBOOffset=0;//index*textureSize*textureSize;
    uint indexOffset=0;//6*index*textureSize*textureSize;
        VBO[WGidY*textureSize+WGidX+mapVBOOffset]=imageLoad(HeightMap, ivec2(WGidX, WGidY));
//    Normals[WGidY*textureSize+WGidX+mapVBOOffset]=imageLoad(NormalMap, ivec2(WGidX, WGidY));
//         UV[WGidY*textureSize+WGidX+mapVBOOffset]=vec4(WGidX, WGidY,size,1);



    if(WGidX==textureSize-1 || WGidY==textureSize-1)
    return;
   
    uint localIndex = uint(6*(WGidY*textureSize+WGidX)+indexOffset);
    Index[localIndex+0]=uint((WGidY+1)*textureSize+WGidX  +mapVBOOffset);
    Index[localIndex+1]=WGidY*textureSize    +WGidX+1+mapVBOOffset;
    Index[localIndex+2]=WGidY*textureSize    +WGidX  +mapVBOOffset;
    Index[localIndex+3]=WGidY*textureSize    +WGidX+1+mapVBOOffset;
    Index[localIndex+4]=(WGidY+1)*textureSize+WGidX  +mapVBOOffset;
    Index[localIndex+5]=(WGidY+1)*textureSize+WGidX+1+mapVBOOffset;
    
  }