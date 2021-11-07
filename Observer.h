#ifndef MYSTRATEGY_CPP_OBSERVER
#define MYSTRATEGY_CPP_OBSERVER

#include "model/Game.hpp"
#include "model/Action.hpp"
#include "model/FlyingWorkerGroup.hpp"

#include <vector>
#include <iostream>

using namespace std;

const int OBSERVER_PERIOD = 40;

class Observer
{
public:
    vector<double> traffic;
    vector<double> bottleneckTraffic;
    vector<int> bottleneckTrafficPeak;

    vector<vector<int>> bottleneckTrafficTimed;

    // Workers nums per planet in format prop[planet] = number of workers of cetrain type at planet
    vector<int> enemies;
    vector<int> allies;
    vector<int> ours;
  
    vector<int> alliesList;

    vector<int> enemyFlyersTo;

    void setup(const model::Game& game);

    void update(const model::Game& game, vector<vector<int>>& d);

    bool isOurs(int planet);

    int size();

    int isAlly(int playerIndex)
    {
        for (int index : alliesList)
            if (index == playerIndex)
                return true;
        return false;
    }

    int isEnemy(int playerIndex)
    {
        return !isAlly(playerIndex);
    }
};

#endif //MYSTRATEGY_CPP_OBSERVER