//
// Created by Maximilian on 09.10.2021.
//

#include "Cycle.h"

Cycle::Cycle() : buildingPlanet(CYCLE_BUILD_NUM, -1), isBuilt(false),
				 orderedPlanet(CYCLE_BUILD_NUM, false), isPlanned(false),
				 stackedPlanet(CYCLE_BUILD_NUM, false), prodFactor(1),
				 trafficCoeff(CYCLE_BUILD_NUM, vector<int>(CYCLE_BUILD_NUM, 0)) {

	trafficCoeff[MINES][FOUNDRY] = 8;
	trafficCoeff[FOUNDRY][MINES] = 4;

	trafficCoeff[MINES][EXTRAFOUNDRY] = 8;
	trafficCoeff[EXTRAFOUNDRY][MINES] = 4;

	trafficCoeff[FOUNDRY][CHIP_FACTORY] = 2;
	trafficCoeff[EXTRAFOUNDRY][CHIP_FACTORY] = 2;
	trafficCoeff[CHIP_FACTORY][MINES] = 3;
	trafficCoeff[CHIP_FACTORY][CAREER] = 3;

	trafficCoeff[FOUNDRY][ACCUMULATOR_FACTORY] = 1;
	trafficCoeff[EXTRAFOUNDRY][ACCUMULATOR_FACTORY] = 1;
	trafficCoeff[ACCUMULATOR_FACTORY][MINES] = 2;
	trafficCoeff[ACCUMULATOR_FACTORY][FARM] = 1;

	trafficCoeff[FOUNDRY][REPLICATOR] = 1;
	trafficCoeff[EXTRAFOUNDRY][REPLICATOR] = 1;
	trafficCoeff[CHIP_FACTORY][REPLICATOR] = 2;
	trafficCoeff[ACCUMULATOR_FACTORY][REPLICATOR] = 1;
	trafficCoeff[REPLICATOR][MINES] = 3;
	trafficCoeff[REPLICATOR][CAREER] = 1;
	trafficCoeff[REPLICATOR][FARM] = 1;

	trafficCoeff[CAREER][FURNACE] = 8;
	trafficCoeff[FURNACE][CAREER] = 4;

	trafficCoeff[FURNACE][CHIP_FACTORY] = 4;

	trafficCoeff[FARM][BIOREACTOR] = 4;
	trafficCoeff[BIOREACTOR][FARM] = 2;

	trafficCoeff[BIOREACTOR][ACCUMULATOR_FACTORY] = 2;
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
		if (shortageRobots[planet][plK.first] > 1 && leftRobots > 0) {
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

int planetType(const model::Game& game, int id) {
	if (game.planets[id].harvestableResource.has_value() &&
		game.planets[id].harvestableResource.value() != t2r(STONE)) { //has a harvestable resource differing from stone
		return r2t(game.planets[id].harvestableResource.value());
	} else return 3;
}

int Cycle::buildeff(const vector<vector<int>>& planetDists, const vector<int>& candidates) {
	int ans = 0;
	for (int i = 0; i < CYCLE_BUILD_NUM; i++) {
		for (int j = 0; j < CYCLE_BUILD_NUM; j++) {
			ans += trafficCoeff[i][j] * planetDists[candidates[i]][candidates[j]];
		}
	}

	return ans;
}

bool Cycle::onMySide(const vector<vector<int>>& planetDists, const int& homePlanet, const vector<int>& enemyHomePlanets,
					 int id) {
	int dist = -1;
	for (int en: enemyHomePlanets) {
		if (dist == -1) dist = planetDists[id][en];
		else dist = min(dist, planetDists[id][en]);
	}

	return ((double) dist > 0.75 * (double) planetDists[id][homePlanet]);
}

void Cycle::planBuilding(const model::Game& game, const int& homePlanet, const vector<int>& enemyHomePlanets,
						 const vector<vector<int>>& planetDists) {
	cout << "Calculating optimal building..." << "\n";

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	const int attepmts = 175;
	const int opts = 100;
	vector<vector<int>> candidates;
	candidates.resize(attepmts);

	vector<vector<int>> ptypes(5); //planet type: planets' ids
	//ptypes.resize(5); // 0-2 - RES, 3 - NO RES, 4 - ALL
	for (int id = 0; id < game.planets.size(); ++id) {
		//is mine, doesn't have a building, and on 'my' side
		if ((game.planets[id].workerGroups.empty() || game.planets[id].workerGroups[0].playerIndex == game.myIndex)
			&& !game.planets[id].building.has_value() && onMySide(planetDists, homePlanet, enemyHomePlanets, id)) {
			ptypes[planetType(game, id)].push_back(id);
			ptypes[4].push_back(id);
		}
	}

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

		int cureff = buildeff(planetDists, current);
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
					int neweff = buildeff(planetDists, newcand);
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

	std::sort(candidates.begin(), candidates.end(), [&](const vector<int>& b1, const vector<int>& b2) {
		return buildeff(planetDists, b1) < buildeff(planetDists, b2);
	}); //sorting candidates
	vector<int> bestbuilding = candidates[0];

	for (int i = 0; i < 10; i++) {
		cout << i << ": " << bestbuilding[i] << "\n";
	}
	int totalbuildeff = buildeff(planetDists, bestbuilding);
	cout << totalbuildeff << '\n';
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "Time difference = " <<
			  std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "ms" << std::endl;

	for (int i = 0; i < 10; i++) {
		buildingPlanet[i] = bestbuilding[i];
	}

	buildEff = totalbuildeff;
	isPlanned = true;
}