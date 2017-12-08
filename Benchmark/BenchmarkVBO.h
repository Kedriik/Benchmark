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
//GENERATE HEIGHTMAP BENCHMARK
class BenchmarkVBO
{
	GLFWwindow *window;
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;
	//Common buffers//
	GLuint VBO;
	
	GLuint PerFrameBuffer;
	GLuint ConstantBuffer;
	GLuint RenderProgram;
	GLuint ParticlesBuffer;
	struct Particle
	{
		vec4 velocity;
		vec4 acceleration;
		float mass = 1;
		float density = 0;
		int index;
		int tab;
	} *particles;

	vector<Particle> initialParticles;
	vector<vec4> particlesPositions;
	////Tests objects////
	GLuint ComputePositionUpdate;
	GLuint ComputeVelocityUpdate;
	GLuint VertexPositionUpdate;
	GLuint VertexVelocityUpdate;
	GLuint IndexesBuffer;
	vector<float> indexes;
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
	int count = 10240;
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
	BenchmarkVBO();
	void draw(GLuint drawMode);
	void initialsPartciles(int mode = 0);
	int init(int width = 800, int height = 600);
	void initBuffers();
	void launchLoop();
	void updateBuffers();
	void updateParticlesComputeShader(double deltaTime);
	void updateParticlesVertexShader(double deltaTime);
	~BenchmarkVBO();
};

