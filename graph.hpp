#pragma once

#include <cstdio>
#include <random>
#include "pool.hpp"

#define ADJACENCY_SIZE 50

class Graph
{
	private:
		Pool* _pools; //массив бассейнов

		unsigned int* _adjComponent;
		unsigned int _adjSize{0};
		unsigned int _adjCapacity{ADJACENCY_SIZE};
		void _pushToAdj(const unsigned int& poolPos);
		void _clearAdjacency();

		unsigned int* _globalUsed;
		void _clearGlobalUsed();

		void _makePipe(const int& firstPos, const int& secondPos); //соединить бассейны трубой
		void _addWater(const int& poolPos, const double& waterMeasure); //добавить воду в бассейн

		void _dfs(int poolPos, int* usedPools, int& adjacencyCount, double& adjacencySum); //запуск dfs из бассейна
		int _intRand(const int& min, const int& max);
		void _merge();
	public:
		Graph();
		~Graph();

		//функции нужны для реализации интерфейса
		void makePipes();
		void addWater();
		void getMeasures();
};
