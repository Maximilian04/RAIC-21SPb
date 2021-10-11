//
// Created by Maximilian on 07.10.2021.
//

#ifndef MYSTRATEGY_CPP_PLANET_H
#define MYSTRATEGY_CPP_PLANET_H


class Planet {
public:
	int id;
	int planetOwner; // 1 - we are, 0 - we aren't

	int building;
	int robotNumber; // >0 - our robots, <0 - enemy robot
};


#endif //MYSTRATEGY_CPP_PLANET_H
