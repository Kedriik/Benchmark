
#include "Benchmark.h"


Benchmark::Benchmark()
{
}

void Benchmark::draw()
{
	glUseProgram(RenderProgram);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, PerFrameBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ConstantBuffer);
	//
	//glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	//glVertexAttribPointer(
	//	2,                  // attribute. No particular reason for 2, but must match the layout in the shader.
	//	4,                  // size
	//	GL_FLOAT,           // type
	//	GL_FALSE,           // normalized?
	//	0,                  // stride
	//	(void*)0            // array buffer offset
	//);
	//Draw array
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(
		3,                  // attribute. No particular reason for 3, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
}


Benchmark::~Benchmark()
{
}

int Benchmark::initWindow(int width , int height)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	std::cout << "GLFW init completed" << std::endl;



	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//	glfwWindowHint(GL_CONTEXT_FLAGS,GL_CONTEXT_FLAG_DEBUG_BIT);



	window = glfwCreateWindow(width, height, "Kedrowsky&Partnerzy Solutions", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	std::cout << "GLEW init completed" << std::endl;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	ProjectionMatrix = glm::infinitePerspective(45.0f, float(width) / float(height), 0.01f);

	return 1;
}

void Benchmark::initBuffers()
{
	ShaderInfo  RenderProgramSource[] = {
		{ GL_VERTEX_SHADER,  "Common\\Vertex.shader" },
		{ GL_FRAGMENT_SHADER,"Common\\Fragment.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	RenderProgram = LoadShaders(RenderProgramSource);

	ShaderInfo  PoligonizatorSource[] = {
		{ GL_COMPUTE_SHADER, "Common\\Polygonizator.txt" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	Polygonizator = LoadShaders(PoligonizatorSource);

	glGenBuffers(1, &PerFrameBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, PerFrameBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct perFrameData), NULL, GL_DYNAMIC_COPY);
	perFrameData = (struct perFrameData *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(struct perFrameData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glGenBuffers(1, &ConstantBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ConstantBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct constantData), NULL, GL_DYNAMIC_COPY);
	constantData = (struct constantData *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(struct constantData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	constantData->ProjectionMatrix = ProjectionMatrix;
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	int indexSize=6 * (heightMapSize - 1)*(heightMapSize - 1);
	int size = heightMapSize*heightMapSize;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(vec4), NULL, GL_DYNAMIC_COPY);

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);

	glGenTextures(1, &HeightMap);
	glBindTexture(GL_TEXTURE_2D, HeightMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, heightMapSize, heightMapSize, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

}

void Benchmark::launchLoop()
{

	initWindow();
	 loopTotalTime = 0;
	double deltaTime = 0;
	double stallTime = 1.0;
	camera.setPosition(vec3(0, 10, 0));
	camera.setUp(vec3(0, 1, 0));
	camera.setForward(vec3(1, 0, 0));
	ViewMatrix = camera.cameraPositionKeyboard(0);
	do
	{

		static double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();
		deltaTime = double(currentTime - lastTime);
		if (deltaTime > stallTime) deltaTime = 0;
		loopTotalTime += deltaTime;
		///////////////
		lastTime = currentTime;
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}

void Benchmark::updateBuffers()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, PerFrameBuffer);
	perFrameData = (struct perFrameData *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(struct perFrameData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	perFrameData->ViewMatrix = ViewMatrix;
	perFrameData->time = loopTotalTime;
	perFrameData->seed = vec3(0.0f);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
