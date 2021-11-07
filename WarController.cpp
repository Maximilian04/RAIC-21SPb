#include "WarController.h"

void WarController::setTarget(int i, Target newTarget)
{
    targets[i].number += newTarget.number;
    targets[i].importance = min(targets[i].importance, newTarget.importance);
}

void WarController::update(const model::Game& game, FlyingController& fc, Observer& observer)
{
    targets.assign(game.planets.size(), Target(0, LOWEST_IMPORTANCE));

    vector<pair<int, double>> bottlenecks;
    for (int i = 0; i < observer.size(); i++)
        if (observer.bottleneckTraffic[i] > 0)
            bottlenecks.push_back({i, observer.bottleneckTraffic[i]});
    
    int freeWarriors = 0;
    for (int i = 0; i < game.planets.size(); i++)
        freeWarriors += max(observer.ours[i] - fc.onFlightAt(i), 0);

    targets.clear();

    if (bottlenecks.size() == 0) // Enemy has flying controller
    {

    }
    else
    {
        sort(bottlenecks.begin(), bottlenecks.end(), [](pair<int, double> a, pair<int, double> b){return a.second > b.second;});

        int i = 0;
        while (freeWarriors > 0)
        {
            if (bottlenecks[i].second < 0.2)
                break;

            int numToSend = 10;
            numToSend = min(numToSend, freeWarriors);
            
            setTarget(bottlenecks[i].first, Target(10, 0));

            freeWarriors -= numToSend;
            
            i += 1;
            if (i >= bottlenecks.size() || i >= 7)
                break;
        }
    }

    vector<int> needed(game.planets.size(), 0);
    vector<int> localWarriors(game.planets.size(), 0);
    for (int i = 0; i < game.planets.size(); i++)
    {
        needed[i] = targets[i].number - fc.onFlightAt(i) - observer.ours[i];
        localWarriors[i] = observer.ours[i];
    }

    for (int j = 0; j < game.planets.size(); j++)
    {
        if (needed[j] <= 0)
            continue;
        
        for (int i = 0; i < game.planets.size(); i++)
        {            
            if (observer.ours[i] == 0)
                continue;
                
            int numToSend = min(min(localWarriors[i], needed[j]), -needed[i]);
            if (numToSend < 0)
                continue;

            fc.send(i, j, numToSend, nullopt, AVOIDANCE_WITH_ATTACK);

            needed[j] -= numToSend;
            localWarriors[i] -= numToSend;
            needed[i] += numToSend;

            if (localWarriors[i] == 0 || needed[i] >= 0)
                break;
        }
    }
}