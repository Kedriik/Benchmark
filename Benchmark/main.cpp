#include "Benchmark.h"
#include "BenchmarkVBO.h"
using namespace std;
using namespace glm;

int main()
{

	srand(time(NULL));
	int test = 1;
	if(test==0)
	{
		Benchmark m_benchmark;
		m_benchmark.init(1280,1024);
		m_benchmark.initBuffers();
		m_benchmark.launchLoop();
	}
	if (test == 1)
	{
		BenchmarkVBO m_benchmark;
		m_benchmark.initialsPartciles(0);
		m_benchmark.init(1280, 1024);
		m_benchmark.initBuffers();
		m_benchmark.launchLoop();
	}

	return 0;
}


