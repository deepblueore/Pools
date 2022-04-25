#include "graph.hpp"

#define POOL_COUNT 10 //число бассейнов
#define PIPE_COUNT 3 //число труб
#define ADD_COUNT 3 //число добавлений воды
#define W_MIN 1
#define W_MAX 10

Graph::Graph()
{
	_globalUsed = new unsigned int[POOL_COUNT];

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
	delete[] _globalUsed;
}

void Graph::_pushToAdj(const unsigned int& poolPos)
{
	++_adjSize;
	if(_adjSize > _adjCapacity)
	{
		unsigned int* newAdjComponent = new unsigned int[_adjCapacity * 2];
		std::memcpy(newAdjComponent, _adjComponent, sizeof(unsigned int) * _adjCapacity);
		newAdjComponent[_adjSize - 1] = poolPos;
		_adjCapacity *= 2;

		delete[] _adjComponent;
		_adjComponent = newAdjComponent;
	}
	else
	{
		_adjComponent[_adjSize - 1] = poolPos;
	}
}

void Graph::_makePipe(const int& firstPos, const int& secondPos)
{
	//корректировка списка смежности
	_pools[firstPos].pushPipe(secondPos);
	_pools[secondPos].pushPipe(firstPos);
}

void Graph::_dfs(int poolPos, int* usedPools, int& adjacencyCount, double& adjacencySum)
{
	_globalUsed[poolPos] = 1;
	usedPools[poolPos] = 1; //помечаем вершину посещенной
	_pushToAdj(poolPos);
	++adjacencyCount; //увеличиваем размер компоненты связности
	adjacencySum += _pools[poolPos].getMeas();

	unsigned int* pipesToPool = _pools[poolPos].getPipes();
	for(unsigned int poolIter = 0; poolIter < _pools[poolPos].getSize(); ++poolIter)
	{
		printf("%d ", poolIter);
		if(pipesToPool[poolIter] == poolPos){continue;}
		else if(!usedPools[pipesToPool[poolIter]])
		{
			_dfs(pipesToPool[poolIter], usedPools, adjacencyCount, adjacencySum);
		}
	}
	printf("\n");
}

void Graph::_merge()
{
	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		if(_globalUsed[poolIter]){continue;}

		_adjComponent = new unsigned int[ADJACENCY_SIZE];

		int* usedPools = new int[POOL_COUNT];
		std::memset(usedPools, 0, POOL_COUNT);
		int adjacencyCount = 0;
		double adjacencySum = 0;
		double newMeasure;

		_dfs(poolIter, usedPools, adjacencyCount, adjacencySum);

		newMeasure = adjacencySum / adjacencyCount;
		for(unsigned int iter = 0; iter < POOL_COUNT; ++iter)
		{
			if(usedPools[iter])
			{
				_pools[iter].setMeas(newMeasure);
			}
		}

		delete[] usedPools;
		_clearAdjacency();
	}
}

void Graph::_clearAdjacency()
{
	if(_adjComponent != NULL)
	{
		delete[] _adjComponent;
		_adjComponent = NULL;
		_adjSize = 0;
		_adjCapacity = ADJACENCY_SIZE;
	}
}

void Graph::_addWater(const int& poolPos, const double& waterMeasure)
{
	_pools[poolPos].addMeas(waterMeasure);
}

void Graph::makePipes()
{
	for(unsigned int pipeIter = 0; pipeIter < PIPE_COUNT; ++pipeIter)
	{
		int a = _intRand(0, POOL_COUNT - 1);
		int b = _intRand(0, POOL_COUNT - 1);
		printf("%d %d\n", a, b);
		_makePipe(a, b);
	}
}

void Graph::addWater()
{
	for(unsigned int additionIter = 0; additionIter < ADD_COUNT; ++additionIter)
	{
		_addWater(_intRand(0, POOL_COUNT - 1), _intRand(W_MIN, W_MAX));
	}
}

void Graph::_clearGlobalUsed()
{
	delete[] _globalUsed;
	_globalUsed = new unsigned int[POOL_COUNT];
}

void Graph::getMeasures()
{
	_merge();
	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		printf("pool %d: %f liters\n", poolIter, _pools[poolIter].getMeas());
	}
	printf("\n\n\n\n\n");
	_clearGlobalUsed();
}

int Graph::_intRand(const int& min, const int& max)
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(min,max);
	return distribution(generator);
}

