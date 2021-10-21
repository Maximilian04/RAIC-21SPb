//
// Created by Maximilian on 09.10.2021.
//

#ifndef MYSTRATEGY_CPP_CYCLE_H
#define MYSTRATEGY_CPP_CYCLE_H

#define CYCLE_BUILD_NUM 10

#include <vector>
#include <set>

using namespace std;

class Cycle {
public:
	vector<int> buildingPlanet; // MINES -> REPLICATOR + EXTRAFOUNDRY
	vector<bool> orderedPlanet; // MINES -> REPLICATOR + EXTRAFOUNDRY
	vector<bool> stackedPlanet; // MINES -> REPLICATOR+ EXTRAFOUNDRY
	set<int> usedPlanets;
	bool isBuilt;
	bool isPlanned;

	float prodFactor;

	Cycle();
};


#endif //MYSTRATEGY_CPP_CYCLE_H
