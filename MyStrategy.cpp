#include "MyStrategy.hpp"
#include <exception>

#include <iostream>
#include <vector>

using namespace std;

#define NUM_TYPES 20 // number of types of means of production

class Mean { // class for means of production like robots, resources, buildings
public:
	enum class Type; // type of mean of production
	//!!!undefined behaviour with incorrect t!!!
	static model::BuildingType t2b(Type t); // translate Mean::Type to model::BuildingType
	static model::Resource t2r(Type t); // translate Mean::Type to model::Resource
	static Type b2t(model::BuildingType t); // translate model::BuildingType to Mean::Type
	static Type r2t(model::Resource t); // translate model::Resource to Mean::Type
	static vector<Type> getParents(Type t); //
	static vector<Type> getChildren(Type t); //
	static int type_table[NUM_TYPES][NUM_TYPES]; // таблица количеств необходимых ресурсов
	static bool isInitialised;
	static void initialise(const model::Game& game);

	Type type;
private:
};

MyStrategy::MyStrategy() {}

model::Action MyStrategy::getAction(const model::Game& game) {
	if (!Mean::isInitialised) { // Initialising
		Mean::initialise(game);
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
				for (int targetPlanet: targetPlanets) {
					moveActions.push_back(model::MoveAction(i, targetPlanet, workerGroup.number / targetPlanets.size(),
															optional<model::Resource>(model::Resource::STONE)));
				}
			}
		}
	}

	return model::Action(moveActions, buildingActions);
}

enum class Mean::Type { // it's necessary, what STONE has 0 number!!!
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
vector<Mean::Type> Mean::getParents(Type t) {
	return vector<Mean::Type>(); //TODO реализовать
} //
vector<Mean::Type> Mean::getChildren(Type t) {
	return vector<Mean::Type>(); //TODO реализовать
} //
int Mean::type_table[NUM_TYPES][NUM_TYPES] = {};
bool Mean::isInitialised = false;
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
			Mean::type_table[(int) Mean::r2t(producer.second.produceResource.value())][(int) Mean::b2t(producer.first)] = 1;
		}
		if (producer.second.produceWorker) {
			Mean::type_table[(int) Mean::Type::ROBOT][(int) Mean::b2t(producer.first)] = 1;
		}
	}
}