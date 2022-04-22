#pragma once

#include <vector>
#include <mutex>
#include <thread>
#include <iostream>
#include <exception>
#include <map>
#include <utility>
#include <cstdio>

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
extern std::mutex poolsMutex;
