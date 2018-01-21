
#include "BenchmarkVBO.h"


BenchmarkVBO::BenchmarkVBO(int _count, int _test, int _postEffect)
{
	count = _count;
	if (test == 0) test = Test::ComputeTest;
	if (test == 1) test = Test::VertexTest;

	if (_postEffect) postEffect = _postEffect;
	
}
BenchmarkVBO::~BenchmarkVBO()
{
}
int BenchmarkVBO::init(int _width, int _height)
{
	width = _width;
	height = _height;
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



	window = glfwCreateWindow(width, height, "Benchmarks", NULL, NULL);
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


	vec3 camPos = vec3(0, 15.1211f, 0); vec3(51.2, 250, 51.2);
	camera = Camera(window, width, height, camPos, vec3(0, -1, 0.0), vec3(1, 0, 0), 0.01);
	camera.setSens(0.01f, 10.1f);

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	return 1;
}
void BenchmarkVBO::initBuffers()
{
	ShaderInfo  RenderProgramSource[] = {
		{ GL_VERTEX_SHADER,  "VBOBenchmarkCommons\\Vertex.shader" },
		{ GL_FRAGMENT_SHADER,"VBOBenchmarkCommons\\Fragment.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	RenderProgram = LoadShaders(RenderProgramSource);

	ShaderInfo  ComputePSource[] = {
		{ GL_COMPUTE_SHADER, "VBOBenchmark\\ComputePosition.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	ComputePositionUpdate = LoadShaders(ComputePSource);

	ShaderInfo  ComputeVSource[] = {
		{ GL_COMPUTE_SHADER, "VBOBenchmark\\ComputeVelocity.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	ComputeVelocityUpdate = LoadShaders(ComputeVSource);

	ShaderInfo VertexPSource[] = {
		{ GL_VERTEX_SHADER, "VBOBenchmark\\VertexPosition.shader" },
		{ GL_FRAGMENT_SHADER, "VBOBenchmark\\pass_through_fragment.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	VertexPositionUpdate = LoadShaders(VertexPSource);

	ShaderInfo  VertexVSource[] = {
		{ GL_VERTEX_SHADER, "VBOBenchmark\\VertexVelocity.shader" },
		{ GL_FRAGMENT_SHADER, "VBOBenchmark\\pass_through_fragment.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	VertexVelocityUpdate = LoadShaders(VertexVSource);

	ShaderInfo  DenseSource[] = {
		{ GL_COMPUTE_SHADER, "PostEffects\\Dense.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	DenseShader = LoadShaders(DenseSource);

	ShaderInfo  FillingSource[] = {
		{ GL_COMPUTE_SHADER, "VBOBenchmarkCommons\\Filling.shader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	FillingShader = LoadShaders(FillingSource);

	for (int i = 0; i < count; i++)
	{
		indexes.push_back(i);
	}
	glGenBuffers(1, &IndexesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(float), indexes.data(), GL_DYNAMIC_COPY);
	
	vector<vec2> _UV;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			_UV.push_back(vec2(i, j));
		}
	}
	glGenBuffers(1, &UV);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, UV);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, width*height * sizeof(vec2), _UV.data(), GL_DYNAMIC_COPY);



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

	int size = count;
	

	
	glGenBuffers(1, &ParticlesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticlesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, count*sizeof(struct Particle), NULL, GL_DYNAMIC_COPY);
	if(gpuFilling==false)
	{
		particles = (struct Particle *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, count*sizeof(struct Particle), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		for (int i=0 ; i < count; i++)
		{
			particles[i].acceleration = initialParticles.at(i).acceleration;
			particles[i].density = initialParticles.at(i).density;
			particles[i].mass = initialParticles.at(i).mass;
			particles[i].velocity = initialParticles.at(i).velocity;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(vec4), particlesPositions.data(), GL_DYNAMIC_COPY);
	
	glGenFramebuffers(1, &DefaultFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, DefaultFrameBuffer);
	

	for (int i = 0; i < height*width; i++)
	{
		emptyTexture.push_back(vec4(0, 0, 0, 0));
	}
	for (int i = 0; i < 2; i++)
	{
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, emptyTexture.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		denseTextures.push_back(texture);
	}
	// - Color 
	glGenTextures(1, &ColorTexture);
	glBindTexture(GL_TEXTURE_2D, ColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture, 0);

	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };// , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
	glDrawBuffers(1, attachments);
	
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f
	};


	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	std::vector<glm::vec3> vertices;



	ShaderInfo  shadersExpRender[] = {
		{ GL_VERTEX_SHADER,  "RendererShaders\\Passthrough.vertexshader" },
		{ GL_FRAGMENT_SHADER,"RendererShaders\\Renderer.fragmentshader" },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL },
		{ GL_NONE, NULL }
	};
	RenderShaders = LoadShaders(shadersExpRender);


	glGenFramebuffers(1, &FinalFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FinalFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NULL, 0);

	glGenBuffers(1, &DebugBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, DebugBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4), NULL, GL_DYNAMIC_COPY);
	vec4 *debug = (vec4 *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
double BenchmarkVBO::launchLoop()
{
	loopTotalTime = 0;
	double deltaTime = 0;
	double stallTime = 1.0;
	camera.setPosition(vec3(0, 50, 0));
	camera.setUp(vec3(1, 0, 0));
	camera.setForward(vec3(0, -1, 0));
	ViewMatrix = camera.cameraPositionKeyboard(0);
	vector<vec4> hp;
	GLuint drawMode = GL_FILL;
	double iddleTime = 0;
	do
	{
		static double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();
		deltaTime = double(currentTime - lastTime);
		loopTotalTime += deltaTime;
		///////////////
		bool updated = false;
		ViewMatrix = camera.cameraPositionKeyboard(deltaTime, &updated);
		glBindFramebuffer(GL_FRAMEBUFFER, DefaultFrameBuffer);
		/*
		if(updated == true )
		{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		iddleTime = 0;
		}
		else iddleTime += deltaTime;
		if(iddleTime<0.5) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		*/
		if (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		updateBuffers();
		drawMode = GL_FILL;
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		{
			drawMode = GL_LINE;
		}
		glPolygonMode(GL_FRONT_AND_BACK, drawMode);
		if (test == Test::VertexTest)
		{
			updateParticlesVertexShader(deltaTime);
		}
		if (test == Test::ComputeTest)
		{
			updateParticlesComputeShader(deltaTime);
		}
		if (test == Test::CPUTest)
		{
		}
		draw(drawMode);
		if(postEffect==1)
		densePostEffect();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glClear(GL_DEPTH_BUFFER_BIT);
		drawRendererOutputs();
		///////////////
		lastTime = currentTime;
		glfwSwapBuffers(window);
		glfwPollEvents();
		frames++;
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0 && loopTotalTime<testTime);
	glfwDestroyWindow(window);
	double result = frames / loopTotalTime;
	loopTotalTime = 0;
	frames = 0;
	return result;
}
void BenchmarkVBO::updateBuffers()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, PerFrameBuffer);
	perFrameData = (struct perFrameData *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(struct perFrameData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	perFrameData->ViewMatrix = ViewMatrix;
	perFrameData->time = loopTotalTime;
	perFrameData->seed = vec3(0.0f);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
void BenchmarkVBO::updateParticlesComputeShader(double deltaTime)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, PerFrameBuffer);

	glUseProgram(ComputeVelocityUpdate);
	glUniform1f(glGetUniformLocation(ComputeVelocityUpdate, "deltaTime"), deltaTime);
	glUniform1i(glGetUniformLocation(ComputeVelocityUpdate, "count"), count);
	glDispatchCompute(1, count / 1024+1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glUseProgram(ComputePositionUpdate);
	glUniform1f(glGetUniformLocation(ComputePositionUpdate, "deltaTime"), deltaTime);
	glDispatchCompute(1, count / 1024+1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
void BenchmarkVBO::updateParticlesVertexShader(double deltaTime)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, PerFrameBuffer);
	
	glUseProgram(VertexVelocityUpdate);
	glUniform1f(glGetUniformLocation(VertexVelocityUpdate, "deltaTime"), deltaTime);
	glUniform1i(glGetUniformLocation(VertexVelocityUpdate, "count"), count);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, IndexesBuffer);
	glVertexAttribPointer(
		3,                  // attribute. No particular reason for 3, but must match the layout in the shader.
		1,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);


	glDrawArrays(GL_POINTS, 0,count);
	
	glUseProgram(VertexPositionUpdate);
	glUniform1f(glGetUniformLocation(VertexPositionUpdate, "deltaTime"), deltaTime);
	
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, IndexesBuffer);
	glVertexAttribPointer(
		3,                  // attribute. No particular reason for 3, but must match the layout in the shader.
		1,                  // size
		GL_FLOAT,	      // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);


	glDrawArrays(GL_POINTS, 0, count);
	glDisableVertexAttribArray(3);
}
void BenchmarkVBO::draw(GLuint drawMode)
{
	if (denseTexture == 1)
	{
		denseTexture = 0;
	}
	else if (denseTexture == 0)
	{
		denseTexture = 1;
	}
	denseTexture = 0;
	glBindTexture(GL_TEXTURE_2D, denseTextures.at(0));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, emptyTexture.data());

	glPointSize(1);
	glBindImageTexture(0, denseTextures.at(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glUseProgram(RenderProgram);
	glUniform1i(glGetUniformLocation(RenderProgram, "denseTexture"), 0);
	glUniform1i(glGetUniformLocation(RenderProgram, "height"),height);
	glUniform1i(glGetUniformLocation(RenderProgram, "width"), width);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticlesBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, PerFrameBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ConstantBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, DebugBuffer);

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


	glDrawArrays(GL_POINTS, 0, count);
	glDisableVertexAttribArray(0);
///	gpuDebug("gl_Position");
}
void BenchmarkVBO::drawRendererOutputs()
{
	glUseProgram(RenderShaders);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ColorTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, denseTextures.at(1));
	glUniform1i(glGetUniformLocation(RenderShaders, "Color"), 0);
	glUniform1i(glGetUniformLocation(RenderShaders, "denseTexture"), 1);
	glEnableVertexAttribArray(11);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		11,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(11);
}
void BenchmarkVBO::densePostEffect()
{
	glBindImageTexture(0, denseTextures.at(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindTexture(GL_TEXTURE_2D, denseTextures.at(1));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, emptyTexture.data());
	glBindImageTexture(1, denseTextures.at(1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, DebugBuffer);
	glUseProgram(DenseShader);
	glUniform1i(glGetUniformLocation(DenseShader, "denseTexture"), 0);
	glUniform1i(glGetUniformLocation(DenseShader, "outputDenseTexture"), 1);
	glDispatchCompute(width, height, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
void BenchmarkVBO::initialsPartciles(int mode)
{
	if(mode==0)
	{
		for (int i = 0; i < count; i++)
		{
			particlesPositions.push_back(float(rand() % 200) / 10.0f*normalize(vec4(1000 - rand() % 2000, 1000 - rand() % 2000, 1000 - rand() % 2000, 1)));
		}

		for (int i = 0; i < count; i++)
		{
			Particle temp;
			temp.acceleration = vec4(0.0);
			temp.density = 1;
			temp.index = -1;
			temp.mass = 1;
			temp.velocity = vec4(0.0);
			initialParticles.push_back(temp);
		}
	}
	if (mode == 1)
	{
		int factor = 10;
		for (int i = 0; i < count/ factor; i++)
		{
			particlesPositions.push_back(float(rand() % 10) / 10.0f*normalize(vec4(1000 - rand() % 2000, 1000 - rand() % 2000, 1000 - rand() % 2000, 1)));
		}

		for (int i = 0; i < count/ factor; i++)
		{
			Particle temp;
			temp.acceleration = vec4(0.0);
			temp.density = 1;
			temp.index = -1;
			temp.mass = 1;
			temp.velocity = vec4(0.0);
			initialParticles.push_back(temp);
		}

		float mass = float(count) / float(factor);
		for (int i = count / factor; i < count ; i++)
		{
			particlesPositions.push_back((1+(float(rand() % 100) / 10.0f))*normalize(vec4(1000 - rand() % 2000, 0, 1000 - rand() % 2000, 1)));
		}

		for (int i = count / factor; i < count ; i++)
		{
			Particle temp;
			temp.acceleration = vec4(0.0);
			temp.density = 1;
			temp.index = -1;
			temp.mass = 1;
			float r = length(particlesPositions.at(i));
			vec3 dir = cross(normalize(particlesPositions.at(i).xyz()), vec3(0, 1, 0));
			float speed = 0.05*sqrt(0.1f*mass / r);
			temp.velocity =  vec4(dir*speed, 0);

			initialParticles.push_back(temp);
		}
	}

	if (mode == 2)
	{
		int factor = 10;
		for (int i = 0; i < count / factor; i++)
		{
			particlesPositions.push_back(float(rand() % 10) / 10.0f*normalize(vec4(1000 - rand() % 2000, 1000 - rand() % 2000, 1000 - rand() % 2000, 1)));
		}


		for (int i = 0; i < count / factor; i++)
		{
			Particle temp;
			temp.acceleration = vec4(0.0);
			temp.density = 1;
			temp.index = -1;
			temp.mass = 1;
			temp.velocity = vec4(0.0);
			initialParticles.push_back(temp);
		}

		float mass = float(count) / float(factor);
		for (int i = count / factor; i < count; i++)
		{
			int q = rand() % 8;
				 if(q==0)
				particlesPositions.push_back(-(1 + (float(rand() % 100) / 10.0f))*normalize(vec4(1,0,0,1)));
			else if( q==1)
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(1,0,0,1)));
			else if (q == 2)
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(0, 0, -1, 1)));
			else if (q == 3)
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(0, 0, 1, 1)));
			else if (q == 4)
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(1, 0, 1, 1)));
			else if (q == 5)
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(-1, 0, -1, 1)));
			else if (q == 6)
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(-1, 0, 1, 1)));
			else if (q == 7)
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(1, 0, -1, 1)));
		}

		for (int i = count / factor; i < count; i++)
		{
			Particle temp;
			temp.acceleration = vec4(0.0);
			temp.density = 1;
			temp.index = -1;
			temp.mass = 1;
			float r = length(particlesPositions.at(i));
			vec3 dir = cross(normalize(particlesPositions.at(i).xyz()), vec3(0, 1, 0));
			float speed = 0.015*r*sqrt(0.1f*mass / r);
			temp.velocity = vec4(dir*speed, 0);

			initialParticles.push_back(temp);
		}
	}

	if (mode == 3)
	{
	
		//	count /= 2;
			int _count = count / 2;
			int factor = 10;
			for (int i = 0; i < _count / factor; i++)
			{
				particlesPositions.push_back(float(rand() % 10) / 10.0f*normalize(vec4(1000 - rand() % 2000, 1000 - rand() % 2000, 1000 - rand() % 2000, 1)));
			}

			for (int i = 0; i < _count / factor; i++)
			{
				Particle temp;
				temp.acceleration = vec4(0.0);
				temp.density = 1;
				temp.index = -1;
				temp.mass = 1;
				temp.velocity = vec4(0.0);
				initialParticles.push_back(temp);
			}

			float mass = float(_count) / float(factor);
			for (int i = _count / factor; i < _count; i++)
			{
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(1000 - rand() % 2000, 0, 1000 - rand() % 2000, 1)));
			}

			for (int i = _count / factor; i < _count; i++)
			{
				Particle temp;
				temp.acceleration = vec4(0.0);
				temp.density = 1;
				temp.index = -1;
				temp.mass = 1;
				float r = length(particlesPositions.at(i));
				vec3 dir = cross(normalize(particlesPositions.at(i).xyz()), vec3(0, 1, 0));
				float speed = 0.05*sqrt(0.1f*mass / r);
				temp.velocity = vec4(dir*speed, 0);

				initialParticles.push_back(temp);
			}
			for (int i = 0; i < _count; i++)
			{
				particlesPositions.at(i) += vec4(10, 0, 0, 0);
			}
			////////////

			for (int i = 0; i < _count / factor; i++)
			{
				particlesPositions.push_back(float(rand() % 10) / 10.0f*normalize(vec4(1000 - rand() % 2000, 1000 - rand() % 2000, 1000 - rand() % 2000, 1)));
			}

			for (int i = 0; i < _count / factor; i++)
			{
				Particle temp;
				temp.acceleration = vec4(0.0);
				temp.density = 1;
				temp.index = -1;
				temp.mass = 1;
				temp.velocity = vec4(0.0);
				initialParticles.push_back(temp);
			}

		//	float mass = float(count) / float(factor);
			for (int i = _count / factor; i < _count; i++)
			{
				particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(1000 - rand() % 2000, 0, 1000 - rand() % 2000, 1)));
			}

			for (int i = _count / factor; i < _count; i++)
			{
				Particle temp;
				temp.acceleration = vec4(0.0);
				temp.density = 1;
				temp.index = -1;
				temp.mass = 1;
				float r = length(particlesPositions.at(i));
				vec3 dir = cross(normalize(particlesPositions.at(_count+i).xyz()), vec3(0, 1, 0));
				float speed = 0.05*sqrt(0.1f*mass / r);
				temp.velocity = vec4(dir*speed, 0);

				initialParticles.push_back(temp);
			}
			for (int i = _count; i < 2* _count; i++)
			{
				particlesPositions.at(i) += vec4(-10, 0, 0, 0);
				initialParticles.at(i).velocity += vec4(0.1, 0, 0, 1);
			}
	}
	if (mode == 4)
	{
		int factor = 10;
		for (int i = 0; i < count / factor; i++)
		{
			particlesPositions.push_back(float(rand() % 10) / 10.0f*normalize(vec4(1000 - rand() % 2000, 1000 - rand() % 2000, 1000 - rand() % 2000, 1)));
		}

		for (int i = 0; i < count / factor; i++)
		{
			Particle temp;
			temp.acceleration = vec4(0.0);
			temp.density = 1;
			temp.index = -1;
			temp.mass = 1;
			temp.velocity = vec4(0.0);
			initialParticles.push_back(temp);
		}

		float mass = float(count) / float(factor);
		for (int i = count / factor; i < count; i++)
		{
			particlesPositions.push_back((1 + (float(rand() % 100) / 10.0f))*normalize(vec4(1000 - rand() % 2000, 0, 1000 - rand() % 2000, 1)));
		}

		for (int i = count / factor; i < count; i++)
		{
			Particle temp;
			temp.acceleration = vec4(0.0);
			temp.density = 1;
			temp.index = -1;
			temp.mass = 1;
			float r = length(particlesPositions.at(i));
			vec3 dir = cross(normalize(particlesPositions.at(i).xyz()), vec3(0, 1, 0));
			float speed = 0.05*sqrt(0.1f*mass / sqrt(r));
			temp.velocity = vec4(dir*speed, 0);

			initialParticles.push_back(temp);
		}

	}
}
