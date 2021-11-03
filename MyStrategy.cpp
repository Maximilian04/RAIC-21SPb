#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() : homePlanet(-1), resetTimer(0) {}

int planetType(const model::Game& game, int id) {
	if (game.planets[id].harvestableResource.has_value() &&
		game.planets[id].harvestableResource.value() != t2r(STONE)) { //has a harvestable resource differing from stone
		return r2t(game.planets[id].harvestableResource.value());
	} else return 3;
}

int buildeff(const MyStrategy* strat, vector<int> candidates) {
	int ans = 0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			ans += strat->prodCycle.buildcoeff[i][j] * (strat->planetDists)[candidates[i]][candidates[j]];
		}
	}

	return ans;
}

struct BuildComp {
	MyStrategy* strat;
	BuildComp(MyStrategy* s) {
		strat = s;
	}

	bool operator()(vector<int> b1, vector<int> b2) {
		return buildeff(strat, b1) < buildeff(strat, b2);
	}
};

bool MyStrategy::onMySide(int id) {
	int dist = -1;
	for (int en: enemyhomePlanets) {
		if (dist == -1) dist = planetDists[id][en];
		else dist = min(dist, planetDists[id][en]);
	}

	return ((double) dist > 0.75 * (double) planetDists[id][homePlanet]);
}

model::Action MyStrategy::getAction(const model::Game& game) {
	if (homePlanet == -1) init(game);
	separatePlanets(game);
	++resetTimer;

	vector<model::MoveAction> moveActions;
	vector<model::BuildingAction> buildActions;

	if (!prodCycle.isPlanned) {
		cout << "Calculating optimal building..." << "\n";

		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		const int attepmts = 175;
		const int opts = 100;
		vector<vector<int>> candidates;
		candidates.resize(attepmts);

		vector<vector<int>> ptypes; //planet type: planets' ids
		ptypes.resize(5); // 0-2 - RES, 3 - NO RES, 4 - ALL
		for (int id = 0; id < game.planets.size(); ++id) {
			//is mine, doesn't have a building, and on 'my' side
			if (planetInf[id].planetOwner && !game.planets[id].building.has_value() && onMySide(id)) {
				ptypes[planetType(game, id)].push_back(id);
				ptypes[4].push_back(id);
			}
		}

#if 0
		for(int i = 0; i < 4; i++)
		{
			cout << ptypes[i].size() << "\t";
		}
		cout << "\n";
#endif

		vector<int> avdist(5, 0); //average distances between planets of one type
		for (int t = 0; t < 5; t++) {
			int num = 0;
			int distsum = 0;
			for (int i = 0; i < ptypes[t].size(); i++) {
				for (int j = 0; j < ptypes[t].size(); j++) {
					num++;
					distsum += planetDists[ptypes[t][i]][ptypes[t][j]];
				}
			}

			if (t < 3) avdist[t] = distsum / num;
			else avdist[t] = game.maxTravelDistance * 2.5;
		}

		vector<vector<int>> pgraph(
				game.planets.size()); //graph where objects are planets connected with their 'neighbour' planets with the same type
		for (int i = 0; i < game.planets.size(); i++) {
			int type = planetType(game, i);
			if (type == 3)
				type = 4; //if a planet doesn't have resources then its `neigbours` are planets with any type nearby
			for (int j = 0; j < ptypes[type].size(); j++) {
				if (planetDists[i][ptypes[type][j]] <= avdist[type]) {
					pgraph[i].push_back(ptypes[type][j]);
				}
			}
		}

		std::default_random_engine generator;
		for (int att = 0; att < attepmts; att++) //making attempts trying to find the best solution
		{
			vector<int> current(10, -1);

			/*std::uniform_int_distribution<int> distribution(1,6);
			int dice_roll = distribution(generator); */
			for (int i = 0; i < 10; i++) {
				int btype = 3;
				if (i < 3) btype = i; //building type

				int randompl = -1;
				//check if current contains randompl (if it does - choose another randompl)
				while (std::count(current.begin(), current.begin() + i + 1, randompl) > 0) {
					std::uniform_int_distribution<int> distribution(0, ptypes[btype].size() - 1);
					int randomind = distribution(generator);
					randompl = ptypes[btype][randomind];
				}
				current[i] = randompl;
			}

			int cureff = buildeff(this, current);
			set<int> usedids;
			map<int, int> indicies;
			for (int i = 0; i < 10; i++) {
				usedids.emplace(current[i]);
				indicies[current[i]] = i;
			}

			for (int o = 0; o < opts; o++) {
				bool change = false;
				for (int i = 0; i < 10; i++) {
					for (int j = 0; j < pgraph[current[i]].size(); j++) {
						int pl = pgraph[current[i]][j];
						vector<int> newcand = current;
						bool swap = false;
						if (usedids.find(pl) != usedids.end()) //if planet is used then try to swap it
						{
							if (indicies[pl] >= 3 ||
								planetType(game, current[i]) == indicies[pl]) //check if we can swap
							{
								newcand[i] = pl;
								newcand[indicies[pl]] = current[i];
								/*indicies[current[i]] = indicies[pl]; 
								indicies[pl] = i;*/
								swap = true;
							} else continue; //if we can't swap just move on
						} else {
							newcand[i] = pl;
							//usedids.erase(current[i]);
						}
						int neweff = buildeff(this, newcand);
						if (neweff < cureff) {
							if (swap) {
								indicies[current[i]] = indicies[pl];
								indicies[pl] = i;
							} else {
								usedids.erase(current[i]);
								usedids.emplace(pl);
								indicies.erase(current[i]);
								indicies[pl] = i;
							}
							current = newcand;
							cureff = neweff;
							change = true;

							break;
						}
					}
				}
				if (!change) break;
			}
			//cout << cureff << "\n";

			candidates[att] = current;
		}

		std::sort(candidates.begin(), candidates.end(), BuildComp(this)); //sorting candidates
		vector<int> bestbuilding = candidates[0];

		for (int i = 0; i < 10; i++) {
			cout << i << ": " << bestbuilding[i] << "\n";
		}
		cout << buildeff(this, bestbuilding) << '\n';
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << "Time difference = " <<
				  std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "ms" << std::endl;

		for (int i = 0; i < 10; i++) {
			prodCycle.buildingPlanet[i] = bestbuilding[i];
		}

		prodCycle.isPlanned = true;
#if 0
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
#endif
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
					if (prodCycle.sendRobots(game, moveActions, id, ORE, 32,
											 {
													 {prodCycle.buildingPlanet[FOUNDRY],      1.0 / 2},
													 {prodCycle.buildingPlanet[EXTRAFOUNDRY], 1.0 / 2}},
											 {},
											 12*3)) {

						prodCycle.stackedPlanet[MINES] = false;
					}

				} else if (id == prodCycle.buildingPlanet[CAREER]) {
					if (prodCycle.sendRobots(game, moveActions, id, SAND, 16,
											 {
													 {prodCycle.buildingPlanet[FURNACE], 1.0 / 1}},
											 {},
											 12*3)) {

						prodCycle.stackedPlanet[CAREER] = false;
					}

				} else if (id == prodCycle.buildingPlanet[FARM]) {
					if (prodCycle.sendRobots(game, moveActions, id, ORGANICS, 8,
											 {
													 {prodCycle.buildingPlanet[BIOREACTOR], 1.0 / 1}},
											 {},
											 6*3)) {

						prodCycle.stackedPlanet[FARM] = false;
					}

				} else if (id == prodCycle.buildingPlanet[FOUNDRY]) {
					if (prodCycle.sendRobots(game, moveActions, id, METAL, 16 / 2,
											 {
													 {prodCycle.buildingPlanet[CHIP_FACTORY],        1.0 / 4},
													 {prodCycle.buildingPlanet[ACCUMULATOR_FACTORY], 1.0 / 8},
													 {prodCycle.buildingPlanet[REPLICATOR],          1.0 / 8}},
											 {
													 {prodCycle.buildingPlanet[MINES], 1.0 / 2}},
											 16*3)) {

						prodCycle.stackedPlanet[FOUNDRY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[EXTRAFOUNDRY]) {
					if (prodCycle.sendRobots(game, moveActions, id, METAL, 16 / 2,
											 {
													 {prodCycle.buildingPlanet[CHIP_FACTORY],        1.0 / 4},
													 {prodCycle.buildingPlanet[ACCUMULATOR_FACTORY], 1.0 / 8},
													 {prodCycle.buildingPlanet[REPLICATOR],          1.0 / 8}},
											 {
													 {prodCycle.buildingPlanet[MINES], 1.0 / 2}},
											 16*3)) {

						prodCycle.stackedPlanet[EXTRAFOUNDRY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[FURNACE]) {
					if (prodCycle.sendRobots(game, moveActions, id, SILICON, 8,
											 {
													 {prodCycle.buildingPlanet[CHIP_FACTORY], 1.0 / 2}},
											 {
													 {prodCycle.buildingPlanet[CAREER], 1.0 / 2}},
											 12*3)) {

						prodCycle.stackedPlanet[FURNACE] = false;
					}

				} else if (id == prodCycle.buildingPlanet[BIOREACTOR]) {
					if (prodCycle.sendRobots(game, moveActions, id, PLASTIC, 4,
											 {
													 {prodCycle.buildingPlanet[ACCUMULATOR_FACTORY], 1.0 / 2}},
											 {
													 {prodCycle.buildingPlanet[FARM], 1.0 / 2}},
											 12*3)) {

						prodCycle.stackedPlanet[BIOREACTOR] = false;
					}

				} else if (id == prodCycle.buildingPlanet[CHIP_FACTORY]) {
					if (prodCycle.sendRobots(game, moveActions, id, CHIP, 4,
											 {
													 {prodCycle.buildingPlanet[REPLICATOR], 1.0 / 3}},
											 {
													 {prodCycle.buildingPlanet[MINES],  1.0 / 3},
													 {prodCycle.buildingPlanet[CAREER], 1.0 / 3}},
											 12*3)) {

						prodCycle.stackedPlanet[CHIP_FACTORY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[ACCUMULATOR_FACTORY]) {
					if (prodCycle.sendRobots(game, moveActions, id, ACCUMULATOR, 2,
											 {
													 {prodCycle.buildingPlanet[REPLICATOR], 1.0 / 3}},
											 {
													 {prodCycle.buildingPlanet[MINES], 1.0 / 3},
													 {prodCycle.buildingPlanet[FARM],  1.0 / 3}},
											 12*3)) {

						prodCycle.stackedPlanet[ACCUMULATOR_FACTORY] = false;
					}

				} else if (id == prodCycle.buildingPlanet[REPLICATOR]) {
					if (prodCycle.sendRobots(game, moveActions, id, -1, 5,
											 {},
											 {
													 {prodCycle.buildingPlanet[FARM],   1.0 / 7},
													 {prodCycle.buildingPlanet[CAREER], 1.0 * 2 / 7},
													 {prodCycle.buildingPlanet[MINES],  1.0 * 4 / 7}},
											 14*3)) {

						prodCycle.stackedPlanet[REPLICATOR] = false;
					}

				} else {
					prodCycle.sendRobots(game, moveActions, id, -1, 0,
										 {},
										 {
												 {prodCycle.buildingPlanet[FARM],   1.0 / 7},
												 {prodCycle.buildingPlanet[CAREER], 1.0 * 2 / 7},
												 {prodCycle.buildingPlanet[MINES],  1.0 * 4 / 7}},
										 0, true);
				}
			}
		}
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
			enemyhomePlanets.push_back(i);
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
