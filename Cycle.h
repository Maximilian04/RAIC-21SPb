//
// Created by Maximilian on 09.10.2021.
//

#ifndef MYSTRATEGY_CPP_CYCLE_H
#define MYSTRATEGY_CPP_CYCLE_H

#define CYCLE_BUILD_NUM 10

#include "model/Game.hpp"
#include "model/Action.hpp"
#include <vector>
#include <utility>
#include <set>
#include <random>
#include <algorithm>
#include <chrono>
#include <map>
#include <functional>

#include "Terms.h"
#include "FlyingController.h"

using namespace std;

class Cycle {
public:
	vector<int> buildingPlanet; // MINES -> REPLICATOR + EXTRAFOUNDRY
	vector<bool> orderedPlanet; // MINES -> REPLICATOR + EXTRAFOUNDRY
	vector<bool> stackedPlanet; // MINES -> REPLICATOR+ EXTRAFOUNDRY
	set<int> usedPlanets;
	bool isBuilt;
	bool isPlanned;

	vector<vector<int>> trafficCoeff;

	float prodFactor;

	bool sendRobots(const model::Game& game, FlyingController &fc, int planet, int resource,
					int capacity, vector<pair<int, float>> plKRes, vector<pair<int, float>> plKEmpty,
					int batchSize, bool protectStuck = false);

	void planBuilding(const model::Game& game, const int& homePlanet, const vector<int>& enemyHomePlanets,
					  const vector<vector<int>>& planetDists);

	Cycle();

private:
	int buildeff(const vector<vector<int>>& planetDists, const vector<int>& candidates);

	bool onMySide(const vector<vector<int>>& planetDists, const int& homePlanet, const vector<int>& enemyHomePlanets,
				  int id);
};


#endif //MYSTRATEGY_CPP_CYCLE_H
