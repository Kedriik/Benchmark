#include "Benchmark.h"
#include "BenchmarkVBO.h"
#include "BenchmarkOcean.h"
#include <fstream>
using namespace std;
using namespace glm;

int main()
{
	srand(time(NULL));
	int test = 1;
	if (test == 4)
	{
		Benchmark m_benchmark;
		m_benchmark.init(1280, 1024, 2, 100);
		m_benchmark.initBuffers();
		m_benchmark.setOctaves(1);
		cout << "Average FPS for blank is  " << m_benchmark.launchLoop() << endl;
		_getch();
		fstream file;
		file.open("Benchmark.csv", 'w');
		file.close();
	}
	if (test == 5)
	{
		fstream file;
			file.open("Benchmark.csv", 'w');

		for (int j = 1; j < 6; j++)
		{
			int hmSize = 500 + 100 * j;
			file << "Heightmap Size" << hmSize << "x" << hmSize << endl;
			file << "octaves,";
			file << "Compute Shader,";
			file << "Vertex Shader" << endl;


			int octaves = 2;
			for (int i = 0; i < 6; i++)
			{
				file << octaves << ",";
				Benchmark m_benchmark;
				m_benchmark.init(1280, 1024, 0, hmSize);
				m_benchmark.initBuffers();
				m_benchmark.setOctaves(octaves);
				file << m_benchmark.launchLoop() << ",";

				Benchmark m_benchmark1;
				m_benchmark1.init(1280, 1024, 1, hmSize);
				m_benchmark1.initBuffers();
				m_benchmark1.setOctaves(octaves);
				file << m_benchmark1.launchLoop() << endl;
				octaves *= 2;

			}
		}
			file.close();


			file.open("BenchmarkOcean.csv", 'w');

			for (int j = 1; j < 6; j++)
			{
				int hmSize = 500+100 * j;
				file << "Heightmap Size" << hmSize << "x" << hmSize << endl;
				file << "octaves,";
				file << "Compute Shader,";
				file << "Vertex Shader" << endl;


				int octaves = 2;
				for (int i = 0; i < 6; i++)
				{
					file << octaves << ",";
					BenchmarkOcean m_benchmark;
					m_benchmark.init(1280, 1024, 0, hmSize);
					m_benchmark.initBuffers();
					m_benchmark.setOctaves(octaves);
					file << m_benchmark.launchLoop() << ",";

					BenchmarkOcean m_benchmark1;
					m_benchmark1.init(1280, 1024, 1, hmSize);
					m_benchmark1.initBuffers();
					m_benchmark1.setOctaves(octaves);
					file << m_benchmark1.launchLoop() << endl;
					octaves *= 2;

				}
				cout << "Press any key to close" << endl;
			}
			file.close();
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
	if (test == 3)
	{
		int octaves = 64;
		cout << "-----------Compute CPU TIME PROFILLING test--------" << endl;
		
			Benchmark m_benchmark;
			m_benchmark.init(1280, 1024, 0, 100);
			m_benchmark.initBuffers();
			m_benchmark.setOctaves(octaves);
			cout << "Average FPS for " << octaves << "octaves is " << m_benchmark.launchLoop() << endl;
			octaves *= 2;
		
		cout << "Press any key to close" << endl;
		_getch();
	}

	return 0;
}


