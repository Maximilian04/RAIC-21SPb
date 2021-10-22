#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() : homePlanet(-1), resetTimer(0) {}

model::Action MyStrategy::getAction(const model::Game& game) {
	if (homePlanet == -1) init(game);
	separatePlanets(game);
	++resetTimer;

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
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[MINES]]) < minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[MINES]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[FOUNDRY] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]]) < minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[EXTRAFOUNDRY] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[CAREER]]) < minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[CAREER]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[FURNACE] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FARM]]) < minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FARM]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[BIOREACTOR] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]]) < minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[CHIP_FACTORY] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]]) < minD) {
				minD = planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]];
				minI = *planet;
			}
		}
		prodCycle.buildingPlanet[ACCUMULATOR] = minI;
		prodCycle.usedPlanets.erase(minI);

		minD = -1;
		for (auto planet = prodCycle.usedPlanets.begin(); planet != prodCycle.usedPlanets.end(); ++planet) {
			if (minD == -1 || (planetDists[*planet][prodCycle.buildingPlanet[FOUNDRY]]) < minD) {
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
		prodCycle.usedPlanets.emplace(prodCycle.buildingPlanet[EXTRAFOUNDRY]);

		cout << prodCycle.buildingPlanet[MINES] << endl;
		cout << prodCycle.buildingPlanet[CAREER] << endl;
		cout << prodCycle.buildingPlanet[FARM] << endl;
		cout << prodCycle.buildingPlanet[FOUNDRY] << endl;
		cout << prodCycle.buildingPlanet[FURNACE] << endl;
		cout << prodCycle.buildingPlanet[BIOREACTOR] << endl;
		cout << prodCycle.buildingPlanet[CHIP_FACTORY] << endl;
		cout << prodCycle.buildingPlanet[ACCUMULATOR_FACTORY] << endl;
		cout << prodCycle.buildingPlanet[REPLICATOR] << endl;
		cout << prodCycle.buildingPlanet[EXTRAFOUNDRY] << endl;

		prodCycle.isPlanned = true;
	} else if (!prodCycle.isBuilt) {
		int freeStone = min(game.planets[homePlanet].resources.count(t2r(STONE)) ?
							game.planets[homePlanet].resources.at(t2r(STONE)) : 0,
							!game.planets[homePlanet].workerGroups.empty() ?
							game.planets[homePlanet].workerGroups[0].number : 0);
		bool hasMissedBuild = false;
		for (int building = 0; building < CYCLE_BUILD_NUM; ++building) {
			if (game.planets[prodCycle.buildingPlanet[building]].building.has_value()) continue;
			hasMissedBuild = true;
			if (prodCycle.orderedPlanet[building]) {
				buildActions.push_back(model::BuildingAction(prodCycle.buildingPlanet[building],
															 optional<model::BuildingType>(
																	 t2b(building == EXTRAFOUNDRY ? FOUNDRY
																								  : building))));
			} else {
				if (freeStone < stoneCost(building == EXTRAFOUNDRY ? FOUNDRY : building)) continue;

				moveActions.push_back(model::MoveAction(homePlanet, prodCycle.buildingPlanet[building],
														stoneCost(building == EXTRAFOUNDRY ? FOUNDRY : building),
														optional<model::Resource>(t2r(STONE))));
				freeStone -= stoneCost(building);
				prodCycle.orderedPlanet[building] = true;
			}
		}
		if (!hasMissedBuild) {
			prodCycle.isBuilt = true;
			prodCycle.prodFactor = 2.5;
			cout << "работаем" << endl;
		}
	} else {
		if (resetTimer > 100) {
			for (int building = 3; building < prodCycle.stackedPlanet.size(); ++building) {
				if (prodCycle.stackedPlanet[building]) {
					if (game.planets[prodCycle.buildingPlanet[building]].workerGroups.empty() ||
						game.planets[prodCycle.buildingPlanet[building]].workerGroups[0].playerIndex != game.myIndex) {
						continue;
					}
					int freeRobots = game.planets[prodCycle.buildingPlanet[building]].workerGroups[0].number;
					moveActions.push_back(
							model::MoveAction(prodCycle.buildingPlanet[building], prodCycle.buildingPlanet[CAREER],
											  freeRobots / 3,
											  optional<model::Resource>()));
					moveActions.push_back(
							model::MoveAction(prodCycle.buildingPlanet[building], prodCycle.buildingPlanet[FARM],
											  freeRobots / 3,
											  optional<model::Resource>()));
					moveActions.push_back(
							model::MoveAction(prodCycle.buildingPlanet[building], prodCycle.buildingPlanet[MINES],
											  freeRobots -
											  2 * freeRobots / 3,
											  optional<model::Resource>()));
				}
			}

			resetTimer = 0;
			prodCycle.stackedPlanet = vector<bool>(prodCycle.stackedPlanet.size(), true);
		}
		for (int id = 0; id < game.planets.size(); ++id) {
			if (!game.planets[id].workerGroups.empty() &&
				game.planets[id].workerGroups[0].playerIndex == game.myIndex) {

				if (id == prodCycle.buildingPlanet[MINES]) {
					int freeRobots = max(0,
										 (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 32));
					if (freeRobots < 12 * 3) continue;
					prodCycle.stackedPlanet[MINES] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FOUNDRY],
															freeRobots,
															optional<model::Resource>(t2r(ORE))));


				} else if (id == prodCycle.buildingPlanet[CAREER]) {
					int freeRobots = max(0,
										 (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 16));
					if (freeRobots < 12 * 3) continue;
					prodCycle.stackedPlanet[CAREER] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FURNACE],
															freeRobots,
															optional<model::Resource>(t2r(SAND))));


				} else if (id == prodCycle.buildingPlanet[FARM]) {
					int freeRobots = max(0,
										 (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 8));
					if (freeRobots < 6 * 3) continue;
					prodCycle.stackedPlanet[FARM] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[BIOREACTOR],
															freeRobots,
															optional<model::Resource>(t2r(ORGANICS))));


				} else if (id == prodCycle.buildingPlanet[FOUNDRY]) {
					int freeRobots = max(0, (int) (game.planets[id].workerGroups[0].number -
												   prodCycle.prodFactor * 16 / 2));
					int freeReses = game.planets[id].resources.count(t2r(METAL)) ?
									game.planets[id].resources.at(t2r(METAL)) : 0;
					// ----- EXTRAFOUNDRY -----
					int freeOre = game.planets[id].resources.count(t2r(ORE)) ?
								  game.planets[id].resources.at(t2r(ORE)) : 0;
					int freeOreExtra = game.planets[prodCycle.buildingPlanet[EXTRAFOUNDRY]].resources.count(t2r(ORE)) ?
									   game.planets[prodCycle.buildingPlanet[EXTRAFOUNDRY]].resources.at(t2r(ORE)) : 0;
					int sentOre = max((freeOre - freeOreExtra) / 2, 0);
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[EXTRAFOUNDRY],
															min(freeRobots, sentOre),
															optional<model::Resource>(t2r(ORE))));
					freeRobots = max(freeRobots - sentOre, 0);
					// ----- EXTRAFOUNDRY -----
					freeRobots = min(freeRobots, 2 * freeReses);
					if (freeRobots < 16 * 3) continue;
					prodCycle.stackedPlanet[FOUNDRY] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[MINES],
															freeRobots / 2,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[CHIP_FACTORY],
															freeRobots / 4,
															optional<model::Resource>(t2r(METAL))));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[ACCUMULATOR_FACTORY],
															freeRobots / 8,
															optional<model::Resource>(t2r(METAL))));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[REPLICATOR],
															freeRobots -
															(freeRobots / 2 + freeRobots / 4 + freeRobots / 8),
															optional<model::Resource>(t2r(METAL))));

				} else if (id == prodCycle.buildingPlanet[EXTRAFOUNDRY]) {
					int freeRobots = max(0, (int) (game.planets[id].workerGroups[0].number -
												   prodCycle.prodFactor * 16 / 2));
					int freeReses = game.planets[id].resources.count(t2r(METAL)) ?
									game.planets[id].resources.at(t2r(METAL)) : 0;
					freeRobots = min(freeRobots, 2 * freeReses);
					if (freeRobots < 16 * 3) continue;
					prodCycle.stackedPlanet[EXTRAFOUNDRY] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FOUNDRY],
															max(freeRobots - freeReses, 0),
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FOUNDRY],
															freeReses,
															optional<model::Resource>(t2r(METAL))));


				} else if (id == prodCycle.buildingPlanet[FURNACE]) {
					int freeRobots = max(0,
										 (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 8));
					if (freeRobots < 12 * 3) continue;
					prodCycle.stackedPlanet[FURNACE] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[CAREER],
															freeRobots / 2,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[CHIP_FACTORY],
															freeRobots - freeRobots / 2,
															optional<model::Resource>(t2r(SILICON))));


				} else if (id == prodCycle.buildingPlanet[BIOREACTOR]) {
					int freeRobots = max(0,
										 (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 4));
					if (freeRobots < 12 * 3) continue;
					prodCycle.stackedPlanet[BIOREACTOR] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FARM],
															freeRobots / 2,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[ACCUMULATOR_FACTORY],
															freeRobots - freeRobots / 2,
															optional<model::Resource>(t2r(PLASTIC))));


				} else if (id == prodCycle.buildingPlanet[CHIP_FACTORY]) {
					int freeRobots = max(0,
										 (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 4));
					if (freeRobots < 12 * 3) continue;
					prodCycle.stackedPlanet[CHIP_FACTORY] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[MINES],
															freeRobots / 3,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[CAREER],
															freeRobots / 3,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[REPLICATOR],
															freeRobots - 2 * freeRobots / 3,
															optional<model::Resource>(t2r(CHIP))));


				} else if (id == prodCycle.buildingPlanet[ACCUMULATOR_FACTORY]) {
					int freeRobots = max(0,
										 (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 2));
					if (freeRobots < 12 * 3) continue;
					prodCycle.stackedPlanet[ACCUMULATOR_FACTORY] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[MINES],
															freeRobots / 3,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FARM],
															freeRobots / 3,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[REPLICATOR],
															freeRobots - 2 * freeRobots / 3,
															optional<model::Resource>(t2r(ACCUMULATOR))));


				} else if (id == prodCycle.buildingPlanet[REPLICATOR]) {
					int freeRobots = max(0, (int) (game.planets[id].workerGroups[0].number - prodCycle.prodFactor * 5));
					if (freeRobots < 14 * 3) continue;
					prodCycle.stackedPlanet[REPLICATOR] = false;
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FARM],
															1 * freeRobots / 7,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[CAREER],
															2 * freeRobots / 7,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[MINES],
															freeRobots -
															3 * freeRobots / 7,
															optional<model::Resource>()));
				} else {
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[FARM],
															game.planets[id].workerGroups[0].number / 7,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[CAREER],
															2 * game.planets[id].workerGroups[0].number / 7,
															optional<model::Resource>()));
					moveActions.push_back(model::MoveAction(id, prodCycle.buildingPlanet[MINES],
															game.planets[id].workerGroups[0].number -
															3 * (game.planets[id].workerGroups[0].number / 7),
															optional<model::Resource>()));
				}
			}
		}
	}

	return model::Action(moveActions, buildActions, optional<model::Specialty>());
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