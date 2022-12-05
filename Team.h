//
// Created by tur22 on 05/12/2022.
//

#ifndef WET1MIVNE_MASTER_TEAM_H
#define WET1MIVNE_MASTER_TEAM_H


struct Team
{
    int teamId;
    int points;
    int playersCount;
    AVLTree<int, std::shared_ptr<Player>> teamPlayers;
    Team(int teamId, int points) : teamId(teamId),points(points),playersCount(0),teamPlayers(AVLTree<int, std::shared_ptr<Player>>)
    {

    }

};
#endif //WET1MIVNE_MASTER_TEAM_H
