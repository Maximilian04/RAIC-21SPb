#include "WarController.h"

void WarController::update(const model::Game& game, FlyingController& fc, Observer& observer)
{
    int enemyBase = 0;
    for (int i = 0; i < game.planets.size(); i++)
    {
        if (observer.enemies[i] + observer.enemyFlyersTo[i] > 
            observer.enemies[enemyBase] + observer.enemyFlyersTo[enemyBase])
            enemyBase = i;
    }

    for (int i = 0; i < game.planets.size(); i++)
    {
        if (observer.ours[i] == 0)
            continue;

        int numToSend = game.planets[i].workerGroups[0].number - fc.onFlightAt(i);

        if (numToSend > 0)
            fc.send(i, enemyBase, numToSend, nullopt, AVOIDANCE_WITH_ATTACK);
    }
}