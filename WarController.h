#ifndef MYSTRATEGY_CPP_WAR_CONTROLLER
#define MYSTRATEGY_CPP_WAR_CONTROLLER

#include "model/Game.hpp"
#include "model/Action.hpp"

#include "Observer.h"
#include "FlyingController.h"

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

#define LOWEST_IMPORTANCE 10

struct Target
{
    int number;
    int importance;

    Target(int number, int importance)
    {
        this->number = number;
        this->importance = importance;
    }
};

class WarController
{
public:
    vector<Target> targets;

    void setTarget(int i, Target newTarget);
    void update(const model::Game& game, FlyingController& fc, Observer& observer);
};

#endif // MYSTRATEGY_CPP_WAR_CONTROLLER