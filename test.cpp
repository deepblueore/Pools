#include "pool.hpp"
#include <random>

#define POOL_COUNT 100
#define POOL_GEN_THREADS 4
#define ADD_THREADS 4
#define CONNECT_THREADS 4
#define MEASURE_THREADS 4
#define CONNECT_NUM 20
#define ADDITION_COUNT 150
#define R_MIN 1
#define R_MAX 20

std::vector<iPool*> pools;
std::map<int, std::vector<iPool*>> pConnects;
std::mutex poolsMutex;
std::mutex genMutex;
std::mutex addMutex;
std::mutex connectMutex;

int intRand(const int& min, const int& max)
{
        static thread_local std::mt19937 generator;
        std::uniform_int_distribution<int> distribution(min,max);
        return distribution(generator);
}

void genPools(std::vector<iPool*>* poolVec)
{
	for(int iter = 0; iter < POOL_COUNT / POOL_GEN_THREADS; ++iter)
	{
		genMutex.lock();
		poolVec->push_back(new iPool());
		genMutex.unlock();
	}
}

void addToPools(std::vector<std::pair<iPool*, long double>>* toPool)
{
	while(true)
	{
		addMutex.lock();
		if(!toPool->empty())
		{
			std::vector<std::pair<iPool*, long double>>::iterator curPair = toPool->end() - 1;
			curPair->first->add(curPair->second);
			toPool->pop_back();
			addMutex.unlock();
		}
		else
		{
			addMutex.unlock();
			break;
		}
	}
}

void connectPools(std::vector<std::pair<int, int>>* toPool)
{
	while(true)
	{
		connectMutex.lock();
		if(!toPool->empty())
		{
			std::vector<std::pair<int, int>>::iterator curPair = toPool->end() - 1;
			pools[curPair->first]->connect(pools[curPair->second]);
			toPool->pop_back();
			connectMutex.unlock();
		}
		else
		{
			connectMutex.unlock();
			break;
		}
	}
}


int main()
{
	//генерация и первичное заполнение POOL_COUNT бассейнов в POOL_GEN_THREADS потоков
	std::vector<std::thread> commands;
	for(int iter = 0; iter < POOL_GEN_THREADS; ++iter){commands.emplace_back(std::thread(genPools, &pools));}
	for(int iter = 0; iter < POOL_GEN_THREADS; ++iter){commands[iter].join();}
	commands.clear();

	//добавление воды в ADD_THREADS потоков
	std::vector<std::pair<iPool*, long double>> poolsToAdd;
	for(int iter = 0; iter < POOL_COUNT; ++iter){poolsToAdd.emplace_back(std::make_pair(pools[iter], intRand(R_MIN, R_MAX)));}
	for(int iter = 0; iter < ADD_THREADS; ++iter){commands.emplace_back(std::thread(addToPools, &poolsToAdd));};
	for(int iter = 0; iter < ADD_THREADS; ++iter){commands[iter].join();}
	commands.clear();
	printf("After first addition:\n");
	for(iPool* curPool : pools){printf("Pool %d: %Lf liters\n", curPool->getPos(), curPool->getMeasure());}

	//соединение случайных CONNECT_NUM бассейнов в CONNECT_THREADS потоков
	std::vector<std::pair<int, int>> poolsToConnect;
	for(int iter = 0; iter < CONNECT_NUM; ++iter)
	{
		int a = intRand(0, POOL_COUNT - 1);
		int b = intRand(0, POOL_COUNT - 1);
		printf("\nConnection built: %d %d\n", a, b);
		poolsToConnect.emplace_back(std::make_pair(a, b));
	}
	printf("\n");
	for(int iter = 0; iter < CONNECT_THREADS; ++iter){commands.emplace_back(std::thread(connectPools, &poolsToConnect));}
	for(int iter = 0; iter < CONNECT_THREADS; ++iter){commands[iter].join();}
	commands.clear();
	printf("After connection:\n");
	for(iPool* curPool : pools){printf("Pool %d: %Lf liters\n", curPool->getPos(), curPool->getMeasure());}

	//добавление воды ADDITION_COUNT раз
	
}

