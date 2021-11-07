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
    vector<vector<int>> bottleneckTrafficTimed;

    vector<int> enemies;

    Observer();

    void update(const model::Game& game, vector<vector<int>>& d);

    // TODO: use flying groups to predict enemies number on planet (btw doesn`t really matter)
    bool isOurs(int planet);

    int size();
};

#endif //MYSTRATEGY_CPP_OBSERVER