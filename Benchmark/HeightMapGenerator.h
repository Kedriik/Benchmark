#define GLM_SWIZZLE
#include "glew-2.0.0\include\GL\glew.h"
#include "glfw-3.2.1.bin.WIN64\include\GLFW\glfw3.h"
#include "glm\glm\gtx\transform.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\quaternion.hpp"
#include "glm\glm\gtx\quaternion.hpp"
#include <iostream>
#include "glm\glm\glm.hpp"
#include <string>
#include <fstream>
#include "LoadShaders\include\LoadShaders.h"
#include <vector>
#include <time.h>
#include <conio.h>
#include <vector>
#include <chrono>
#include "windows.h"
#include "glm\glm\glm.hpp"
#include "camera.h"
using namespace glm;
using namespace std;
class HeightMapGenerator
{
public:
	static float permute(float x) { return floor(mod(((x*34.0) + 1.0)*x, 289.0)); }
	static vec4 fade(vec4 t) { return t*t*t*(t*(t*6.0f - 15.0f) + 10.0f); }
	static float taylorInvSqrt(float r) { return 1.79284291400159 - 0.85373472095314 * r; }
	static vec4 permute(vec4 x) { return mod(((x*34.0f) + 1.0f)*x, 289.0f); }
	static vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159f - 0.85373472095314f * r; }
	static float mod289(float x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
	static vec4 mod289(vec4 x) { return x - floor(x * (1.0f / 289.0f)) * 289.0f; }
	static vec4 perm(vec4 x) { return mod289(((x * 34.0f) + 1.0f) * x); }
	static float rand(float n) { return fract(sin(n) * 43758.5453123); }
	static vec4 grad4(float j, vec4 ip) {
		const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
		vec4 p, s;

		p.xyz = floor(fract(vec3(j) * ip.xyz) * 7.0f) * ip.z - 1.0f;
		float temp = dot(abs(p.xyz()), ones.xyz());
		p.w = 1.5 - dot(abs(p.xyz()), ones.xyz());
		s = vec4(lessThan(p, vec4(0.0)));
		p.xyz = p.xyz + (s.xyz*2.0f - 1.0f) * s.www;

		return p;
	}
	static float snoise(vec4 v) {
		const vec2  C = vec2(0.138196601125010504f,0.309016994374947451f); // (sqrt(5) - 1)/4   F4
								   // First corner
		vec4 i = floor(v + dot(v, C.yyyy()));
		vec4 x0 = v - i + dot(i, C.xxxx());

		// Other corners

		// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
		vec4 i0;

		vec3 isX = step(x0.yzw(), x0.xxx());
		vec3 isYZ = step(x0.zww(), x0.yyz());
		//  i0.x = dot( isX, vec3( 1.0 ) );
		i0.x = isX.x + isX.y + isX.z;
		i0.yzw = 1.0f - isX;

		//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
		i0.y += isYZ.x + isYZ.y;
		i0.zw += 1.0f - isYZ.xy;

		i0.z += isYZ.z;
		i0.w += 1.0 - isYZ.z;

		// i0 now contains the unique values 0,1,2,3 in each channel
		vec4 i3 = clamp(i0, 0.0f, 1.0f);
		vec4 i2 = clamp(i0 - 1.0f, 0.0f, 1.0f);
		vec4 i1 = clamp(i0 - 2.0f, 0.0f, 1.0f);

		//  x0 = x0 - 0.0 + 0.0 * C 
		vec4 x1 = x0 - i1 + 1.0f * C.xxxx;
		vec4 x2 = x0 - i2 + 2.0f * C.xxxx;
		vec4 x3 = x0 - i3 + 3.0f * C.xxxx;
		vec4 x4 = x0 - 1.0f + 4.0f * C.xxxx;

		// Permutations
		i = mod(i, 289.0f);
		float j0 = permute(permute(permute(permute(i.w) + i.z) + i.y) + i.x);
		vec4 j1 = permute(permute(permute(permute(
			i.w + vec4(i1.w, i2.w, i3.w, 1.0))
			+ i.z + vec4(i1.z, i2.z, i3.z, 1.0))
			+ i.y + vec4(i1.y, i2.y, i3.y, 1.0))
			+ i.x + vec4(i1.x, i2.x, i3.x, 1.0));
		// Gradients
		// ( 7*7*6 points uniformly over a cube, mapped onto a 4-octahedron.)
		// 7*7*6 = 294, which is close to the ring size 17*17 = 289.

		vec4 ip = vec4(1.0 / 294.0, 1.0 / 49.0, 1.0 / 7.0, 0.0);

		vec4 p0 = grad4(j0, ip);
		vec4 p1 = grad4(j1.x, ip);
		vec4 p2 = grad4(j1.y, ip);
		vec4 p3 = grad4(j1.z, ip);
		vec4 p4 = grad4(j1.w, ip);

		// Normalise gradients
		vec4 norm = taylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
		p0 *= norm.x;
		p1 *= norm.y;
		p2 *= norm.z;
		p3 *= norm.w;
		p4 *= taylorInvSqrt(dot(p4, p4));
		
		// Mix contributions from the five corners
		
		vec3 m0 = vec3(0.0);// max(0.6 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);
		vec2 m1 = vec2(0.0);// max(0.6 - vec2(dot(x3, x3), dot(x4, x4)), 0.0);
		m0 = m0 * m0;
		m1 = m1 * m1;
		return 49.0f * (dot(m0*m0, vec3(dot(p0, x0), dot(p1, x1), dot(p2, x2)))
			+ dot(m1*m1, vec2(dot(p3, x3), dot(p4, x4))));

	}

	//Classic 4D Perlin Noise
static	float cnoise(vec4 P) {
		vec4 Pi0 = floor(P); // Integer part for indexing
		vec4 Pi1 = Pi0 + 1.0f; // Integer part + 1
		Pi0 = mod(Pi0, 289.0f);
		Pi1 = mod(Pi1, 289.0f);
		vec4 Pf0 = fract(P); // Fractional part for interpolation
		vec4 Pf1 = Pf0 - 1.0f; // Fractional part - 1.0
		vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
		vec4 iy = vec4(Pi0.yy, Pi1.yy);
		vec4 iz0 = vec4(Pi0.zzzz);
		vec4 iz1 = vec4(Pi1.zzzz);
		vec4 iw0 = vec4(Pi0.wwww);
		vec4 iw1 = vec4(Pi1.wwww);

		vec4 ixy = permute(permute(ix) + iy);
		vec4 ixy0 = permute(ixy + iz0);
		vec4 ixy1 = permute(ixy + iz1);
		vec4 ixy00 = permute(ixy0 + iw0);
		vec4 ixy01 = permute(ixy0 + iw1);
		vec4 ixy10 = permute(ixy1 + iw0);
		vec4 ixy11 = permute(ixy1 + iw1);

		vec4 gx00 = ixy00 / 7.0f;
		vec4 gy00 = floor(gx00) / 7.0f;
		vec4 gz00 = floor(gy00) / 6.0f;
		gx00 = fract(gx00) - 0.5f;
		gy00 = fract(gy00) - 0.5f;
		gz00 = fract(gz00) - 0.5f;
		vec4 gw00 = vec4(0.75) - abs(gx00) - abs(gy00) - abs(gz00);
		vec4 sw00 = step(gw00, vec4(0.0));
		gx00 -= sw00 * (step(0.0f, gx00) - 0.5f);
		gy00 -= sw00 * (step(0.0f, gy00) - 0.5f);

		vec4 gx01 = ixy01 / 7.0f;
		vec4 gy01 = floor(gx01) / 7.0f;
		vec4 gz01 = floor(gy01) / 6.0f;
		gx01 = fract(gx01) - 0.5f;
		gy01 = fract(gy01) - 0.5f;
		gz01 = fract(gz01) - 0.5f;
		vec4 gw01 = vec4(0.75) - abs(gx01) - abs(gy01) - abs(gz01);
		vec4 sw01 = step(gw01, vec4(0.0));
		gx01 -= sw01 * (step(0.0f, gx01) - 0.5f);
		gy01 -= sw01 * (step(0.0f, gy01) - 0.5f);

		vec4 gx10 = ixy10 / 7.0f;
		vec4 gy10 = floor(gx10) / 7.0f;
		vec4 gz10 = floor(gy10) / 6.0f;
		gx10 = fract(gx10) - 0.5f;
		gy10 = fract(gy10) - 0.5f;
		gz10 = fract(gz10) - 0.5f;
		vec4 gw10 = vec4(0.75) - abs(gx10) - abs(gy10) - abs(gz10);
		vec4 sw10 = step(gw10, vec4(0.0));
		gx10 -= sw10 * (step(0.0f, gx10) - 0.5f);
		gy10 -= sw10 * (step(0.0f, gy10) - 0.5f);

		vec4 gx11 = ixy11 / 7.0f;
		vec4 gy11 = floor(gx11) / 7.0f;
		vec4 gz11 = floor(gy11) / 6.0f;
		gx11 = fract(gx11) - 0.5f;
		gy11 = fract(gy11) - 0.5f;
		gz11 = fract(gz11) - 0.5f;
		vec4 gw11 = vec4(0.75) - abs(gx11) - abs(gy11) - abs(gz11);
		vec4 sw11 = step(gw11, vec4(0.0));
		gx11 -= sw11 * (step(0.0f, gx11) - 0.5f);
		gy11 -= sw11 * (step(0.0f, gy11) - 0.5f);

		vec4 g0000 = vec4(gx00.x, gy00.x, gz00.x, gw00.x);
		vec4 g1000 = vec4(gx00.y, gy00.y, gz00.y, gw00.y);
		vec4 g0100 = vec4(gx00.z, gy00.z, gz00.z, gw00.z);
		vec4 g1100 = vec4(gx00.w, gy00.w, gz00.w, gw00.w);
		vec4 g0010 = vec4(gx10.x, gy10.x, gz10.x, gw10.x);
		vec4 g1010 = vec4(gx10.y, gy10.y, gz10.y, gw10.y);
		vec4 g0110 = vec4(gx10.z, gy10.z, gz10.z, gw10.z);
		vec4 g1110 = vec4(gx10.w, gy10.w, gz10.w, gw10.w);
		vec4 g0001 = vec4(gx01.x, gy01.x, gz01.x, gw01.x);
		vec4 g1001 = vec4(gx01.y, gy01.y, gz01.y, gw01.y);
		vec4 g0101 = vec4(gx01.z, gy01.z, gz01.z, gw01.z);
		vec4 g1101 = vec4(gx01.w, gy01.w, gz01.w, gw01.w);
		vec4 g0011 = vec4(gx11.x, gy11.x, gz11.x, gw11.x);
		vec4 g1011 = vec4(gx11.y, gy11.y, gz11.y, gw11.y);
		vec4 g0111 = vec4(gx11.z, gy11.z, gz11.z, gw11.z);
		vec4 g1111 = vec4(gx11.w, gy11.w, gz11.w, gw11.w);

		vec4 norm00 = taylorInvSqrt(vec4(dot(g0000, g0000), dot(g0100, g0100), dot(g1000, g1000), dot(g1100, g1100)));
		g0000 *= norm00.x;
		g0100 *= norm00.y;
		g1000 *= norm00.z;
		g1100 *= norm00.w;

		vec4 norm01 = taylorInvSqrt(vec4(dot(g0001, g0001), dot(g0101, g0101), dot(g1001, g1001), dot(g1101, g1101)));
		g0001 *= norm01.x;
		g0101 *= norm01.y;
		g1001 *= norm01.z;
		g1101 *= norm01.w;

		vec4 norm10 = taylorInvSqrt(vec4(dot(g0010, g0010), dot(g0110, g0110), dot(g1010, g1010), dot(g1110, g1110)));
		g0010 *= norm10.x;
		g0110 *= norm10.y;
		g1010 *= norm10.z;
		g1110 *= norm10.w;

		vec4 norm11 = taylorInvSqrt(vec4(dot(g0011, g0011), dot(g0111, g0111), dot(g1011, g1011), dot(g1111, g1111)));
		g0011 *= norm11.x;
		g0111 *= norm11.y;
		g1011 *= norm11.z;
		g1111 *= norm11.w;

		float n0000 = dot(g0000, Pf0);
		float n1000 = dot(g1000, vec4(Pf1.x, Pf0.yzw));
		float n0100 = dot(g0100, vec4(Pf0.x, Pf1.y, Pf0.zw));
		float n1100 = dot(g1100, vec4(Pf1.xy, Pf0.zw));
		float n0010 = dot(g0010, vec4(Pf0.xy, Pf1.z, Pf0.w));
		float n1010 = dot(g1010, vec4(Pf1.x, Pf0.y, Pf1.z, Pf0.w));
		float n0110 = dot(g0110, vec4(Pf0.x, Pf1.yz, Pf0.w));
		float n1110 = dot(g1110, vec4(Pf1.xyz, Pf0.w));
		float n0001 = dot(g0001, vec4(Pf0.xyz, Pf1.w));
		float n1001 = dot(g1001, vec4(Pf1.x, Pf0.yz, Pf1.w));
		float n0101 = dot(g0101, vec4(Pf0.x, Pf1.y, Pf0.z, Pf1.w));
		float n1101 = dot(g1101, vec4(Pf1.xy, Pf0.z, Pf1.w));
		float n0011 = dot(g0011, vec4(Pf0.xy, Pf1.zw));
		float n1011 = dot(g1011, vec4(Pf1.x, Pf0.y, Pf1.zw));
		float n0111 = dot(g0111, vec4(Pf0.x, Pf1.yzw));
		float n1111 = dot(g1111, Pf1);
		vec4 fade_xyzw = fade(Pf0);
		vec4 n_0w = mix(vec4(n0000, n1000, n0100, n1100), vec4(n0001, n1001, n0101, n1101), fade_xyzw.w);
		vec4 n_1w = mix(vec4(n0010, n1010, n0110, n1110), vec4(n0011, n1011, n0111, n1111), fade_xyzw.w);
		vec4 n_zw = mix(n_0w, n_1w, fade_xyzw.z);
		vec2 n_yzw = mix(n_zw.xy(), n_zw.zw(), fade_xyzw.y);
		float n_xyzw = mix(n_yzw.x, n_yzw.y, fade_xyzw.x);
		return 2.2 * n_xyzw;
	}
	static void generateHeightMap(vector<vec4> *heightMap, int heightMapSize, double time)
	{
		for (int i = 0; i < heightMapSize; i++)
		{
			for (int j = 0; j < heightMapSize; j++)
			{

				int  WGidY = i;
				int  WGidX = j;
				float X = float((WGidX - (heightMapSize - 1) / 2.0f) / float(heightMapSize - 1));
				float Z = float((WGidY - (heightMapSize - 1) / 2.0f) / float(heightMapSize - 1));
				vec4 vectorToStore = vec4(X, 0, Z, 1);
				float f = 2;
				float A = 0.1;
				float h = 0;
				int octaves = 10;
				for (int i = 0; i<octaves; i++)
				{
					h += A*cnoise(vec4(f*vectorToStore.xyz, time));
					A /= 2.0;
					f *= 2.0;
				}
	//			h = 0;
				vectorToStore.y = h;
				heightMap->push_back(vectorToStore);
			}
		}
	}
};
