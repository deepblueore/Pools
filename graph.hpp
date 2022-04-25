#pragma once

#include <cstdio>
#include <random>
#include "pool.hpp"

class Graph
{
	private:
		Pool* _pools; //массив бассейнов

		void _makePipe(const int& firstPos, const int& secondPos); //соединить бассейны трубой
		void _addWater(const int& poolPos, const double& waterMeasure); //добавить воду в бассейн

		void _dfs(int poolPos, int* usedPools, int* globalUsed, int& adjacencyCount, double& adjacencySum); //запуск dfs из бассейна
		int _intRand(const int& min, const int& max); //генератор чисел
		void _merge(); //распределение воды через трубы перед выводом на экран
	public:
		Graph();
		~Graph();

		//функции нужны для реализации интерфейса
		void makePipes();
		void addWater();
		void getMeasures();
};
