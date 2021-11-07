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

    vector<int> enemies;


    Observer();

    void update(const model::Game& game, vector<vector<int>>& d);

    bool isOurs(int planet);

    int size();
};

#endif //MYSTRATEGY_CPP_OBSERVER