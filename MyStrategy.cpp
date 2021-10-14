#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() : homePlanet(-1) {}

model::Action MyStrategy::getAction(const model::Game& game) {
	if (homePlanet == -1) init(game);
	separatePlanets(game);

	vector<model::MoveAction> moveActions;
	vector<model::BuildingAction> buildActions;

	if (!prodCycle.isPlanned) {
		for (int id = 0; id < game.planets.size(); ++id) { // перебираем планеты с ресурсами
			if (game.planets[id].harvestableResource.has_value() &&
				game.planets[id].harvestableResource.value() != t2r(STONE)) {
				int resource = r2t(game.planets[id].harvestableResource.value());

				if (prodCycle.buildingPlanet[resource] == -1 ||
					planetDists[prodCycle.buildingPlanet[resource]][homePlanet] >
					planetDists[id][homePlanet]) {

					prodCycle.buildingPlanet[resource] = id;
				}
			}
		}
		/*prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[ORE]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[SAND]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[ORGANICS]);*/

		// перебираем планеты для зданий
		for (int id = 0; id < game.planets.size(); ++id) {
			if (id != homePlanet &&
				id != prodCycle.buildingPlanet[ORE] &&
				id != prodCycle.buildingPlanet[SAND] &&
				id != prodCycle.buildingPlanet[ORGANICS]) {
				if (prodCycle.usedPlanets.size() < (CYCLE_BUILD_NUM - 3)) {
					prodCycle.usedPlanets.emplace(id);
				} else {
					int minD = -1;
					int minI;
					for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
						if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[ORE]] +
										   planetDists[*planet][prodCycle.buildingPlanet[SAND]] +
										   planetDists[*planet][prodCycle.buildingPlanet[ORGANICS]]) > minD) {
							minD = planetDists[*planet][prodCycle.buildingPlanet[ORE]] +
								   planetDists[*planet][prodCycle.buildingPlanet[SAND]] +
								   planetDists[*planet][prodCycle.buildingPlanet[ORGANICS]];
							minI = *planet;
						}
					}
					if ((planetDists[id][prodCycle.buildingPlanet[ORE]] +
						 planetDists[id][prodCycle.buildingPlanet[SAND]] +
						 planetDists[id][prodCycle.buildingPlanet[ORGANICS]]) < minD) {
						prodCycle.usedPlanets.erase(minI);
						prodCycle.usedPlanets.emplace(id);
					}
				}
			}
		}

		int minD = -1;
		int minI;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[MINES]]) > minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[MINES]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[FOUNDRY] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[CAREER]]) > minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[CAREER]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[FURNACE] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FARM]]) > minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FARM]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[BIOREACTOR] = minI;
		prodCycle.usedPlanets.erase(minI);


		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]]) > minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[CHIP_FACTORY] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]]) > minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[ACCUMULATOR] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]]) > minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[REPLICATOR] = minI;
		prodCycle.usedPlanets.erase(minI);


		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[MINES]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[CAREER]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[FARM]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[FOUNDRY]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[FURNACE]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[BIOREACTOR]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[CHIP_FACTORY]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[ACCUMULATOR_FACTORY]);
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[REPLICATOR]);

		cout << prodCycle.buildingPlanet[MINES] << endl;
		cout << prodCycle.buildingPlanet[CAREER] << endl;
		cout << prodCycle.buildingPlanet[FARM] << endl;
		cout << prodCycle.buildingPlanet[FOUNDRY] << endl;
		cout << prodCycle.buildingPlanet[FURNACE] << endl;
		cout << prodCycle.buildingPlanet[BIOREACTOR] << endl;
		cout << prodCycle.buildingPlanet[CHIP_FACTORY] << endl;
		cout << prodCycle.buildingPlanet[ACCUMULATOR_FACTORY] << endl;
		cout << prodCycle.buildingPlanet[REPLICATOR] << endl;

		prodCycle.isPlanned = true;
	} else if (!prodCycle.isBuilt) {
		int freeStone = min(game.planets[homePlanet].resources.count(t2r(STONE)) ?
							game.planets[homePlanet].resources.at(t2r(STONE)) : 0,
							!game.planets[homePlanet].workerGroups.empty() ?
							game.planets[homePlanet].workerGroups[0].number : 0);
		for (int building = 0; building < CYCLE_BUILD_NUM; ++building) {
			if (game.planets[prodCycle.buildingPlanet[building]].building.has_value()) continue;
			if (prodCycle.orderedPlanet[building]) {
				buildActions.push_back(model::BuildingAction(prodCycle.buildingPlanet[building],
															 optional<model::BuildingType>(t2b(building))));
			} else {
				if (freeStone < stoneCost(building)) continue;

				moveActions.push_back(model::MoveAction(homePlanet, prodCycle.buildingPlanet[building],
														stoneCost(building), optional<model::Resource>(t2r(STONE))));
				freeStone -= stoneCost(building);
				prodCycle.orderedPlanet[building] = true;
			}
		}
	} else {

	}

	return model::Action(moveActions, buildActions);
}

void MyStrategy::init(const model::Game& game) {
	for (int i = 0; i < game.planets.size(); ++i) {
		if (!game.planets[i].workerGroups.empty() &&
			game.planets[i].workerGroups[0].playerIndex == game.myIndex) {
			homePlanet = i;
			break;
		}
	}

	planetDists = vector<vector<int>>(game.planets.size(), vector<int>(game.planets.size(), 0));
	int count = game.planets.size();
	while (count--) {
		for (int i = 0; i < game.planets.size(); ++i) {
			for (int j = 0; j < game.planets.size(); ++j) {
				if (i == j) continue;
				if ((abs(game.planets[i].x - game.planets[j].x) +
					 abs(game.planets[i].y - game.planets[j].y) <= game.maxTravelDistance)) {
					planetDists[i][j] = planetDists[j][i] = abs(game.planets[i].x - game.planets[j].x) +
															abs(game.planets[i].y - game.planets[j].y);

					for (int m = 0; m < game.planets.size(); ++m) {
						if (m == i || m == j) continue;
						if (planetDists[m][i] != 0 &&
							(planetDists[m][j] == 0 || planetDists[m][j] > (planetDists[m][i] + planetDists[i][j]))) {
							planetDists[m][j] = planetDists[j][m] = planetDists[m][i] + planetDists[i][j];
						}
						if (planetDists[m][j] != 0 &&
							(planetDists[m][i] == 0 || planetDists[m][i] > (planetDists[m][j] + planetDists[i][j]))) {
							planetDists[m][i] = planetDists[i][m] = planetDists[m][j] + planetDists[i][j];
						}
					}
				}
			}
		}
	}

	/*for (int i = 0; i < game.planets.size(); ++i) {
		for (int j = 0; j < game.planets.size(); ++j) {
			cout << planetDists[i][j] << " ";
		}
		cout << endl;
	}*/

	prodCycle.buildingPlanet = vector<int>(9, -1);
}

void MyStrategy::separatePlanets(const model::Game& game) { // generating list of planets
	//TODO change to true zone separating
	if (planetInf.empty()) {
		planetInf = vector<Planet>(game.planets.size());
	}

	for (int id = 0; id < game.planets.size(); ++id) {
		if (game.planets[id].workerGroups.empty() || game.planets[id].workerGroups[0].playerIndex == game.myIndex) {
			planetInf[id].planetOwner = 1;
		} else {
			planetInf[id].planetOwner = 0;
		}
	}
}