#include "pool.hpp"

std::vector<iPool*> pools;
std::map<int, std::vector<iPool*>> pConnects;
std::mutex genMutex;
std::mutex addMutex;
std::mutex connectMutex;
std::mutex poolsMutex;

iPool::iPool()
{
	//позиция в векторе бассейнов пишется в position,
	//в список смежности добавляется соответствующая одна пара
        _position = pools.size();
        pConnects.insert(std::make_pair(_position, std::vector<iPool*>{}));
}

long double iPool::getMeasure() const{return _measure;}

int iPool::getPos() const
{
        if(_position == -1){throw std::runtime_error("position undefined!");} //если позиция не определена
        return _position;
}

void iPool::add(const long double water)
{
	//заводится вектор посещенных вершин и вектор для компоненты связности 
	//this в компоненте связности по умолчанию
	//poolsMutex.lock();
        std::vector<bool> isUsed{};
        for(int iter = 0; iter < pools.size(); ++iter){isUsed.push_back(false);}
        std::vector<iPool*> poolsComp{};
        poolsComp.push_back(pools[_position]);
        dfs(poolsComp, isUsed);

	poolsMutex.lock();
	//косметическая печать
	//printf("Got component (add): ");
        //for(iPool* curPool : poolsComp){printf("%d ", curPool->getPos());}
        //printf("-> %Lf\n", water);

	//добавленная вода равномерно распределяется по компоненте связности
	long double newWater = water / poolsComp.size();
        for(iPool* curPool : poolsComp){curPool->setMeasure(curPool->getMeasure() + newWater);}

	//еще одна косметическая печать
        //for(iPool* curPool : pools){printf("Pool %d: %Lf liters\n", curPool->getPos(), curPool->getMeasure());}
        poolsMutex.unlock();
}

void iPool::connect(iPool* pool)
{
	//инициализируются переменная для хранения объема компоненты связности
	//и указатель на текущую пару списка смежности
        long double connectedMeasure = 0;
        std::map<int, std::vector<iPool*>>::iterator mapIter{NULL};

        //обновляется список смежности
        poolsMutex.lock();
        mapIter = pConnects.find(_position);
        mapIter->second.push_back(pool);
        mapIter = pConnects.find(pool->getPos());
        mapIter->second.push_back(pools[_position]);
	poolsMutex.unlock();

        //dfs собирает для бассейна this компоненту связности
        std::vector<bool> isUsed{};
        for(int iter = 0; iter < pools.size(); ++iter){isUsed.push_back(false);}
        std::vector<iPool*> poolsComp{};
        poolsComp.push_back(pools[_position]);
        dfs(poolsComp, isUsed);

	poolsMutex.lock();

        //суммарный объем компоненты связности собирается в connectedMeasure
        for(iPool* curPool : poolsComp){connectedMeasure += curPool->getMeasure();}

        //накопленный объем разделяется поровну между связанными бассейнами
        long double newMeasure = connectedMeasure / poolsComp.size();

	//косметическая печать
	//printf("Got component (connect): ");
        //for(iPool* curPool : poolsComp){printf("%d ", curPool->getPos());}
        //printf("\n");

	//объем устанавливается на распределенный
        for(iPool* curPool : poolsComp){curPool->setMeasure(newMeasure);}
        
	//еще одна косметическая печать
	//for(iPool* curPool : pools){printf("Pool %d: %Lf liters\n", curPool->getPos(), curPool->getMeasure());}
        poolsMutex.unlock();
}

void iPool::dfs(std::vector<iPool*>& poolsComp, std::vector<bool>& isUsed)
{
	//алгоритм помечает текущий бассейн и рекурсивно запускается из всех
	//непосещенных соседей
        isUsed[_position] = true;
        std::map<int, std::vector<iPool*>>::iterator dfsIter{NULL};
	poolsMutex.lock();
        dfsIter = pConnects.find(_position);
	poolsMutex.unlock();
        for(iPool* curPool : dfsIter->second)
        {
                if(isUsed[curPool->getPos()]){continue;}
                poolsComp.push_back(curPool);
                curPool->dfs(poolsComp, isUsed);
        }
}

void iPool::setMeasure(const long double newMeasure){_measure = newMeasure;}
void iPool::setPosition(const int position){_position = position;}

int Interface::_intRand(const int& min, const int& max)
{
        static thread_local std::mt19937 generator;
        std::uniform_int_distribution<int> distribution(min,max);
        return distribution(generator);
}

void Interface::_genPools(std::vector<iPool*>* poolVec)
{
	for(int iter = 0; iter < POOL_COUNT / POOL_GEN_THREADS; ++iter)
	{
		genMutex.lock();
		poolVec->push_back(new iPool());
		genMutex.unlock();
	}
}

void Interface::_addToPools(std::vector<std::pair<iPool*, long double>>* toPool)
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

void Interface::_connectPools(std::vector<std::pair<int, int>>* toPool)
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

Interface::Interface(){;}

void Interface::generate()
{
	//генерация и первичное заполнение POOL_COUNT бассейнов в POOL_GEN_THREADS потоков
	std::vector<std::thread> commands;
	for(int iter = 0; iter < POOL_GEN_THREADS; ++iter){commands.emplace_back(std::thread(&Interface::_genPools, this, &pools));}
	for(int iter = 0; iter < POOL_GEN_THREADS; ++iter){commands[iter].join();}
	commands.clear();

	//добавление воды в ADD_THREADS потоков
	std::vector<std::pair<iPool*, long double>> poolsToAdd;
	for(int iter = 0; iter < POOL_COUNT; ++iter){poolsToAdd.emplace_back(std::make_pair(pools[iter], _intRand(R_MIN, R_MAX)));}
	for(int iter = 0; iter < ADD_THREADS; ++iter){commands.emplace_back(std::thread(&Interface::_addToPools, this, &poolsToAdd));};
	for(int iter = 0; iter < ADD_THREADS; ++iter){commands[iter].join();}
	commands.clear();

	//косметическая печать
	printf("When generated:\n");
	for(iPool* curPool : pools){printf("Pool %d: %Lf liters\n", curPool->getPos(), curPool->getMeasure());}
}

void Interface::connect()
{
	std::vector<std::pair<int, int>> poolsToConnect;
	std::vector<std::thread> commands;

	//косметическая печать
	for(int iter = 0; iter < CONNECT_NUM; ++iter)
	{
		int a = _intRand(0, POOL_COUNT - 1);
		int b = _intRand(0, POOL_COUNT - 1);
		printf("\nConnection built: %d %d\n", a, b);
		poolsToConnect.emplace_back(std::make_pair(a, b));
	}
	printf("\n");

	//запуск многопоточной части
	for(int iter = 0; iter < CONNECT_THREADS; ++iter){commands.emplace_back(std::thread(&Interface::_connectPools, 
				this, &poolsToConnect));}
	for(int iter = 0; iter < CONNECT_THREADS; ++iter){commands[iter].join();}
	commands.clear();

	//еще одна косметическая печать
	printf("After connection:\n");
	for(iPool* curPool : pools){printf("Pool %d: %Lf liters\n", curPool->getPos(), curPool->getMeasure());}
}

void Interface::addWater()
{
	//добавление воды ADDITION_COUNT раз
	std::vector<std::pair<iPool*, long double>> poolsToAdd;
	std::vector<std::thread> commands;
	for(int iter = 0; iter < ADDITION_COUNT; ++iter)
	{
		poolsToAdd.emplace_back(std::make_pair(pools[_intRand(0, POOL_COUNT - 1)], _intRand(R_MIN+100, R_MAX+100)));
	}

	//запуск многопоточной части
        for(int iter = 0; iter < ADD_THREADS; ++iter){commands.emplace_back(std::thread(&Interface::_addToPools, this, &poolsToAdd));};
        for(int iter = 0; iter < ADD_THREADS; ++iter){commands[iter].join();}
        commands.clear();

	//косметическая печать
        printf("\nAfter all the additions:\n");
        for(iPool* curPool : pools){printf("Pool %d: %Lf liters\n", curPool->getPos(), curPool->getMeasure());}
}
