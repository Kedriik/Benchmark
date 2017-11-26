#define GLM_SWIZZLE
//#pragma once
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
	////Tests objects////
	GLuint ComputeShader;
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
	int heightMapSize = 100;
	Camera camera;
public:
	Benchmark();
	void draw();
	int initWindow(int width = 800, int height = 600);
	void initBuffers();
	void launchLoop();
	void updateBuffers();
	~Benchmark();

};

