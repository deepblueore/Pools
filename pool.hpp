#pragma once

#include <cstring>
#include <cstdio>

#define PALLOC_SIZE 50

class Pool
{
	private:
		double _measure = 0; //объем воды
		unsigned int _position; //позиция в массиве бассейнов (для быстрой работы списка смежности)
		unsigned int* _ptrPipes{NULL}; //позиции бассейнов, с которыми есть труба
		unsigned int _pipesSize = 0; //фактический размер _pipes[]
		unsigned int _pipesCapacity = PALLOC_SIZE; //емкость pipes[]
	public:
		Pool();
		~Pool();

		//геттеры
		double getMeas() const;
		unsigned int getPos() const;
		unsigned int* getPipes() const;
		unsigned int getSize() const;

		//сеттеры
		void setMeas(const double& newMeasure);
		void setPos(const unsigned int& newPosition);

		//добавления
		void pushPipe(const unsigned int& newPipe);
		void addMeas(const double& newMeasure);
};
