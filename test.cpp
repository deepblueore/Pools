#include "pool.hpp"
#include "graph.hpp"

int main()
{
	Graph pools;
	pools.makePipes();
	pools.getMeasures();
	pools.addWater();
	pools.getMeasures();
}
