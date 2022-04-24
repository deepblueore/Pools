#include "pool.hpp"
#include "defines.hpp"

Graph::Graph()
{
	_pools = new pool[POOL_COUNT]; //инициализация массива бассейнов

	_adjacency = new int* [POOL_COUNT];
	for(unsigned int rowIter = 0; rowIter < POOL_COUNT; ++rowIter)
	{
		_pools[rowIter].measure = _intRand(W_MIN, W_MAX); //первое добавление воды в каждый бассейн

		_adjacency[rowIter] = new int[POOL_COUNT]; //инициализация столбцов матрицы смежности
		for(unsigned int columnIter = 0; columnIter < POOL_COUNT; ++columnIter)
		{
			//изначально матрица единичная
			_adjacency[rowIter][columnIter] = 0;
			_adjacency[rowIter][rowIter] = 1;
		}
	}
}

Graph::~Graph()
{
	delete[] _pools;
	for(unsigned int rowIter = 0; rowIter < POOL_COUNT; ++rowIter)
	{
		delete[] _adjacency[rowIter];
	}
}

int Graph::_intRand(const int& min, const int& max) const
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(min,max);
	return distribution(generator);
}

void Graph::_makePipe(const int& firstPos, const int& secondPos)
{
	//корректировка матрицы смежности (не самый оптимальный вариант)
	_adjacency[firstPos][secondPos] = 1;
	_adjacency[secondPos][firstPos] = 1;

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
			_pools[poolIter].measure = newMeasure;
		}
	}

	delete[] usedPools;
}

void Graph::_dfs(int poolPos, int* usedPools, int& adjacencyCount, double& adjacencySum)
{
	usedPools[poolPos] = 1; //помечаем вершину посещенной
	++adjacencyCount; //увеличиваем размер компоненты связности
	adjacencySum += _pools[poolPos].measure;
	for(unsigned int columnIter = 0; columnIter < POOL_COUNT; ++columnIter) //обход возможных соседей
	{
		if(columnIter == poolPos){continue;}
		else if(_adjacency[poolPos][columnIter] && !usedPools[columnIter]){_dfs(columnIter, usedPools, adjacencyCount, adjacencySum);}
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
			_pools[poolIter].measure += additionalMeasure;
		}
	}

	delete[] usedPools;
}

void Graph::makePipes()
{
	for(unsigned int pipeIter = 0; pipeIter < PIPE_COUNT; ++pipeIter)
	{
		_makePipe(_intRand(0, POOL_COUNT-1), _intRand(0, POOL_COUNT-1));
	}
}

void Graph::addWater()
{
	for(unsigned int additionIter = 0; additionIter < ADD_COUNT; ++additionIter)
	{
		_addWater(_intRand(0, POOL_COUNT - 1), _intRand(W_MIN, W_MAX));
	}
}

void Graph::getMeasures()
{
	for(unsigned int poolIter = 0; poolIter < POOL_COUNT; ++poolIter)
	{
		printf("pool %d: %f liters\n", poolIter, _pools[poolIter].measure);
	}
	printf("\n\n\n\n\n");
}
