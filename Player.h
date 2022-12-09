//
// Created by tur22 on 03/12/2022.
//

#ifndef WET1MIVNE_MASTER_PLAYER_H
#define WET1MIVNE_MASTER_PLAYER_H

#include <iostream>
#include <ostream>
#include <memory>
#include <stdio.h>
#include "Team.h"

struct Player {
    int playerId;
    int teamId;
    int gamesPlayed;
    int goals;
    int cards;
    int teamGamesPlayed_preAdd;
    bool goalKeeper;
    std::shared_ptr<Team> teamP;
    std::shared_ptr<Player> close_PrevPlayer;
    std::shared_ptr<Player> close_NextPlayer;

    Player(int playerId, int teamId, int gamesPlayed, int goals, int cards, bool goalKeeper, std::shared_ptr<Team> team)
            : playerId(playerId), teamId(teamId), gamesPlayed(gamesPlayed), goals(goals), cards(cards),
              goalKeeper(goalKeeper), teamP(team), close_PrevPlayer(nullptr), close_NextPlayer(nullptr) {
        teamGamesPlayed_preAdd = teamP->gamesPlayed;
    }

    Player() = default;

    bool operator<(const Player temp_player) const {
        if (goals < temp_player.goals) {
            return true;
        } else if (goals == temp_player.goals) {
            if (cards > temp_player.cards) {
                return true;
            } else if (cards == temp_player.cards) {
                if (playerId < temp_player.playerId) {
                    return true;
                }
            }
        }
        return false;
    }

    bool operator>(const Player temp_player) const {
        if (goals > temp_player.goals) {
            return true;
        } else if (goals == temp_player.goals) {
            if (cards < temp_player.cards) {
                return true;
            } else if (cards == temp_player.cards) {
                if (playerId > temp_player.playerId) {
                    return true;
                }
            }
        }
        return false;
    }

    bool operator==(const Player temp_player) const {

        if (goals == temp_player.goals) {
            if (cards == temp_player.cards) {
                if (playerId == temp_player.playerId) {
                    return true;
                }
            }
        }
        return false;
    }
};

#endif //WET1MIVNE_MASTER_PLAYER_H
