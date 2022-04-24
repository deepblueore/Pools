#pragma once

#include <vector>
#include <mutex>
#include <thread>
#include <iostream>
#include <exception>
#include <map>
#include <utility>
#include <cstdio>
#include <random>

#define POOL_COUNT 9000000 //число бассейнов
#define POOL_GEN_THREADS 4 //потоки генерации бассейна
#define ADD_THREADS 4 //потоки добавления воды
#define CONNECT_THREADS 4 //потоки соединения
#define MEASURE_THREADS 4 
#define CONNECT_NUM 10 //число труб
#define ADDITION_COUNT 150 //число добавлений воды
#define R_MIN 1 //минимальный добавляемый объем
#define R_MAX 20

class iPool
{
        private:
                long double _measure = 0; //объем воды в бассейне
                int _position = -1; //номер бассейна в векторе бассейнов
        public:
                iPool();
                long double getMeasure() const; //вернуть объем
                void add(const long double water); //добавить воду
                void connect(iPool* pool); //соединить бассейн с pool

                int getPos() const; //вернуть порядковый номер бассейна
                void setMeasure(const long double newMeasure); //установить новый объем
		void setPosition(const int position);
                void dfs(std::vector<iPool*>& poolsComp, std::vector<bool>& isUsed); //обход в глубину по трубам
};

//Глобальные переменные (в перспективе - убрать)
extern std::vector<iPool*> pools;
extern std::map<int, std::vector<iPool*>> pConnects;

class Interface
{
	private:

		int _intRand(const int& min, const int& max); //thread-safe генератор случайных чисел
		void _genPools(std::vector<iPool*>* poolVec); //генератор бассейнов
		void _addToPools(std::vector<std::pair<iPool*, long double>>* toPool); //добавление воды
		void _connectPools(std::vector<std::pair<int, int>>* toPool); //построение трубы
	public:
		Interface();
		void generate();
		void connect();
		void addWater();
};
