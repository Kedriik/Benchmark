
#include "Benchmark.h"


Benchmark::Benchmark()
{
}

void Benchmark::draw(GLuint drawMode)
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
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 3, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

	glDrawElements(GL_TRIANGLES, 6*(heightMapSize)*(heightMapSize), GL_UNSIGNED_INT, (void*)0);
	glDisableVertexAttribArray(0);
}

void Benchmark::polygonise()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, IndexBuffer);


	glBindImageTexture(0, HeightMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glUseProgram(Polygonizator);
	glUniform1i(glGetUniformLocation(Polygonizator, "HeightMap"), 0);
	glUniform1i(glGetUniformLocation(Polygonizator, "heightMapSize"), heightMapSize);
	glDispatchCompute((heightMapSize/10), (heightMapSize/10), 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Benchmark::polygoniseVertex()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, IndexBuffer);


	glBindImageTexture(0, HeightMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glUseProgram(VertexPolygonizator);
	glUniform1i(glGetUniformLocation(VertexPolygonizator, "HeightMap"), 0);
	glUniform1i(glGetUniformLocation(VertexPolygonizator, "heightMapSize"), heightMapSize);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, heightMapIndexBuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 3, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_POINTS, 0, heightMapSize*heightMapSize);
	glDisableVertexAttribArray(0);
}

Benchmark::~Benchmark()
{
	
	GLuint buffersToDelete[11] = {
	 VBO,
	 HeightMap,
	 PerFrameBuffer,
	 ConstantBuffer,
	 ElementBuffer,
	 RenderProgram,
	 Polygonizator,
	 VertexPolygonizator,
	 ComputeShader,
	 VertexShader,
	 heightMapIndexBuffer };
	glDeleteBuffers(11, buffersToDelete);
}

int Benchmark::init(int width , int height, int _test, int _heightMapSize)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	//std::cout << "GLFW init completed" << std::endl;



	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//	glfwWindowHint(GL_CONTEXT_FLAGS,GL_CONTEXT_FLAG_DEBUG_BIT);



	window = glfwCreateWindow(width, height, "Benchmarks", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
//	std::cout << "GLEW init completed" << std::endl;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	ProjectionMatrix = glm::infinitePerspective(45.0f, float(width) / float(height), 0.01f);


	vec3 camPos = vec3(0, 15.1211f, 0); vec3(51.2, 250, 51.2);
	camera = Camera(window, width, height, camPos, vec3(0, -1, 0.0), vec3(1, 0, 0), 0.01);
	camera.setSens(0.01f, 0.1f);

	glEnable(GL_DEPTH_TEST);
	if (_test == 0)
		test = Compute;
	if (_test == 1)
		test = Vertex;
	if (_test == 2)
		test = CPU;

	heightMapSize = _heightMapSize;
	return 1;
}

void Benchmark::initBuffers()
{
	ShaderInfo  RenderProgramSource[] = {
		{ GL_VERTEX_SHADER,  "HeightMapCommons\\Vertex.shader" },
		{ GL_FRAGMENT_SHADER,"HeightMapCommons\\Fragment.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	RenderProgram = LoadShaders(RenderProgramSource);

	ShaderInfo  PoligonizatorSource[] = {
		{ GL_COMPUTE_SHADER, "Polygonizators\\Polygonizator.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	Polygonizator = LoadShaders(PoligonizatorSource);

	ShaderInfo  ComputeSource[] = {
		{ GL_COMPUTE_SHADER, "HeightMapGenerators\\Compute.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	ComputeShader = LoadShaders(ComputeSource);


	ShaderInfo  VertexProgramSource[] = {
		{ GL_VERTEX_SHADER,  "HeightMapGenerators\\Vertex.shader" },
		{ GL_FRAGMENT_SHADER,"HeightMapGenerators\\PassThroughFragment.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	VertexShader = LoadShaders(VertexProgramSource);

	ShaderInfo  VertexPolygonizatorSource[] = {
		{ GL_VERTEX_SHADER,  "Polygonizators\\PolygonizatorVertex.shader" },
		{ GL_FRAGMENT_SHADER,"Polygonizators\\PassThroughFragment.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	VertexPolygonizator = LoadShaders(VertexPolygonizatorSource);
	
	for (int i = 0; i < heightMapSize; i++)
	{
		for (int j = 0; j < heightMapSize; j++)
		{
			heightMapIndex.push_back(vec2(i,j));
		}
	}

	glGenBuffers(1, &heightMapIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, heightMapIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, heightMapSize*heightMapSize * sizeof(vec2), heightMapIndex.data(), GL_DYNAMIC_COPY);

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

	int indexSize=7*(heightMapSize )*(heightMapSize );
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

	glGenerateMipmap(GL_TEXTURE_2D);

	

}

double Benchmark::launchLoop()
{
	loopTotalTime = 0;
	double deltaTime = 0;
	double stallTime = 1.0;
	camera.setPosition(vec3(0, 1, 0));
	camera.setUp(vec3(1, 0, 0));
	camera.setForward(vec3(0, -1, 0));
	ViewMatrix = camera.cameraPositionKeyboard(0);
	vector<vec4> hp;
	GLuint drawMode = GL_FILL;
	do
	{
		static double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();
		deltaTime = double(currentTime - lastTime);
	//	if (deltaTime > stallTime) deltaTime = 0;
		loopTotalTime += deltaTime;
		///////////////
		ViewMatrix = camera.cameraPositionKeyboard(deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		updateBuffers();
		drawMode = GL_FILL;
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		{
			drawMode = GL_LINE;
		}
		glPolygonMode(GL_FRONT_AND_BACK, drawMode);
		if (test == Test::VertexTest)
		{
			generateHeightmapVertexShader();
			polygoniseVertex();
		}
		if (test == Test::ComputeTest)
		{
			generateHeightmapComputeShader();
			polygonise();
		}
		if (test == Test::CPUTest)
		{

		}
		
		draw(drawMode);
		
		///////////////
		lastTime = currentTime;
		glfwSwapBuffers(window);
		glfwPollEvents();
		frames++;

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0 && loopTotalTime<testTime);
	glfwDestroyWindow(window);
	double result= frames / loopTotalTime;
	loopTotalTime = 0;
	frames = 0;
	return result;
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

void Benchmark::generateHeightmapComputeShader()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, PerFrameBuffer);
	glBindImageTexture(0, HeightMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glUseProgram(ComputeShader);
	glUniform1i(glGetUniformLocation(ComputeShader, "HeightMap"), 0);
	glUniform1i(glGetUniformLocation(ComputeShader, "heightMapSize"), heightMapSize);
	glUniform1i(glGetUniformLocation(ComputeShader, "octaves"), octaves);
	glDispatchCompute(heightMapSize/10, heightMapSize/10, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Benchmark::generateHeightmapVertexShader()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, PerFrameBuffer);
	glBindImageTexture(0, HeightMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glUseProgram(VertexShader);
	glUniform1i(glGetUniformLocation(VertexShader, "HeightMap"), 0);
	glUniform1i(glGetUniformLocation(VertexShader, "heightMapSize"), heightMapSize);
	glUniform1i(glGetUniformLocation(VertexShader, "octaves"), octaves);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, heightMapIndexBuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 3, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_POINTS, 0, heightMapSize*heightMapSize);
	glDisableVertexAttribArray(0);

}
