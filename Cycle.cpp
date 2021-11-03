//
// Created by Maximilian on 09.10.2021.
//

#include "Cycle.h"

Cycle::Cycle() : buildingPlanet(CYCLE_BUILD_NUM, -1), isBuilt(false),
				 orderedPlanet(CYCLE_BUILD_NUM, false), isPlanned(false),
				 stackedPlanet(CYCLE_BUILD_NUM, false), prodFactor(1) {
	for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) buildcoeff[i][j] = 0;
		}

	buildcoeff[MINES][FOUNDRY] = 8;
	buildcoeff[FOUNDRY][MINES] = 4;

	buildcoeff[MINES][EXTRAFOUNDRY] = 8;
	buildcoeff[EXTRAFOUNDRY][MINES] = 4;

	buildcoeff[FOUNDRY][CHIP_FACTORY] = 2;
	buildcoeff[EXTRAFOUNDRY][CHIP_FACTORY] = 2;
	buildcoeff[CHIP_FACTORY][MINES] = 3;
	buildcoeff[CHIP_FACTORY][CAREER] = 3;

	buildcoeff[FOUNDRY][ACCUMULATOR_FACTORY] = 1;
	buildcoeff[EXTRAFOUNDRY][ACCUMULATOR_FACTORY] = 1;
	buildcoeff[ACCUMULATOR_FACTORY][MINES] = 2;
	buildcoeff[ACCUMULATOR_FACTORY][FARM] = 1;

	buildcoeff[FOUNDRY][REPLICATOR] = 1;
	buildcoeff[EXTRAFOUNDRY][REPLICATOR] = 1;
	buildcoeff[CHIP_FACTORY][REPLICATOR] = 2;
	buildcoeff[ACCUMULATOR_FACTORY][REPLICATOR] = 1;
	buildcoeff[REPLICATOR][MINES] = 3;
	buildcoeff[REPLICATOR][CAREER] = 1;
	buildcoeff[REPLICATOR][FARM] = 1;

	buildcoeff[CAREER][FURNACE] = 8;
	buildcoeff[FURNACE][CAREER] = 4;

	buildcoeff[FURNACE][CHIP_FACTORY] = 4;

	buildcoeff[FARM][BIOREACTOR] = 4;
	buildcoeff[BIOREACTOR][FARM] = 2;

	buildcoeff[BIOREACTOR][ACCUMULATOR_FACTORY] = 2;
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
	int freeReses = 0;
	if (resource != -1) {
		freeReses = game.planets[planet].resources.count(t2r(resource)) ?
					game.planets[planet].resources.at(t2r(resource)) : 0;
		if (resource == METAL)
			freeRobots = min(freeRobots, (int) ((sumKRes + sumKEmpty) / sumKRes * freeReses));
	}
	int leftRobots = freeRobots;

	static vector<vector<float>> shortageRobots(game.planets.size(), vector<float>(game.planets.size(), 0));
	for (pair<int, float> plK: plKRes) {
		if (shortageRobots[planet][plK.first] > 1 && leftRobots > 0 && freeReses > 0) {
			moveActions.push_back(model::MoveAction(planet, plK.first,
													min(min(leftRobots, (int) shortageRobots[planet][plK.first]),
														freeReses),
													optional<model::Resource>(t2r(resource))));
			leftRobots -= min(min(leftRobots, (int) shortageRobots[planet][plK.first]),
							  freeReses);
			shortageRobots[planet][plK.first] -= min(freeRobots, (int) shortageRobots[planet][plK.first]);
		}
	}
	for (pair<int, float> plK: plKEmpty) {
		if (shortageRobots[planet][plK.first] > 1  && leftRobots > 0) {
			moveActions.push_back(model::MoveAction(planet, plK.first,
													min(leftRobots, (int) shortageRobots[planet][plK.first]),
													optional<model::Resource>()));
			leftRobots -= min(freeRobots, (int) shortageRobots[planet][plK.first]);
			shortageRobots[planet][plK.first] -= min(freeRobots, (int) shortageRobots[planet][plK.first]);
		}
	}

	if (freeRobots < batchSize && !protectStuck) return false;
	for (pair<int, float> plK: plKRes) {
		moveActions.push_back(model::MoveAction(planet, plK.first, (int) (plK.second * freeRobots),
												optional<model::Resource>(t2r(resource))));
		leftRobots -= (int) (plK.second * freeRobots);
		shortageRobots[planet][plK.first] += plK.second * freeRobots - ((int) (plK.second * freeRobots));
	}
	for (pair<int, float> plK: plKEmpty) {
		moveActions.push_back(model::MoveAction(planet, plK.first, (int) (plK.second * freeRobots),
												optional<model::Resource>()));
		leftRobots -= (int) (plK.second * freeRobots);
		shortageRobots[planet][plK.first] += plK.second * freeRobots - ((int) (plK.second * freeRobots));
	}
	if (leftRobots > 0) {
		//lastAct.workerNumber = leftRobots;
		moveActions.rbegin()->workerNumber += leftRobots;
	}
	return true;
}