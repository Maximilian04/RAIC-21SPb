#ifndef MYSTRATEGY_CPP_FLYING_CONTROLLER
#define MYSTRATEGY_CPP_FLYING_CONTROLLER

#include "model/Game.hpp"
#include "model/Action.hpp"
#include <vector>
#include <utility>
#include <set>
#include <random>
#include <algorithm>
#include <chrono>
#include <map>
//#include <functional>
#include <iostream>

#include "FlyingGroup.h"

using namespace std;

const int INF = 100000;
class FlyingController
{
public:
    // Distances in format d[i][j] = correct flying distance between planets i and j
    vector<vector<int>> d;
    // Adjacent planets in format adj[i] = all planets adjacent with planet i
    vector<vector<int>> adj;

    vector<FlyingGroup> groups;

    FlyingController();
    void setup(vector<vector<int>> distances, vector<vector<int>> adjacent);
    vector<int> findPath(FlyingGroup group);
    int onFlightAt(int planet);
    void send(int fr, int to, int num, optional<model::Resource> res);
    vector<model::MoveAction> update();
};

#endif //MYSTRATEGY_CPP_FLYING_CONTROLLER