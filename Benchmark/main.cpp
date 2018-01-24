#include "Benchmark.h"
#include "BenchmarkVBO.h"
#include "BenchmarkOcean.h"
using namespace std;
using namespace glm;

int main()
{
	srand(time(NULL));
	int test = 2;
	if (test == 0)
	{
		int octaves = 2;
		cout << "-----------Compute test--------" << endl;
		for(int i=0;i<6;i++)
		{
			Benchmark m_benchmark;
			m_benchmark.init(1280,1024,0,100);
			m_benchmark.initBuffers();
			m_benchmark.setOctaves(octaves);
			cout << "Average FPS for "<<octaves<<"octaves is " << m_benchmark.launchLoop() << endl;
			octaves *= 2;
		}

		octaves = 2;
		cout << "-----------Vertex test--------" << endl;
		for (int i = 0; i<6; i++)
		{
			Benchmark m_benchmark;
			m_benchmark.init(1280, 1024, 1, 100);
			m_benchmark.initBuffers();
			m_benchmark.setOctaves(octaves);
			cout << "Average FPS for " << octaves << "octaves is " << m_benchmark.launchLoop() << endl;
			octaves *= 2;
		}

		cout << "Press any key to close" << endl;
		_getch();
	}
	if (test == 1)
	{
		/*
		cout << "Podaj tryb particli" << endl;
		int q = 0,w,h,count, test, postEffect;
		cin >> q;
		cout << "Podaj rozdzielczosc x i y" << endl;
		cin >> w;
		cin >> h;
		cout << "Podaj ilosc particli" << endl;
		cin >> count;
		cout << "Test?" << endl;
		cin >> test;
		cout << "post effect?" << endl;
		cin >> postEffect;*/

		BenchmarkVBO m_benchmark(10240, 1,1);
		m_benchmark.initialsPartciles(1);
		m_benchmark.init(1280, 1024);
		m_benchmark.initBuffers();
		cout << "Result:" << m_benchmark.launchLoop() << endl;
		_getch();
	}
	if (test == 2)
	{

		int octaves = 2;
		cout << "-----------Compute test--------" << endl;
		for (int i = 0; i<6; i++)
		{
			BenchmarkOcean m_benchmark;
			m_benchmark.init(1280, 1024, 0, 1000);
			m_benchmark.initBuffers();
			m_benchmark.setOctaves(octaves);
			cout << "Average FPS for " << octaves << "octaves is " << m_benchmark.launchLoop() << endl;
			octaves *= 2;
		}

		octaves = 2;
		cout << "-----------Vertex test--------" << endl;
		for (int i = 0; i<6; i++)
		{
			BenchmarkOcean m_benchmark;
			m_benchmark.init(1280, 1024, 1, 1000);
			m_benchmark.initBuffers();
			m_benchmark.setOctaves(octaves);
			cout << "Average FPS for " << octaves << "octaves is " << m_benchmark.launchLoop() << endl;
			octaves *= 2;
		}

		cout << "Press any key to close" << endl;
		_getch();
	}

	return 0;
}


