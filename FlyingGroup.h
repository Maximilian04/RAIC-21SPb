#ifndef MYSTRATEGY_CPP_FLYING_GROUP
#define MYSTRATEGY_CPP_FLYING_GROUP

#include "model/Game.hpp"
#include "model/Action.hpp"
#include <vector>
#include <utility>
#include <set>
#include <random>
#include <algorithm>
#include <chrono>
#include <map>
#include <iostream>

using namespace std;

#define IGNORANCE 0
#define AVOIDANCE 1
#define AVOIDANCE_WITH_ATTACK 2


class FlyingGroup {
public:
    int timeToNext;
    int timeFromPrev;

    // Departure and destination planets
    int fr, to;
    // Number of wrokers in group
    int num;
    // Safety type
    int safety;
    // Resource to carry
    optional<model::Resource> res;
    // Is trip for group finished
    bool isFinished;
    // Array of the planets along the way
    vector<int> path;

    FlyingGroup(int fr, int to, int num, optional<model::Resource> res, int safety=IGNORANCE);

    void setPath(vector<int> path, vector<vector<int>> &d);

    optional<model::MoveAction> step(vector<vector<int>> &d);
};

#endif //MYSTRATEGY_CPP_FLYING_GROUP