#include "Benchmark.h"
using namespace std;
using namespace glm;

int main()
{
	Benchmark m_benchmark;
	m_benchmark.initWindow();
	m_benchmark.initBuffers();
	m_benchmark.launchLoop();
     cout << "Hello world" << endl;
//	 _getch();
	return 0;
}


