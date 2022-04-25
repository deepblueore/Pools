#include "pool.hpp"

Pool::Pool()
{
	_ptrPipes = new unsigned int[PALLOC_SIZE];
}

Pool::~Pool()
{
	delete[] _ptrPipes;
}

double Pool::getMeas() const{return _measure;}
unsigned int Pool::getPos() const{return _position;}
unsigned int* Pool::getPipes() const{return _ptrPipes;}
unsigned int Pool::getSize() const{return _pipesSize;}

void Pool::setMeas(const double& newMeasure){_measure = newMeasure;}
void Pool::setPos(const unsigned int& newPosition){_position = newPosition;}

void Pool::pushPipe(const unsigned int& newPipe)
{
	++_pipesSize;
	if(_pipesSize > _pipesCapacity)
	{
		unsigned int* newPipes = new unsigned int[_pipesCapacity * 2];
		std::memcpy(newPipes, _ptrPipes, sizeof(unsigned int) * _pipesCapacity);
		newPipes[_pipesSize - 1] = newPipe;
		_pipesCapacity *= 2;
		
		delete[] _ptrPipes;
		_ptrPipes = newPipes;
	}
	else
	{
		_ptrPipes[_pipesSize - 1] = newPipe;
	}
}

void Pool::addMeas(const double& newMeasure){_measure += newMeasure;}
