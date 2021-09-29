#include "MyStrategy.hpp"
#include <exception>

#include <vector>
#include <iostream>
using namespace std;

MyStrategy::MyStrategy() {}

model::Action MyStrategy::getAction(const model::Game& game) {
	vector<int> targetPlanets;
	vector<model::MoveAction> moveActions;
	vector<model::BuildingAction> buildingActions;

	for (int i = 0; i < game.planets.size(); ++i) {
		if (game.planets[i].harvestableResource.has_value()) {
			targetPlanets.push_back(i);

			buildingActions.push_back(model::BuildingAction(i, optional<model::BuildingType>(
					(model::BuildingType) (int) game.planets[i].harvestableResource.value())));
		}
	}

	for (int i = 0; i < game.planets.size(); ++i) {
		for (model::WorkerGroup workerGroup: game.planets[i].workerGroups) {
			if (workerGroup.playerIndex == game.myIndex) {
				for (int targetPlanet: targetPlanets) {
					moveActions.push_back(model::MoveAction(i, targetPlanet, workerGroup.number / targetPlanets.size(),
															optional<model::Resource>(model::Resource::STONE)));
				}
			}
		}
	}

	cout << game.toString() << endl;

	return model::Action(moveActions, buildingActions);
}