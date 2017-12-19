#version 430 
layout( std430, binding=10 ) buffer DebugBuffer
{
 vec4 debug; 
};
layout (rgba32f)  uniform image2D denseTexture;
layout (rgba32f)  uniform image2D outputDenseTexture;
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
	uint X=gl_WorkGroupID.x;
	uint Y=gl_WorkGroupID.y;
float denseFilter()
{
	ivec2 UV=ivec2(X,Y);
	ivec2 size = imageSize(denseTexture);
	if(X<5 || Y < 5 || X> size.x-5 || Y>size.y-5)
		return 0;

	float _dense=0;
	for(int i=-3;i<4;i++)
	{
		for(int j=-3;j<4;j++)
		{
			vec2 v=vec2(i,j);
			float dist=length(v);
			float factor=5;
			_dense=_dense+imageLoad(denseTexture, ivec2(UV+ivec2(i,j))).y*(factor-dist);
		}
	}
	return clamp(_dense, 0,100)/100.0;
}

void main()
{
	ivec2 UV=ivec2(X,Y);
	float currentDense=imageLoad(denseTexture, UV).y;
	float dense=denseFilter();
//	debug.x=dense;
	imageStore(outputDenseTexture, UV, vec4(dense,dense,dense,1));
}