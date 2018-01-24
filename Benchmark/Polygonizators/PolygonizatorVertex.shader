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
uniform int heightMapSize;
layout(location = 0) in vec2 heightMapIndex;
void main(void)
{
  uint  textureSize=uint(heightMapSize);
  int  WGidY=int(heightMapIndex.y);
  int  WGidX=int(heightMapIndex.x);
  uint mapVBOOffset=0;//index*textureSize*textureSize;
  uint indexOffset=0;//6*index*textureSize*textureSize;
            VBO[WGidY*textureSize+WGidX+mapVBOOffset]=imageLoad(HeightMap, ivec2(WGidX, WGidY));
  //    Normals[WGidY*textureSize+WGidX+mapVBOOffset]=imageLoad(NormalMap, ivec2(WGidX, WGidY));
  //         UV[WGidY*textureSize+WGidX+mapVBOOffset]=vec4(WGidX, WGidY,size,1);



  if(WGidX==textureSize-1 || WGidY==textureSize-1)
  return;
 
  uint localIndex = 6*(WGidY*textureSize+WGidX)+indexOffset;
  Index[localIndex+0]=uint((WGidY+1)*textureSize+WGidX  +mapVBOOffset);
  Index[localIndex+1]=WGidY*textureSize    +WGidX+1+mapVBOOffset;
  Index[localIndex+2]=WGidY*textureSize    +WGidX  +mapVBOOffset;
  Index[localIndex+3]=WGidY*textureSize    +WGidX+1+mapVBOOffset;
  Index[localIndex+4]=(WGidY+1)*textureSize+WGidX  +mapVBOOffset;
  Index[localIndex+5]=(WGidY+1)*textureSize+WGidX+1+mapVBOOffset;
    
  }