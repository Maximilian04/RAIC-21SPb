//
// Created by Maximilian on 09.10.2021.
//

#include "Cycle.h"

Cycle::Cycle() : buildingPlanet(CYCLE_BUILD_NUM, -1), isBuilt(false),
				 orderedPlanet(CYCLE_BUILD_NUM, false), isPlanned(false),
				 stackedPlanet(CYCLE_BUILD_NUM, false), prodFactor(1) {
}

bool Cycle::sendRobots(const model::Game& game, vector<model::MoveAction>& moveActions, int planet, int resource,
					   int capacity, vector<pair<int, float>> plKRes, vector<pair<int, float>> plKEmpty,
					   int batchSize, bool protectStuck) {
	float sumKRes = 0, sumKEmpty = 0;
	for (pair<int, float> plK: plKRes) {
		sumKRes += plK.second;
	}
	for (pair<int, float> plK: plKEmpty) {
		sumKEmpty += plK.second;
	}

	int freeRobots = max(0, (int) (game.planets[planet].workerGroups[0].number - this->prodFactor * capacity));
	if (resource != -1) {
		int freeReses = game.planets[planet].resources.count(t2r(resource)) ?
						game.planets[planet].resources.at(t2r(resource)) : 0;
		if (resource == METAL)
			freeRobots = min(freeRobots, (int) (/*(sumKRes + sumKEmpty) / sumKRes*/2 * freeReses));
	}

	if (freeRobots < batchSize/*16 * 3*/ && !protectStuck) return false;
	//!!!this->stackedPlanet[FOUNDRY] = false;
	int leftRobots = freeRobots;
	model::MoveAction lastAct(0, 0, 0, optional<model::Resource>());
	for (pair<int, float> plK: plKEmpty) {
		lastAct = model::MoveAction(planet, plK.first, (int)(plK.second * freeRobots),
									optional<model::Resource>());
		moveActions.push_back(lastAct);
		leftRobots -= (int)(plK.second * freeRobots);
	}
	for (pair<int, float> plK: plKRes) {
		lastAct = model::MoveAction(planet, plK.first, (int)(plK.second * freeRobots),
									optional<model::Resource>(t2r(resource)));
		moveActions.push_back(lastAct);
		leftRobots -= (int)(plK.second * freeRobots);
	}
	if (leftRobots > 0) {
		//lastAct.workerNumber = leftRobots;
		moveActions.rbegin()->workerNumber += leftRobots;
	}
	return true;
}