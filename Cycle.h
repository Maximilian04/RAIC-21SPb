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

#include "Terms.h"

using namespace std;

class Cycle {
public:
	vector<int> buildingPlanet; // MINES -> REPLICATOR + EXTRAFOUNDRY
	vector<bool> orderedPlanet; // MINES -> REPLICATOR + EXTRAFOUNDRY
	vector<bool> stackedPlanet; // MINES -> REPLICATOR+ EXTRAFOUNDRY
	set<int> usedPlanets;
	bool isBuilt;
	bool isPlanned;

	float prodFactor;

	bool sendRobots(const model::Game& game, vector<model::MoveAction>& moveActions, int planet, int resource,
					int capacity, vector<pair<int, float>> plKRes, vector<pair<int, float>> plKEmpty,
					bool protectStuck = false);

	Cycle();
};


#endif //MYSTRATEGY_CPP_CYCLE_H
