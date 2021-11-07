#ifndef MYSTRATEGY_CPP_WAR_CONTROLLER
#define MYSTRATEGY_CPP_WAR_CONTROLLER

#include "model/Game.hpp"
#include "model/Action.hpp"

#include "Observer.h"
#include "FlyingController.h"

#include <vector>
#include <iostream>

using namespace std;

struct Target
{
    int planet;
    int number;

    int importance;
};

class WarController
{
public:
    vector<Target> targets;

    void update(const model::Game& game, FlyingController& fc, Observer& observer);
};

#endif // MYSTRATEGY_CPP_WAR_CONTROLLER