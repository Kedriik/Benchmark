#version 430

out vec4 color;
in float h;
void main()
{
	float colorFactor=10.0*h;
	color=vec4((1.0+colorFactor)/2.0,1.0-(1.0+colorFactor)/2.0,0,1);
}