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
	GLuint DefaultFrameBuffer;
	GLuint FinalFrameBuffer;
	GLuint ColorTexture;
	vector<GLuint> denseTextures;
	int denseTexture = 0;
	vector<vec4> emptyTexture;
	GLuint DenseShader;
	GLuint FillingShader;
	GLuint RenderShaders;
	GLuint quad_vertexbuffer;
	GLuint UV;
	int width, height;
	//Common buffers//
	GLuint VBO;
	GLuint DebugBuffer;
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
	int count;
	int postEffect;
	bool gpuFilling = false;
	double testTime = 65;
	double frames = 0;
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
	BenchmarkVBO(int _count = 5120, int test = 0, int postEffect = 0);
	void draw(GLuint drawMode);
	void drawRendererOutputs();
	void densePostEffect();
	void initialsPartciles(int mode = 0);
	int init(int width = 800, int height = 600);
	void initBuffers();
	double launchLoop();
	void updateBuffers();
	void updateParticlesComputeShader(double deltaTime);
	void updateParticlesVertexShader(double deltaTime);
	void gpuDebug(string Nazwa = "bez nazwy")
	{
		float debug[4];
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, DebugBuffer);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * sizeof(float), &debug);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		cout << Nazwa << endl;
		cout << "\r debug.x:" << debug[0] << endl;
		cout << "\r debug.y:" << debug[1] << endl;
		cout << "\r debug.z:" << debug[2] << endl;
		cout << "\r debug.w:" << debug[3] << endl;


	}
	~BenchmarkVBO();
};

