#include "MyStrategy.hpp"
#include <exception>

#include <iostream>
#include <vector>

using namespace std;

#define NUM_TYPES 20 // number of types of means of production
#define NUM_RESES 10 // number of types of means of production

#define SQUAD_SIZE 10
#define BUILD_KOEF 0.5

class Mean { // class for contain information about means of production like robots, resources, buildings
public:
	enum class Type; // type of mean of production
	//!!!undefined behaviour with incorrect t!!!
	static model::BuildingType t2b(Type t); // translate Mean::Type to model::BuildingType
	static model::Resource t2r(Type t); // translate Mean::Type to model::Resource
	static Type b2t(model::BuildingType t); // translate model::BuildingType to Mean::Type
	static Type r2t(model::Resource t); // translate model::Resource to Mean::Type
	static Type eth2t(model::BuildingType t); // DANGEROUS translate model::BuildingType to Mean::Type
	static Type eth2t(model::Resource t); // DANGEROUS translate model::Resource to Mean::Type
	static Type eth2t(int t); // DANGEROUS translate int to Mean::Type !!!!!!USE ONLY WITH -1!!!!!!
	static vector<Type> getParents(Type t); //TODO реализовать
	static vector<Type> getChildren(Type t); //TODO реализовать
	static int type_table[NUM_TYPES][NUM_TYPES]; // таблица количеств необходимых ресурсов
	static bool isInitialised; //
	static void initialise(const model::Game& game);

	static const vector<Type> res_list; // list of resources
	static const vector<double> res_price_list; // list of minimum prices of resources
private:
};

class PlSub;

class Advert { // advert about object of trade
public:
	int x, y;
	double p; // price or answered price
	Mean::Type type; // type of object

	bool isAnswered; // init with false

	PlSub& author; // subject of trade
	Advert(PlSub& author, double p = -1); // type as none
	Advert(PlSub& author, Mean::Type type, double p = -1);
}; //
class Supply : public Advert {
public:
	Supply(PlSub& author, double p = -1); // type as none
	Supply(PlSub& author, Mean::Type type, double p = -1);
}; //
class Demand : public Advert {
public:
	bool isInternal; // is process or transport

	Demand(PlSub& author, bool isInternal = false, double p = -1); // type as none
	Demand(PlSub& author, Mean::Type type, bool isInternal, double p = -1);
};

class PlSub { // planet - subject of trade
public:
	int x, y;
	int id;
	Mean::Type mean; // building or harvestableResource

	///vector<Supply> supplies;
	///vector<Demand> demands;

	double prices_sell[NUM_RESES];
	double prices_buy[NUM_RESES];

	PlSub(); // init with -1
	PlSub(int x, int y, int id, Mean::Type mean);

private:
};

class Deal {
public:
	Demand* demand = nullptr;
	Supply* supply = nullptr;
	//bool isConcluded = false;
};

model::Action MyStrategy::getAction(const model::Game& game) {
	if (!Mean::isInitialised) { // Initialising
		Mean::initialise(game);
	}

	static vector<PlSub> plSubs; // all "our" planets
	static vector<bool> plZones(game.planets.size(), false);

	// ----- generating list of subjects -----
	for (int id = 0; id < game.planets.size(); ++id) {
		//TODO change to true zone separating
		if (game.planets[id].workerGroups.empty() || game.planets[id].workerGroups[0].playerIndex == game.myIndex) {
			if (!plZones[id]) {
				plZones[id] = true;
				if (game.planets[id].building.has_value()) {
					plSubs.push_back(PlSub(game.planets[id].x, game.planets[id].y, id,
										   Mean::b2t(game.planets[id].building.value().buildingType)));
				} else if (game.planets[id].harvestableResource.has_value()) {
					plSubs.push_back(PlSub(game.planets[id].x, game.planets[id].y, id,
										   Mean::r2t(game.planets[id].harvestableResource.value())));
				} else {
					plSubs.push_back(PlSub(game.planets[id].x, game.planets[id].y, id, Mean::eth2t(-1)));
				}
			} else {
				if (plZones[id]) {
					plZones[id] = false;

					for (int i = plSubs.size() - 1; i >= 0; --i) {
						if (plSubs[i].id == id) {
							plSubs.erase(plSubs.begin() + i);
							break;
						}
					}
				}
			}
		}
	}

	vector<Supply> supplyMarket;
	vector<Demand> demandMarket;

	// ----- generating supplies -----
	for (PlSub& subj: plSubs) {
		unordered_map<model::Resource, int> resources = game.planets[subj.id].resources;

		for (pair<model::Resource, int> resource: resources) {
			while (resource.second >= SQUAD_SIZE) {
				resources[resource.first] -= SQUAD_SIZE;
				supplyMarket.push_back(Supply(subj, Mean::r2t(resource.first))); //TODO set price!!!
			}
		}
	}

	// ----- generating demands -----
	for (PlSub& subj: plSubs) {
		if (game.planets[subj.id].building.has_value()) {
			if (game.planets[subj.id].harvestableResource.has_value()) { // building+resource

			} else { // building

			}
		} else {
			if (game.planets[subj.id].harvestableResource.has_value()) { // resource

			} else { // nothing

			}
		}
	}

	// ----- generating list of deals -----
	vector<Deal> dealList;
	for (int s = 0; s < supplyMarket.size(); ++s) {
		for (int d = 0; d < demandMarket.size(); ++d) {
			if (demandMarket[d].isInternal) {
				dealList.push_back({&demandMarket[d], nullptr});
			} else {
				if (demandMarket[d].type == supplyMarket[s].type) {
					dealList.push_back({&demandMarket[d], &supplyMarket[s]});
				}
			}
		}
	}

	// ----- simulating marketplace with robots -----
	vector<model::MoveAction> moveActionsR;
	for (int id = 0; id < game.planets.size(); ++id) { // todo нужно как-то тасовать порядок планет мб?
		if (game.planets[id].workerGroups.empty() || game.planets[id].workerGroups[0].playerIndex != game.myIndex) {
			continue;
		}

		int x = game.planets[id].x;
		int y = game.planets[id].y;
		for (int robotNum = game.planets[id].workerGroups[0].number; robotNum >= SQUAD_SIZE; robotNum -= SQUAD_SIZE) {
			int bestDeal = -1;
			double bestPrice = 0; // price per tick!
			for (int i = 0; i < dealList.size(); ++i) {
				if (dealList[i].demand->isAnswered) {
					dealList.erase(dealList.begin() + i);
					--i;
					continue;
				}
				double price;
				if (dealList[i].supply) { // external demand
					if (dealList[i].supply->isAnswered) {
						dealList.erase(dealList.begin() + i);
						--i;
						continue;
					}

					price = dealList[i].demand->p / (abs(dealList[i].demand->x - x) + abs(dealList[i].demand->y - y) +
													 abs(dealList[i].demand->x - dealList[i].supply->x) +
													 abs(dealList[i].demand->y - dealList[i].supply->y));
				} else { // internal demand
					price = dealList[i].demand->p - 0.0001 *
													(abs(dealList[i].demand->x - x) + abs(dealList[i].demand->y - y));
				}

				if (price > bestPrice) {
					bestPrice = price;
					bestDeal = i;
				}
			}
			if (bestDeal == -1) break;

			moveActionsR.push_back(model::MoveAction(id,
													 dealList[bestDeal].supply ? dealList[bestDeal].supply->author.id
																			   : dealList[bestDeal].demand->author.id,
													 SQUAD_SIZE, optional<model::Resource>()));
			dealList[bestDeal].demand->isAnswered = true;
			if (dealList[bestDeal].supply) {
				dealList[bestDeal].supply->isAnswered = true;
				dealList[bestDeal].supply->p = dealList[bestDeal].demand->p;
			}

			dealList.erase(dealList.begin() + bestDeal);
		}
	}

	// ----- changing prices -----
	for (Demand demand: demandMarket) {
		if (demand.isAnswered) {

		} else {

		}
	}

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
				for (int& targetPlanet: targetPlanets) {
					moveActions.push_back(model::MoveAction(i, targetPlanet, workerGroup.number / targetPlanets.size(),
															optional<model::Resource>(model::Resource::STONE)));
				}
			}
		}
	}

	return model::Action(moveActions, buildingActions);
}

MyStrategy::MyStrategy() {}

enum class Mean::Type { // it's necessary, what STONE has 0 number!!!
	none = -1,                // it's not true value!!!
	STONE = 0,              //prt

	QUARRY,                 //bld

	MINES,                  //bld
	CAREER,                 //bld
	FARM,                   //bld
	ORE,                    //prt
	SAND,                   //prt
	ORGANICS,               //prt

	FOUNDRY,                //bld
	FURNACE,                //bld
	BIOREACTOR,             //bld
	METAL,                  //prt
	SILICON,                //prt
	PLASTIC,                //prt

	CHIP_FACTORY,           //bld
	ACCUMULATOR_FACTORY,    //bld
	CHIP,                   //prt
	ACCUMULATOR,            //prt

	REPLICATOR,             //bld
	ROBOT,                  //prt
}; // it's necessary, what STONE has 0 number!!!
//!!!undefined behaviour with incorrect t!!!
model::BuildingType Mean::t2b(Mean::Type t) {
	switch (t) {
		case Mean::Type::QUARRY:
			return model::BuildingType::QUARRY;
			break;
		case Mean::Type::MINES:
			return model::BuildingType::MINES;
			break;
		case Mean::Type::CAREER:
			return model::BuildingType::CAREER;
			break;
		case Mean::Type::FARM:
			return model::BuildingType::FARM;
			break;
		case Mean::Type::FOUNDRY:
			return model::BuildingType::FOUNDRY;
			break;
		case Mean::Type::FURNACE:
			return model::BuildingType::FURNACE;
			break;
		case Mean::Type::BIOREACTOR:
			return model::BuildingType::BIOREACTOR;
			break;
		case Mean::Type::CHIP_FACTORY:
			return model::BuildingType::CHIP_FACTORY;
			break;
		case Mean::Type::ACCUMULATOR_FACTORY:
			return model::BuildingType::ACCUMULATOR_FACTORY;
			break;
		case Mean::Type::REPLICATOR:
			return model::BuildingType::REPLICATOR;
			break;
	}
} //
model::Resource Mean::t2r(Mean::Type t) {
	switch (t) {
		case Mean::Type::STONE:
			return model::Resource::STONE;
			break;
		case Mean::Type::ORE:
			return model::Resource::ORE;
			break;
		case Mean::Type::SAND:
			return model::Resource::SAND;
			break;
		case Mean::Type::ORGANICS:
			return model::Resource::ORGANICS;
			break;
		case Mean::Type::METAL:
			return model::Resource::METAL;
			break;
		case Mean::Type::SILICON:
			return model::Resource::SILICON;
			break;
		case Mean::Type::PLASTIC:
			return model::Resource::PLASTIC;
			break;
		case Mean::Type::CHIP:
			return model::Resource::CHIP;
			break;
		case Mean::Type::ACCUMULATOR:
			return model::Resource::ACCUMULATOR;
			break;
	}
} //
Mean::Type Mean::b2t(model::BuildingType t) {
	switch (t) {
		case model::BuildingType::QUARRY:
			return Mean::Type::QUARRY;
			break;
		case model::BuildingType::MINES:
			return Mean::Type::MINES;
			break;
		case model::BuildingType::CAREER:
			return Mean::Type::CAREER;
			break;
		case model::BuildingType::FARM:
			return Mean::Type::FARM;
			break;
		case model::BuildingType::FOUNDRY:
			return Mean::Type::FOUNDRY;
			break;
		case model::BuildingType::FURNACE:
			return Mean::Type::FURNACE;
			break;
		case model::BuildingType::BIOREACTOR:
			return Mean::Type::BIOREACTOR;
			break;
		case model::BuildingType::CHIP_FACTORY:
			return Mean::Type::CHIP_FACTORY;
			break;
		case model::BuildingType::ACCUMULATOR_FACTORY:
			return Mean::Type::ACCUMULATOR_FACTORY;
			break;
		case model::BuildingType::REPLICATOR:
			return Mean::Type::REPLICATOR;
			break;
	}
} //
Mean::Type Mean::r2t(model::Resource t) {
	switch (t) {
		case model::Resource::STONE:
			return Mean::Type::STONE;
			break;
		case model::Resource::ORE:
			return Mean::Type::ORE;
			break;
		case model::Resource::SAND:
			return Mean::Type::SAND;
			break;
		case model::Resource::ORGANICS:
			return Mean::Type::ORGANICS;
			break;
		case model::Resource::METAL:
			return Mean::Type::METAL;
			break;
		case model::Resource::SILICON:
			return Mean::Type::SILICON;
			break;
		case model::Resource::PLASTIC:
			return Mean::Type::PLASTIC;
			break;
		case model::Resource::CHIP:
			return Mean::Type::CHIP;
			break;
		case model::Resource::ACCUMULATOR:
			return Mean::Type::ACCUMULATOR;
			break;
	}
} //
Mean::Type Mean::eth2t(model::BuildingType t) {
	switch (t) {
		case model::BuildingType::QUARRY:
			return Mean::Type::QUARRY;
			break;
		case model::BuildingType::MINES:
			return Mean::Type::MINES;
			break;
		case model::BuildingType::CAREER:
			return Mean::Type::CAREER;
			break;
		case model::BuildingType::FARM:
			return Mean::Type::FARM;
			break;
		case model::BuildingType::FOUNDRY:
			return Mean::Type::FOUNDRY;
			break;
		case model::BuildingType::FURNACE:
			return Mean::Type::FURNACE;
			break;
		case model::BuildingType::BIOREACTOR:
			return Mean::Type::BIOREACTOR;
			break;
		case model::BuildingType::CHIP_FACTORY:
			return Mean::Type::CHIP_FACTORY;
			break;
		case model::BuildingType::ACCUMULATOR_FACTORY:
			return Mean::Type::ACCUMULATOR_FACTORY;
			break;
		case model::BuildingType::REPLICATOR:
			return Mean::Type::REPLICATOR;
			break;
	}
} //
Mean::Type Mean::eth2t(model::Resource t) {
	switch (t) {
		case model::Resource::STONE:
			return Mean::Type::STONE;
			break;
		case model::Resource::ORE:
			return Mean::Type::ORE;
			break;
		case model::Resource::SAND:
			return Mean::Type::SAND;
			break;
		case model::Resource::ORGANICS:
			return Mean::Type::ORGANICS;
			break;
		case model::Resource::METAL:
			return Mean::Type::METAL;
			break;
		case model::Resource::SILICON:
			return Mean::Type::SILICON;
			break;
		case model::Resource::PLASTIC:
			return Mean::Type::PLASTIC;
			break;
		case model::Resource::CHIP:
			return Mean::Type::CHIP;
			break;
		case model::Resource::ACCUMULATOR:
			return Mean::Type::ACCUMULATOR;
			break;
	}
} //
Mean::Type Mean::eth2t(int t) {
	if (t != -1) {
		cout << "ALL'S WRONG ACHTUNG" << endl;
	}
	return Mean::Type::none;
} //
vector<Mean::Type> Mean::getParents(Type t) {
	return vector<Mean::Type>(); //TODO реализовать
} //
vector<Mean::Type> Mean::getChildren(Type t) {
	return vector<Mean::Type>(); //TODO реализовать
} //
int Mean::type_table[NUM_TYPES][NUM_TYPES] = {}; //
bool Mean::isInitialised = false; //
void Mean::initialise(const model::Game& game) {
	for (int i = 0; i < NUM_TYPES; ++i) {
		for (int j = 0; j < NUM_TYPES; ++j) {
			Mean::type_table[i][j] = 0;
		}
	}
	for (pair<model::BuildingType, model::BuildingProperties> product: game.buildingProperties) {
		for (pair<model::Resource, int> condition: product.second.workResources) {
			Mean::type_table[(int) Mean::b2t(product.first)][(int) Mean::r2t(condition.first)] = condition.second;
		}
		for (pair<model::Resource, int> condition: product.second.buildResources) {
			Mean::type_table[(int) Mean::b2t(product.first)][(int) Mean::r2t(condition.first)] = condition.second;
		}
	}
	for (pair<model::BuildingType, model::BuildingProperties> producer: game.buildingProperties) {
		if (producer.second.produceResource.has_value()) {
			Mean::type_table[(int) Mean::r2t(producer.second.produceResource.value())][(int) Mean::b2t(producer.first)]
					= 1;
		}
		if (producer.second.produceWorker) {
			Mean::type_table[(int) Mean::Type::ROBOT][(int) Mean::b2t(producer.first)] = 1;
		}
	}
}

const vector<Mean::Type> Mean::res_list = {
		Mean::Type::STONE,
		Mean::Type::ORE,
		Mean::Type::SAND,
		Mean::Type::ORGANICS,
		Mean::Type::METAL,
		Mean::Type::SILICON,
		Mean::Type::PLASTIC,
		Mean::Type::CHIP,
		Mean::Type::ACCUMULATOR,
		Mean::Type::ROBOT,
}; //
const vector<double> Mean::res_price_list = {
		1,      // STONE
		2,      // ORE
		4,      // SAND
		8,      // ORGANICS
		20,     // METAL
		40,     // SILICON
		80,     // PLASTIC
		200,    // CHIP
		400,    // ACCUMULATOR
		800,    // ROBOT
};

Advert::Advert(PlSub& author, double p) : author(author), x(author.x), y(author.y), type(Mean::Type::none), p(p),
										  isAnswered(false) {
} //
Advert::Advert(PlSub& author, Mean::Type type, double p) : author(author), x(author.x), y(author.y), type(type), p(p),
														   isAnswered(false) {
} //
Supply::Supply(PlSub& author, double p) : Advert(author, p) {
}; //
Supply::Supply(PlSub& author, Mean::Type type, double p) : Advert(author, type, p) {
}; //
Demand::Demand(PlSub& author, bool isInternal, double p) : Advert(author, p), isInternal(isInternal) {
}; //
Demand::Demand(PlSub& author, Mean::Type type, bool isInternal, double p) : Advert(author, type, p),
																			isInternal(isInternal) {
}; //
PlSub::PlSub() : x(-1), y(-1), id(-1), mean(Mean::Type::none) {
	for (double& price: prices_sell) {
		price = 0;
	}
	for (double& price: prices_buy) {
		price = 0;
	}
} //
PlSub::PlSub(int x, int y, int id, Mean::Type mean) : x(x), y(y), id(id), mean(mean) {
	for (double& price: prices_sell) {
		price = 0;
	}
	for (double& price: prices_buy) {
		price = 0;
	}
} //
