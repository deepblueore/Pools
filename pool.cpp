#include "pool.hpp"

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
