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
#include "HeightMapGenerator.h"
using namespace glm;
using namespace std;
//GENERATE HEIGHTMAP BENCHMARK
class Benchmark 
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
	GLuint RenderProgram;
	GLuint Polygonizator;
	GLuint VertexPolygonizator;
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
		float time;
		vec3 seed;
	} *perFrameData;

	struct constantData
	{
		mat4 ProjectionMatrix;
	} *constantData;
	double loopTotalTime = 0;
	int heightMapSize = 500;
	int octaves = 64;
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
	Test test = Vertex;
public:
	Benchmark();
	void draw(GLuint drawMode);
	int init(int width = 800, int height = 600);
	void initBuffers();
	void launchLoop();
	void updateBuffers();
	void generateHeightmapComputeShader();
	void generateHeightmapVertexShader();
	void generateHeigtmapCpu();
	void polygonise();
	void polygoniseVertex();
	~Benchmark();

};

