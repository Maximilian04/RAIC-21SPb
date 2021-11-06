#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() : homePlanet(-1), resetTimer(0) {}

model::Action MyStrategy::getAction(const model::Game& game) {
	if (homePlanet == -1) 
	{
		init(game);
		prodCycle.init(game, homePlanet, enemyHomePlanets, planetDists);
	}
	separatePlanets(game);
	++resetTimer;

	// total robots available
	population  = 0;
	for(int i = 0; i < game.planets.size(); i++)
	{
		//int or = 0;
		for(auto wg: game.planets[i].workerGroups)
		{
			if(wg.playerIndex == game.myIndex)
			{
				population+=wg.number;
			}
		}
	}
	for(auto fw:game.flyingWorkerGroups)
	{
		if(fw.playerIndex == game.myIndex)
		{
			population += fw.number;
		}
	}

	vector<model::MoveAction> moveActions;
	vector<model::BuildingAction> buildActions;

	if (!prodCycle.isPlanned) {
		prodCycle.planBuilding(game, homePlanet, enemyHomePlanets, logDists);
	} else if (!prodCycle.isBuilt) {
#if 0 //TODO: отредачить под новый формат buildPlanets
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

				//moveActions.push_back(model::MoveAction(homePlanet, prodCycle.buildingPlanet[building],
				//										stoneCost(building == EXTRAFOUNDRY ? FOUNDRY : building),
				//										optional<model::Resource>(t2r(STONE))));
				fc.send(homePlanet, prodCycle.buildingPlanet[building],
														stoneCost(building == EXTRAFOUNDRY ? FOUNDRY : building),
														optional<model::Resource>(t2r(STONE)));
				freeStone -= stoneCost(building);
				prodCycle.orderedPlanet[building] = true;
			}
		}
		if (!hasMissedBuild) {
			prodCycle.isBuilt = true;
			prodCycle.prodFactor = 2.5;
			cout << "работаем" << endl;
		}
#endif
	} else {
			
#if 0 //TODO: отредачить под новый формат buildPlanets
		if (resetTimer > 100) {
			for (int building = 3; building < prodCycle.stackedPlanet.size(); ++building) {
				if (prodCycle.stackedPlanet[building]) {
					if (game.planets[prodCycle.buildingPlanet[building]].workerGroups.empty() ||
						game.planets[prodCycle.buildingPlanet[building]].workerGroups[0].playerIndex != game.myIndex) {
						continue;
					}
					int fr = prodCycle.buildingPlanet[building];
					int freeRobots = game.planets[fr].workerGroups[0].number - fc.onFlightAt(fr);
					
					/*moveActions.push_back(
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
					*/

					fc.send(fr, prodCycle.buildingPlanet[CAREER],
											  freeRobots / 3,
											  optional<model::Resource>());

					fc.send(fr, prodCycle.buildingPlanet[FARM],
											  freeRobots / 3,
											  optional<model::Resource>());

					fc.send(fr, prodCycle.buildingPlanet[MINES],
											  freeRobots -
											  2 * freeRobots / 3,
											  optional<model::Resource>());
				}
			}

			resetTimer = 0;
			prodCycle.stackedPlanet = vector<bool>(prodCycle.stackedPlanet.size(), true);
		}
		for (int id = 0; id < game.planets.size(); ++id) {
			if (!game.planets[id].workerGroups.empty() &&
				game.planets[id].workerGroups[0].playerIndex == game.myIndex) {

				if (id == prodCycle.buildingPlanet[MINES]) {
					if (prodCycle.sendRobots(game, fc, id, ORE, 32,
											 {
													 {prodCycle.buildingPlanet[FOUNDRY],      1.0 / 2},
													 {prodCycle.buildingPlanet[EXTRAFOUNDRY], 1.0 / 2}},
											 {},
											 12 * 3)) {

						prodCycle.stackedPlanet[MINES] = false;
					}

				} else if (id == prodCycle.buildingPlanet[CAREER]) {
					if (prodCycle.sendRobots(game, fc, id, SAND, 16,
											 {
													 {prodCycle.buildingPlanet[FURNACE], 1.0 / 1}},
											 {},
											 12 * 3)) {

						prodCycle.stackedPlanet[CAREER] = false;
					}

				} else if (id == prodCycle.buildingPlanet[FARM]) {
					if (prodCycle.sendRobots(game, fc, id, ORGANICS, 8,
											 {
													 {prodCycle.buildingPlanet[BIOREACTOR], 1.0 / 1}},
											 {},
											 6 * 3)) {

						prodCycle.stackedPlanet[FARM] = false;
					}

				} else if (id == prodCycle.buildingPlanet[FOUNDRY]) {
					if (prodCycle.sendRobots(game, fc, id, METAL, 16 / 2,
											 {
													 {prodCycle.buildingPlanet[CHIP_FACTORY],        1.0 / 4},
													 {prodCycle.buildingPlanet[ACCUMULATOR_FACTORY], 1.0 / 8},
													 {prodCycle.buildingPlanet[REPLICATOR],          1.0 / 8}},
											 {
													 {prodCycle.buildingPlanet[MINES], 1.0 / 2}},
											 16 * 3)) {

						prodCycle.stackedPlanet[FOUNDRY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[EXTRAFOUNDRY]) {
					if (prodCycle.sendRobots(game, fc, id, METAL, 16 / 2,
											 {
													 {prodCycle.buildingPlanet[CHIP_FACTORY],        1.0 / 4},
													 {prodCycle.buildingPlanet[ACCUMULATOR_FACTORY], 1.0 / 8},
													 {prodCycle.buildingPlanet[REPLICATOR],          1.0 / 8}},
											 {
													 {prodCycle.buildingPlanet[MINES], 1.0 / 2}},
											 16 * 3)) {

						prodCycle.stackedPlanet[EXTRAFOUNDRY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[FURNACE]) {
					if (prodCycle.sendRobots(game, fc, id, SILICON, 8,
											 {
													 {prodCycle.buildingPlanet[CHIP_FACTORY], 1.0 / 2}},
											 {
													 {prodCycle.buildingPlanet[CAREER], 1.0 / 2}},
											 12 * 3)) {

						prodCycle.stackedPlanet[FURNACE] = false;
					}

				} else if (id == prodCycle.buildingPlanet[BIOREACTOR]) {
					if (prodCycle.sendRobots(game, fc, id, PLASTIC, 4,
											 {
													 {prodCycle.buildingPlanet[ACCUMULATOR_FACTORY], 1.0 / 2}},
											 {
													 {prodCycle.buildingPlanet[FARM], 1.0 / 2}},
											 12 * 3)) {

						prodCycle.stackedPlanet[BIOREACTOR] = false;
					}

				} else if (id == prodCycle.buildingPlanet[CHIP_FACTORY]) {
					if (prodCycle.sendRobots(game, fc, id, CHIP, 4,
											 {
													 {prodCycle.buildingPlanet[REPLICATOR], 1.0 / 3}},
											 {
													 {prodCycle.buildingPlanet[MINES],  1.0 / 3},
													 {prodCycle.buildingPlanet[CAREER], 1.0 / 3}},
											 12 * 3)) {

						prodCycle.stackedPlanet[CHIP_FACTORY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[ACCUMULATOR_FACTORY]) {
					if (prodCycle.sendRobots(game, fc, id, ACCUMULATOR, 2,
											 {
													 {prodCycle.buildingPlanet[REPLICATOR], 1.0 / 3}},
											 {
													 {prodCycle.buildingPlanet[MINES], 1.0 / 3},
													 {prodCycle.buildingPlanet[FARM],  1.0 / 3}},
											 12 * 3)) {

						prodCycle.stackedPlanet[ACCUMULATOR_FACTORY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[REPLICATOR]) {
					if (prodCycle.sendRobots(game, fc, id, -1, 5,
											 {},
											 {
													 {prodCycle.buildingPlanet[FARM],   1.0 / 7},
													 {prodCycle.buildingPlanet[CAREER], 1.0 * 2 / 7},
													 {prodCycle.buildingPlanet[MINES],  1.0 * 4 / 7}},
											 14 * 3)) {

						prodCycle.stackedPlanet[REPLICATOR] = false;
					}

				} else {
					prodCycle.sendRobots(game, fc, id, -1, 0,
										 {},
										 {
												 {prodCycle.buildingPlanet[FARM],   1.0 / 7},
												 {prodCycle.buildingPlanet[CAREER], 1.0 * 2 / 7},
												 {prodCycle.buildingPlanet[MINES],  1.0 * 4 / 7}},
										 0, true);
				}
			}
		}
#endif
	}

	if (game.currentTick == 999) {
		int robC = 0;
		fstream file("stat.txt", ios::in);
		file >> robC;
		file.close();
		for (int id = 0; id < game.planets.size(); ++id) {
			if (!game.planets[id].workerGroups.empty() &&
				game.planets[id].workerGroups[0].playerIndex == game.myIndex) {
				robC += game.planets[id].workerGroups[0].number;
			}
		}
		for (int i = 0; i < game.flyingWorkerGroups.size(); ++i) {
			if (game.flyingWorkerGroups[i].playerIndex == game.myIndex) {
				robC += game.flyingWorkerGroups[i].number;
			}
		}
		file.open("stat.txt", ios::out);
		file << robC;
		file.close();
		int c;
		file.open("statC.txt", ios::in);
		file >> c;
		file.close();
		file.open("statC.txt", ios::out);
		file << c + 1;
		file.close();
		file.open("statRes.txt", ios::out);
		file << robC / (c + 1);
		file.close();
	}

	vector<model::MoveAction> addMoves = fc.update();
	for (model::MoveAction move : addMoves)
	{
		moveActions.push_back(move);
	}

	return model::Action(moveActions, buildActions, optional<model::Specialty>());
}

void MyStrategy::init(const model::Game& game) {
	for (int i = 0; i < game.planets.size(); ++i) {
		if (!game.planets[i].workerGroups.empty() &&
			game.planets[i].workerGroups[0].playerIndex == game.myIndex) {
			homePlanet = i;
		}
		if (!game.planets[i].workerGroups.empty() &&
			game.planets[i].workerGroups[0].playerIndex != game.myIndex) {
			enemyHomePlanets.push_back(i);
		}
	}

	planetDists = vector<vector<int>>(game.planets.size(), vector<int>(game.planets.size(), 0));
	logDists = vector<vector<int>>(game.planets.size(), vector<int>(game.planets.size(), 0));
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

				if ((abs(game.planets[i].x - game.planets[j].x) +
					 abs(game.planets[i].y - game.planets[j].y) <= game.maxTravelDistance + game.logisticsUpgrade)) {
					logDists[i][j] = logDists[j][i] = abs(game.planets[i].x - game.planets[j].x) +
															abs(game.planets[i].y - game.planets[j].y);

					for (int m = 0; m < game.planets.size(); ++m) {
						if (m == i || m == j) continue;
						if (logDists[m][i] != 0 &&
							(logDists[m][j] == 0 || logDists[m][j] > (logDists[m][i] + logDists[i][j]))) {
							logDists[m][j] = logDists[j][m] = logDists[m][i] + logDists[i][j];
						}
						if (logDists[m][j] != 0 &&
							(logDists[m][i] == 0 || logDists[m][i] > (logDists[m][j] + logDists[i][j]))) {
							logDists[m][i] = logDists[i][m] = logDists[m][j] + logDists[i][j];
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
#if 0
	prodCycle.buildingPlanet = vector<int>(9, -1);

	vector<vector<int>> adj;

	for (int i = 0; i < game.planets.size(); ++i) {
		adj.push_back({});
		for (int j = 0; j < game.planets.size(); ++j) {
			if ((abs(game.planets[i].x - game.planets[j].x) + abs(game.planets[i].y - game.planets[j].y) <= game.maxTravelDistance))
				adj[i].push_back(j);
		}
	}

	fc.setup(planetDists, adj);
	//prodCycle.buildingPlanet = vector<int>(9, -1);
}
#endif

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
