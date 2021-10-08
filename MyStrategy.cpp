#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() : homePlanet(-1) {}

model::Action MyStrategy::getAction(const model::Game& game) {
	if (homePlanet == -1) init(game);
	separatePlanets(game);

	vector<model::MoveAction> moveActions;
	vector<model::BuildingAction> buildActions;

	for (int id = 0; id < game.planets.size(); ++id) { // checking robots tasks
		if (planetOwner[id] != 1) continue;
		if (game.planets[id].workerGroups.empty()) continue;

		int freeRobots = game.planets[id].workerGroups[0].number;
		for (Task task: tasks) {
			if (task.currentPlanet != id) continue;
			if (freeRobots < task.number) continue;
			freeRobots -= task.number;

			if (task.finishPlanet != task.startPlanet) {
				//TODO realise resource checking
				moveActions.push_back(task.getMoveAction());
			}
		}
		if (freeRobots == 0) continue;

		for (int targetId = 0; targetId < game.planets.size() && freeRobots > 0; ++targetId) {
			if (game.planets[targetId].building.has_value() &&
				bookedPlace[targetId] != 0
					) { //TODO change to true place calculating

				bookedPlace[targetId] = min(100, freeRobots);
				Task task = Task{min(100, freeRobots), targetId, targetId, -1, targetId};
				moveActions.push_back(task.getMoveAction());
				tasks.push_back(task);
				freeRobots -= min(100, freeRobots);
			}
		}
		if (freeRobots == 0) continue;

		for (int targetId = 0; targetId < game.planets.size() && freeRobots > 0; ++targetId) {
			if (!game.planets[targetId].building.has_value() &&
				game.planets[targetId].harvestableResource.has_value() &&
				bookedPlace[targetId] != 0
					) { //TODO change to true place calculating

				//TODO change to 2:1
				int nearestDist = -1, nearestId = -1;
				for (int i = 0; i < game.planets.size(); ++i) {
					if (i == targetId) continue;
					int dist = abs(game.planets[i].x - game.planets[targetId].x) +
							   abs(game.planets[i].y - game.planets[targetId].y);

					if (nearestDist == -1 || dist < nearestDist) {
						nearestDist = dist;
						nearestId = i;
					}
				}
				if (nearestId == id) {

				} else {
					if (!game.planets[nearestId].building.has_value()) {
						if (!game.planets[id].resources.count(t2r(STONE)) ||
							game.planets[id].resources.at(t2r(STONE)) < 100)
							continue;
						if (freeRobots < 100) continue;
						bookedPlace[targetId] = 100;
						Task task = Task{100, id, nearestId, STONE, targetId};
						moveActions.push_back(task.getMoveAction());
						tasks.push_back(task);
						freeRobots -= 100;
					}
				}

				if (!game.planets[id].resources.count(t2r(STONE)) || game.planets[id].resources.at(t2r(STONE)) < 50)
					continue;
				if (freeRobots < 100) continue;
				bookedPlace[targetId] = 100;
				Task task = Task{100, id, nearestId, STONE, targetId};
				moveActions.push_back(task.getMoveAction());
				tasks.push_back(task);
				freeRobots -= 100;
			}
		}
		if (freeRobots == 0) continue;
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
}

void MyStrategy::separatePlanets(const model::Game& game) { // generating list of planets
	//TODO change to true zone separating
	if (planetOwner.empty()) {
		planetOwner = vector<int>(game.planets.size(), 0);
		bookedResource = vector<int>(game.planets.size(), 0);
		bookedPlace = vector<int>(game.planets.size(), 0);
	}

	for (int id = 0; id < game.planets.size(); ++id) {
		if (game.planets[id].workerGroups.empty() || game.planets[id].workerGroups[0].playerIndex == game.myIndex) {
			planetOwner[id] = 1;
		} else {
			planetOwner[id] = 0;
		}
	}
}