#include "graph.hpp"
#include "defines.hpp"

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
}

void Graph::_merge()
{
	int* globalUsed = new int[POOL_COUNT]; //нужно, чтобы не заходить в одну компоненту связности дважды
	std::memset(globalUsed, 0, POOL_COUNT * sizeof(unsigned int));

	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		if(globalUsed[poolIter]){continue;}
		int* usedPools = new int[POOL_COUNT]; //вершины в компоненте связности текущей вершины
		std::memset(usedPools, 0, POOL_COUNT * sizeof(unsigned int));
		int adjacencyCount = 0; //число вершин в компоненте связности
		double adjacencySum = 0; //объем в компоненте
		double newMeasure;

		_dfs(poolIter, usedPools, globalUsed, adjacencyCount, adjacencySum);

		newMeasure = adjacencySum / adjacencyCount;
		for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
		{
			if(usedPools[poolIter])
			{
				_pools[poolIter].setMeas(newMeasure);
			}
		}
		delete[] usedPools;
	}
	delete[] globalUsed;
}

void Graph::_dfs(int poolPos, int* usedPools, int* globalUsed, int& adjacencyCount, double& adjacencySum)
{
	usedPools[poolPos] = 1; //помечаем вершину посещенной
	globalUsed[poolPos] = 1;
	++adjacencyCount; //увеличиваем размер компоненты связности
	adjacencySum += _pools[poolPos].getMeas();

	unsigned int* pipesToPool = _pools[poolPos].getPipes(); //собираем вершины, смежные с текущей
	for(unsigned int poolIter = 0; poolIter < _pools[poolPos].getSize(); ++poolIter)
	{
		if(pipesToPool[poolIter] == poolPos){continue;}
		else if(!usedPools[pipesToPool[poolIter]])
		{
			_dfs(pipesToPool[poolIter], usedPools, globalUsed, adjacencyCount, adjacencySum);
		}
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
//		printf("piped! (%d)\n", pipeIter);
		_makePipe(_intRand(0, POOL_COUNT - 1), _intRand(0, POOL_COUNT - 1));
	}
}

void Graph::addWater()
{
	for(unsigned int additionIter = 0; additionIter < ADD_COUNT; ++additionIter)
	{
//		printf("added! (%d)\n", additionIter);
		_addWater(_intRand(0, POOL_COUNT - 1), _intRand(W_MIN, W_MAX));
	}
}

void Graph::getMeasures()
{
	_merge();
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

