//
// Created by Maximilian on 07.11.2021.
//

#include "Observer.h"

Observer::Observer() {
}

void Observer::update(const model::Game& game, vector<vector<int>>& d) {
	// Init if first iteration
	if (traffic.size() == 0) {
		traffic.assign(game.planets.size(), 0);
		bottleneckTraffic.assign(game.planets.size(), 0);

		bottleneckTrafficTimed.assign(game.planets.size(), vector<int>(OBSERVER_PERIOD, 0));

	}

	// Count enemies on every planet
	enemies.assign(game.planets.size(), 0);
	for (int i = 0; i < game.planets.size(); i++) {
		int enemiesNum = 0;
		for (model::WorkerGroup wg: game.planets[i].workerGroups)
			if (wg.playerIndex != game.myIndex)
				enemiesNum += wg.number;

		enemies[i] = enemiesNum;
	}

	// Normal traffic amounts (1st criterium)
	for (int i = 0; i < game.planets.size(); i++)
		traffic[i] = max(enemies[i] * 0.05 + traffic[i] * 0.95, enemies[i] * 0.5 + traffic[i] * 0.5);

	// Calculate bottleneck traffic
	vector<int> bottleneckTemp(game.planets.size(), 0);
	for (model::FlyingWorkerGroup fg: game.flyingWorkerGroups)
		if (d[fg.departurePlanet][fg.targetPlanet] > game.maxTravelDistance && fg.departureTick == game.currentTick - 1)
			bottleneckTemp[fg.nextPlanet] += fg.number;

	for (int i = 0; i < game.planets.size(); i++) {
		// Remove oldest value and push current
		bottleneckTrafficTimed[i].erase(bottleneckTrafficTimed[i].begin());
		bottleneckTrafficTimed[i].push_back(bottleneckTemp[i]);

		double avg = 0;
		for (int& value: bottleneckTrafficTimed[i])
			avg += value;
		avg /= bottleneckTrafficTimed[i].size();

		bottleneckTraffic[i] = avg;
	}
}

bool Observer::isOurs(int planet) {
	return enemies[planet] == 0;
}

int Observer::size() {
	return enemies.size();
}