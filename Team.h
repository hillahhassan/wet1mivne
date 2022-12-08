//
// Created by tur22 on 05/12/2022.
//

#ifndef WET1MIVNE_MASTER_TEAM_H
#define WET1MIVNE_MASTER_TEAM_H

#include "AVLTree.h"
#include <iostream>
#include <memory>
#include <stdio.h>
#include "Player.h"
typedef struct Player;
struct Team
{
    int teamId;
    int points;
    int totalGoals;
    int totalCards;
    int gamesPlayed;
    int gksCount;
    int playersCount;
    int t_topScorerId;
    int t_topScorerGoals;
    int t_topScorerCards;
    bool isKosher;

    AVLTree<int, std::shared_ptr<Player> > teamPlayers_byID;
    AVLTree<std::shared_ptr<Player> ,int> teamPlayers_byRank;

    Team(int teamId, int points) : teamId(teamId),
                                   points(points),
                                   totalGoals(0),
                                   totalCards(0),
                                   gamesPlayed(0),
                                   gksCount(0),
                                   playersCount(0),
                                   t_topScorerId(0),
                                   t_topScorerGoals(0),
                                   t_topScorerCards(0),
                                   isKosher(false),
                                   teamPlayers_byID(),
                                   teamPlayers_byRank()
    {
    }

};
#endif //WET1MIVNE_MASTER_TEAM_H
