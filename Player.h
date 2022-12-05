//
// Created by tur22 on 03/12/2022.
//

#ifndef WET1MIVNE_MASTER_PLAYER_H
#define WET1MIVNE_MASTER_PLAYER_H

struct Player
{
    int playerId;
    int teamId;
    int gamesPlayed;
    int goals;
    int cards;
    bool goalKeeper;
    std::shared_ptr<Team>* teamP;
    Player(int playerId, int teamId, int gamesPlayed, int goals, int cards, int goalKeeper, std::shared_ptr<Team>* team)
        : playerId(playerId), teamId(teamId),gamesPlayed(gamesPlayed), goals(goals), cards(cards),
        goalKeeper(goalKeeper), teamP(team)
    {

    }
};

#endif //WET1MIVNE_MASTER_PLAYER_H
