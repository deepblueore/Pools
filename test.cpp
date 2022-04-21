#include "pool.hpp"

std::vector<iPool*> pools;
std::map<int, std::vector<iPool*>> pConnects;
std::mutex poolsMutex;

int main()
{
        for(int iter = 0; iter < 5; ++iter){pools.push_back(new iPool());}
        std::vector<std::thread> commands;

        for(int iter = 0; iter < 3; ++iter){commands.emplace_back(std::thread(&iPool::add, pools[iter], 20));}
        commands.emplace_back(std::thread(&iPool::add, pools[3], 60));
        commands.emplace_back(std::thread(&iPool::add, pools[4], 100));
        for(int iter = 0; iter < 5; ++iter){commands[iter].join();}
        commands.clear();

        commands.emplace_back(std::thread(&iPool::connect, pools[2], pools[3]));
        commands.emplace_back(std::thread(&iPool::connect, pools[3], pools[4]));
        commands.emplace_back(std::thread(&iPool::add, pools[0], 20));
        commands.emplace_back(std::thread(&iPool::add, pools[2], 30));
        commands.emplace_back(std::thread(&iPool::connect, pools[0], pools[1]));
        commands.emplace_back(std::thread(&iPool::connect, pools[0], pools[2]));

        for(int iter = 0; iter < commands.size(); ++iter){commands[iter].join();}
        //for(int iter = 0; iter < 5; ++iter){std::cout << pools[iter].getPos() << " " << pools[iter].getMeasure() << std::endl;}
}

