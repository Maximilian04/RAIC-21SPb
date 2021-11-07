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
#include <iostream>
#include <queue>

#include "FlyingGroup.h"
#include "Observer.h"

using namespace std;

const int INF = 100000;

class FlyingController {
public:
	// Distances in format d[i][j] = correct flying distance between planets i and j
	vector<vector<int>> d;
    // Adjacent planets in format adj[i] = all planets adjacent with planet i
    vector<vector<int>> adj;
    // Adjacent planets without enemies in format adj[i] = all planets adjacent with planet i
    vector<vector<int>> safeAdj;
    // All groups under control
    vector<FlyingGroup> groups;

    Observer* observer;

    FlyingController();

    void setup(vector<vector<int>> distances, Observer* observer);

    vector<int> findPathDijkstra(FlyingGroup group);

    vector<int> findPath(FlyingGroup group);

    int onFlightAt(int planet);
    int onFlightTo(int planet);

    void send(int fr, int to, int num, optional<model::Resource> res, int safetyMode=AVOIDANCE);

    vector<model::MoveAction> update();
    void updateAdj(const model::Game& game, int maxTravelDistance);
    void updateSafeAdj(const model::Game& game, int maxTravelDistance);
};

#endif //MYSTRATEGY_CPP_FLYING_CONTROLLER