#include "graph.hpp"

#define POOL_COUNT 9000000 //число бассейнов
#define PIPE_COUNT 14000000 //число труб
#define ADD_COUNT 35000000 //число добавлений воды
#define W_MIN 1
#define W_MAX 10

Graph::Graph()
{
	_pools = new Pool[POOL_COUNT]; //инициализация массива бассейнов
	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		_pools[poolIter].setPos(poolIter);
		_pools[poolIter].setMeas(_intRand(W_MIN, W_MAX));
	}
}

Graph::~Graph()
{
	delete[] _pools;
}

void Graph::_makePipe(const int& firstPos, const int& secondPos)
{
	//корректировка списка смежности
	_pools[firstPos].pushPipe(secondPos);
	_pools[secondPos].pushPipe(firstPos);

	//перераспределение воды по компоненте связности бассейна
	int* usedPools = new int[POOL_COUNT]; //посещенные бассейны
	int adjacencyCount = 0; //размер компоненты связности
	double sumMeasure = 0; //объем компоненты связности
	double newMeasure; //распределенный объем

	_dfs(firstPos, usedPools, adjacencyCount, sumMeasure); //собираем компоненту связности бассейна

	newMeasure = sumMeasure / adjacencyCount;

	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		if(usedPools[poolIter])
		{
			_pools[poolIter].setMeas(newMeasure);
		}
	}

	delete[] usedPools;
}

void Graph::_dfs(int poolPos, int* usedPools, int& adjacencyCount, double& adjacencySum)
{
	usedPools[poolPos] = 1; //помечаем вершину посещенной
	++adjacencyCount; //увеличиваем размер компоненты связности
	adjacencySum += _pools[poolPos].getMeas();

	unsigned int* pipesToPool = _pools[poolPos].getPipes();
	for(unsigned int poolIter = 0; poolIter < _pools[poolPos].getSize(); ++poolIter)
	{
		if(pipesToPool[poolIter] == poolPos){continue;}
		else if(!usedPools[pipesToPool[poolIter]])
		{
			_dfs(pipesToPool[poolIter], usedPools, adjacencyCount, adjacencySum);
		}
	}
}

void Graph::_addWater(const int& poolPos, const double& waterMeasure)
{
	int* usedPools = new int[POOL_COUNT];
	int adjacencyCount = 0;
	double adjacencySum = 0;
	double additionalMeasure;

	_dfs(poolPos, usedPools, adjacencyCount, adjacencySum); //собираем компоненту связности бассейна

	additionalMeasure = waterMeasure / adjacencyCount;
	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		if(usedPools[poolIter])
		{
			_pools[poolIter].addMeas(additionalMeasure);
		}
	}

	delete[] usedPools;
}

void Graph::makePipes()
{
	for(unsigned int pipeIter = 0; pipeIter < PIPE_COUNT; ++pipeIter)
	{
		printf("piped! (%d)\n", pipeIter);
		_makePipe(_intRand(0, POOL_COUNT - 1), _intRand(0, POOL_COUNT - 1));
	}
}

void Graph::addWater()
{
	for(unsigned int additionIter = 0; additionIter < ADD_COUNT; ++additionIter)
	{
		printf("added! (%d)\n", additionIter);
		_addWater(_intRand(0, POOL_COUNT - 1), _intRand(W_MIN, W_MAX));
	}
}

void Graph::getMeasures()
{
	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		printf("pool %d: %f liters\n", poolIter, _pools[poolIter].getMeas());
	}
	printf("\n\n\n\n\n");
}

int Graph::_intRand(const int& min, const int& max)
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(min,max);
	return distribution(generator);
}

