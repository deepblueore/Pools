#pragma once

#include <cstdio>
#include <random>

typedef struct
{
	double measure{0};
}pool;


class Graph
{
	private:
		pool* _pools; //массив бассейнов
		int** _adjacency; //матрица: 1, если есть труба; 0 - иначе

		void _makePipe(const int& firstPos, const int& secondPos); //соединить бассейны трубой
		void _addWater(const int& poolPos, const double& waterMeasure); //добавить воду в бассейн

		void _dfs(int poolPos, int* usedPools, int& adjacencyCount, double& adjacencySum); //запуск dfs из бассейна
		int _intRand(const int& min, const int& max) const; //генератор чисел
	public:
		Graph();
		~Graph();

		//функции нужны для реализации интерфейса
		void makePipes();
		void addWater();
		void getMeasures();
};
