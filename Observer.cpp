#include "Observer.h"

void Observer::setup(const model::Game& game)
{
    traffic.assign(game.planets.size(), 0);
    bottleneckTraffic.assign(game.planets.size(), 0);
    bottleneckTrafficPeak.assign(game.planets.size(), 0);

    bottleneckTrafficTimed.assign(game.planets.size(), vector<int>(OBSERVER_PERIOD, 0));

    int myTeamIndex = game.players[game.myIndex].teamIndex;
    for (int i = 0; i < game.players.size(); i++)
        if (game.players[i].teamIndex == myTeamIndex)
            alliesList.push_back(i);
}

void Observer::update(const model::Game& game, vector<vector<int>>& d)
{
    // Count enemies on every planet
    allies.assign(game.planets.size(), 0);
    ours.assign(game.planets.size(), 0);
    enemies.assign(game.planets.size(), 0);
    for (int i = 0; i < game.planets.size(); i++)
    {
        int enemiesNum = 0;
        for (model::WorkerGroup wg : game.planets[i].workerGroups)
            if (isEnemy(wg.playerIndex))
                enemiesNum += wg.number;

        enemies[i] = enemiesNum;

        int oursNum = 0;
        for (model::WorkerGroup wg : game.planets[i].workerGroups)
            if (wg.playerIndex == game.myIndex)
                oursNum += wg.number;
        
        ours[i] = oursNum;

        int alliesNum = 0;
        for (model::WorkerGroup wg : game.planets[i].workerGroups)
            if (isAlly(wg.playerIndex))
                alliesNum += wg.number;
        
        allies[i] = alliesNum;
    }

    // Normal traffic amounts (1st criterium)
    for (int i = 0; i < game.planets.size(); i++)
        traffic[i] = max(enemies[i] * 0.05 + traffic[i] * 0.95, enemies[i] * 0.5 + traffic[i] * 0.5);
    
    // Calculate bottleneck traffic       
    enemyFlyersTo.assign(game.planets.size(), 0); 
    vector<int> bottleneckTemp(game.planets.size(), 0);
    for (model::FlyingWorkerGroup fg : game.flyingWorkerGroups)
        if (isEnemy(fg.playerIndex))
        {
            enemyFlyersTo[fg.targetPlanet] += fg.number;
            
            if ((d[fg.departurePlanet][fg.targetPlanet] > game.maxTravelDistance || fg.nextPlanet != fg.targetPlanet) && fg.departureTick == game.currentTick - 1)
                bottleneckTemp[fg.nextPlanet] += fg.number;
        }
    
    for (int i = 0; i < game.planets.size(); i++)
    {
        // Remove oldest value and push current
        bottleneckTrafficTimed[i].erase(bottleneckTrafficTimed[i].begin());
        bottleneckTrafficTimed[i].push_back(bottleneckTemp[i]);

        // Calculate real bottleneck traffic value
        double avg = 0;
        for (int& value : bottleneckTrafficTimed[i])
            avg += value;
        avg /= bottleneckTrafficTimed[i].size();

        bottleneckTraffic[i] = avg;

        // Calculate peak bottleneck traffic value
        int peakValue = -1;
        for (int& value : bottleneckTrafficTimed[i])
            peakValue = max(value, peakValue);
        
        bottleneckTrafficPeak[i] = peakValue;
    }
}

// TODO: use flying groups to predict enemies number on planet (btw doesn`t really matter)
bool Observer::isOurs(int planet)
{
    return enemies[planet] == 0;
}

int Observer::size()
{
    return enemies.size();
