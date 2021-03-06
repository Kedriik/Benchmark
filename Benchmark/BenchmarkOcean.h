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
//#include "HeightMapGenerator.h"
using namespace glm;
using namespace std;
//GENERATE HEIGHTMAP BENCHMARK
class BenchmarkOcean
{
	GLFWwindow *window;
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;
	//Common buffers//
	GLuint VBO;
	vector<vec4> vertices;
	GLuint HeightMap;
	GLuint PerFrameBuffer;
	GLuint ConstantBuffer;
	GLuint ElementBuffer;
	GLuint UpdatingRenderProgram;
	GLuint RenderProgram;
	GLuint Polygonizator;
	GLuint VertexPolygonizator;
	GLuint FlatShader;
	////Tests objects////
	GLuint ComputeShader;
	GLuint VertexShader;
	GLuint heightMapIndexBuffer;
	vector<vec2> heightMapIndex;
	////////////////////
	GLuint VertexProgram, IndexBuffer;

	struct perFrameData
	{
		mat4 ViewMatrix;
		mat4 ModelMatrix = mat4(1.0f);
		float time;
		vec3 seed;
	} *perFrameData;

	struct constantData
	{
		mat4 ProjectionMatrix;
	} *constantData;
	double loopTotalTime = 0;
	int heightMapSize = 100;
	int octaves = 10;
	double frames = 0;
	double testTime = 35;
	Camera camera;
	enum Test
	{
		VertexTest,
		ComputeTest,
		CPUTest
	};
#define Compute Test::ComputeTest
#define Vertex Test::VertexTest
#define CPU Test::CPUTest
	Test test = Compute;
public:
	BenchmarkOcean();
	void drawAndUpdate(GLuint drawMode);
	void draw(GLuint drawMode);
	int init(int width = 800, int height = 600, int test=0, int heightMapSize=100);
	void initBuffers();
	double launchLoop();
	void setOctaves(int _octaves)
	{
		octaves = _octaves;
	};
	void updateBuffers();
	void generateHeightmapComputeShader();
	void generateHeightmapVertexShader();
	void generateFlat();
	void generateHeigtmapCpu();
	void polygonise();
	void polygoniseVertex();
	~BenchmarkOcean();

};

