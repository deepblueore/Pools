#include "pool.hpp"

iPool::iPool()
{
        _position = pools.size();
        pConnects.insert(std::make_pair(_position, std::vector<iPool*>{}));
}

long double iPool::getMeasure() const{return _measure;}

int iPool::getPos() const
{
        if(_position == -1){throw std::runtime_error("position undefined!");}
        return _position;
}

void iPool::add(const long double water)
{
        poolsMutex.lock();
        std::vector<bool> isUsed{};
        for(int iter = 0; iter < pools.size(); ++iter){isUsed.push_back(false);}
        std::vector<iPool*> poolsComp{};
        poolsComp.push_back(pools[_position]);
        dfs(poolsComp, isUsed);

        long double newWater = water / poolsComp.size();
        for(iPool* curPool : poolsComp){printf("%d ", curPool->getPos());}
        printf("\n");
        for(iPool* curPool : poolsComp){curPool->setMeasure(curPool->getMeasure() + newWater);}
        for(int iter = 0; iter < 5; ++iter){std::cout << pools[iter]->getPos() << " " << pools[iter]->getMeasure() << std::endl;}
        std::cout << std::endl;
        poolsMutex.unlock();
}

void iPool::connect(iPool* pool)
{
        long double connectedMeasure = 0;
        std::map<int, std::vector<iPool*>>::iterator mapIter{NULL};

        //Далее обновляется список смежности
        poolsMutex.lock();
        mapIter = pConnects.find(_position);
        mapIter->second.push_back(pool);
        mapIter = pConnects.find(pool->getPos());
        mapIter->second.push_back(pools[_position]);

        //далее dfs собирает для данного бассейна компоненту связности
        std::vector<bool> isUsed{};
        for(int iter = 0; iter < pools.size(); ++iter){isUsed.push_back(false);}
        std::vector<iPool*> poolsComp{};
        poolsComp.push_back(pools[_position]);
        dfs(poolsComp, isUsed);

        //суммарный объем компоненты связности собирается в connectedMeasure
        for(iPool* curPool : poolsComp){connectedMeasure += curPool->getMeasure();}

        //накопленный объем разделяется поровну
        long double newMeasure = connectedMeasure / poolsComp.size();
        printf("%Lf %Lf\n", connectedMeasure, newMeasure);
        for(iPool* curPool : poolsComp){printf("%d ", curPool->getPos());}
        printf("\n");
        for(iPool* curPool : poolsComp){curPool->setMeasure(newMeasure);}
        for(int iter = 0; iter < 5; ++iter){std::cout << pools[iter]->getPos() << " " << pools[iter]->getMeasure() << std::endl;}
        poolsMutex.unlock();
}

void iPool::dfs(std::vector<iPool*>& poolsComp, std::vector<bool>& isUsed)
{
        isUsed[_position] = true;
        std::map<int, std::vector<iPool*>>::iterator dfsIter{NULL};
        dfsIter = pConnects.find(_position);
        for(iPool* curPool : dfsIter->second)
        {
                if(isUsed[curPool->getPos()]){continue;}
                poolsComp.push_back(curPool);
                curPool->dfs(poolsComp, isUsed);
        }
}

void iPool::setMeasure(const long double newMeasure){_measure = newMeasure;}
