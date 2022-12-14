#include "worldcup23a1.h"
#include "AVLTree.h"
#include "Team.h"
#include "Player.h"
#include <stdio.h>

//If the top scorer of team1 is better, return true. Else return false.
bool world_cup_t::cmprTeam_topScorer(std::shared_ptr<Team> team1, std::shared_ptr<Team> team2)
{
    if(team1->t_topScorerGoals > team2->t_topScorerGoals)
    {
        return true;
    }
    else if(team1->t_topScorerGoals < team2->t_topScorerGoals)
    {
        return false;
    }
    else
    {
        if(team1->t_topScorerCards < team2->t_topScorerCards)
        {
            return true;
        }
        else if(team1->t_topScorerCards > team2->t_topScorerCards)
        {
            return false;
        }
        else
        {
            return team1->t_topScorerId > team2->t_topScorerId;
        }
    }


}

world_cup_t::world_cup_t()
        : PlayersTree(), KosherTree(), amount_of_kosher(0), g_topScorerGoals(0), g_topScorerCards(0) {
}

world_cup_t::~world_cup_t() = default;

//Adds a new team to the world cup database.
StatusType world_cup_t::add_team(int teamId, int points) {
    if (teamId <= 0 || points < 0) {
        return StatusType::INVALID_INPUT;
    }

    std::shared_ptr<Team> new_Team(new (std::nothrow) Team(teamId, points));
    if(!new_Team)
    {
        return StatusType::ALLOCATION_ERROR;
    }
    try
    {

        if(TeamsTree.insert(teamId, new_Team) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
    }

    catch (const std::bad_alloc &) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

//Removes a team from the world cup, only if team is empty.
StatusType world_cup_t::remove_team(int teamId) {
    if (teamId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Team> team_to_remove;
    if (TeamsTree.find(teamId, &team_to_remove) != AVLTreeResult::AVL_TREE_SUCCESS) {
        return StatusType::FAILURE;
    }
    if (team_to_remove->playersCount == 0) {
        if (TeamsTree.remove(teamId) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return StatusType::FAILURE;
        }
        return StatusType::SUCCESS;
    }
    return StatusType::FAILURE;
}

//Adds player to the World Cup and to his respective team.
StatusType world_cup_t::add_player(int playerId, int teamId, int gamesPlayed,
                                   int goals, int cards, bool goalKeeper) {
    if (playerId <= 0 || teamId <= 0 || gamesPlayed < 0 || goals < 0 || cards < 0) {
        return StatusType::INVALID_INPUT;
    }
    if (gamesPlayed == 0 && (cards > 0 || goals > 0)) {
        return StatusType::INVALID_INPUT;
    }


    std::shared_ptr<Team> team_of_player;
    std::shared_ptr<Player> new_player_ptr;

    //Checks if team exists or player already exists.
    if (TeamsTree.find(teamId, &team_of_player) == AVL_TREE_DOES_NOT_EXIST) {
        return StatusType::FAILURE;
    }

    if (PlayersTree.find(playerId, &new_player_ptr) == AVL_TREE_ALREADY_EXISTS) {
        return StatusType::FAILURE;
    }

    try {
        new_player_ptr.reset(new Player(playerId, teamId, gamesPlayed, goals,
                                        cards, goalKeeper, team_of_player));
    }
    catch (const std::bad_alloc &) {
        return StatusType::ALLOCATION_ERROR;
    }

    bool player_inserted = false;
    bool playerTeam_inserted = false;
    bool ranking_inserted = false;
    bool teamRanking_inserted = false;

    //Amount of gamesPlayed of the team when player joined team.
    new_player_ptr->teamGamesPlayed_preAdd = team_of_player->gamesPlayed;
    try {
        if (PlayersTree.insert(playerId, new_player_ptr) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return StatusType::FAILURE;
        }
        player_inserted = true;

        if (team_of_player->teamPlayers_byID.insert(playerId, new_player_ptr) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return StatusType::FAILURE;
        }
        playerTeam_inserted = true;

        if (RankingTree.insert(*new_player_ptr, playerId) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return StatusType::FAILURE;
        }
        ranking_inserted = true;

        if (team_of_player->teamPlayers_byRank.insert(*new_player_ptr, playerId) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return StatusType::FAILURE;
        }
        teamRanking_inserted = true;

    }
    catch (const std::bad_alloc &) {
        if (player_inserted) {
            PlayersTree.remove(playerId);
        }
        if (playerTeam_inserted) {
            team_of_player->teamPlayers_byID.remove(playerId);
        }
        if (ranking_inserted) {
            RankingTree.remove(*new_player_ptr);
        }
        if (teamRanking_inserted) {
            team_of_player->teamPlayers_byRank.remove(*new_player_ptr);
        }
        if (new_player_ptr != nullptr) {
        }
        return StatusType::ALLOCATION_ERROR;
    }

    //Adds new player stats contribution to the team.
    team_of_player->totalGoals += goals;
    team_of_player->totalCards += cards;
    team_of_player->gksCount += goalKeeper;
    team_of_player->playersCount++;

    //Checks if team is now kosher.
    if (!team_of_player->isKosher) {
        if (team_of_player->gksCount >= 1 && team_of_player->playersCount >= 11) {
            KosherTree.insert(teamId, team_of_player);
            team_of_player->isKosher = true;
            amount_of_kosher++;
        }
    }

    //Checks for team new top scorer.
    if (goals > team_of_player->t_topScorerGoals ||
        (goals == team_of_player->t_topScorerGoals && (cards < team_of_player->t_topScorerCards ||
                                                       (cards == team_of_player->t_topScorerCards &&
                                                        playerId > team_of_player->t_topScorerId)))) {
        team_of_player->t_topScorerId = playerId;
        team_of_player->t_topScorerGoals = goals;
        team_of_player->t_topScorerCards = cards;
    }

    //Checks for match new top scorer.
    if (goals > g_topScorerGoals ||
        (goals == g_topScorerGoals && (cards < g_topScorerCards ||
                                       (cards == g_topScorerCards && playerId > g_topScorerID)))) {
        g_topScorerID = playerId;
        g_topScorerGoals = goals;
        g_topScorerCards = cards;
    }

    //Checking for new close players.
    Player *close_next = RankingTree.get_next_inorder(*new_player_ptr);
    Player *close_prev = RankingTree.get_prev_inorder(*new_player_ptr);

    if (close_next != nullptr) {
        std::shared_ptr<Player> newClosePlayer;
        PlayersTree.find(close_next->playerId, &newClosePlayer);
        new_player_ptr->close_NextPlayer = newClosePlayer;
        //Updates for neighbor ranked players
        if(newClosePlayer != nullptr)
        {
            newClosePlayer->close_PrevPlayer = new_player_ptr;
        }
    }

    if (close_prev != nullptr) {
        std::shared_ptr<Player> newClosePlayer;
        PlayersTree.find(close_prev->playerId, &newClosePlayer);
        new_player_ptr->close_PrevPlayer = newClosePlayer;
        //Updates for neighbor ranked players
        if(newClosePlayer != nullptr)
        {
            newClosePlayer->close_NextPlayer = new_player_ptr;
        }

    }

    //Incrementing players count in the world cup.
    g_playersCount++;

    return StatusType::SUCCESS;
}

//Removes a player from the world cup.
StatusType world_cup_t::remove_player(int playerId) {
    if (playerId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Player> player_to_remove;
    if (PlayersTree.find(playerId, &player_to_remove) != AVLTreeResult::AVL_TREE_SUCCESS) {
        return StatusType::FAILURE;
    }

    //Subtracts stats the player has contributed to the team.
    std::shared_ptr<Player> PrevPlayer = player_to_remove->close_PrevPlayer.lock();
    std::shared_ptr<Player> NextPlayer = player_to_remove->close_NextPlayer.lock();
    std::shared_ptr<Team> team_of_player = player_to_remove->teamP.lock();
    team_of_player->totalGoals -= player_to_remove->goals;
    team_of_player->totalCards -= player_to_remove->cards;
    team_of_player->gksCount -= player_to_remove->goalKeeper;
    team_of_player->playersCount--;

    //Checks if team is no longer kosher.
    if (team_of_player->isKosher) {
        if (team_of_player->playersCount < 11 || team_of_player->gksCount < 1) {
            KosherTree.remove(team_of_player->teamId);
            team_of_player->isKosher = false;
            amount_of_kosher--;
        }
    }

    //Removes player from his team inner players trees.
    Player *prevPlayer_in_team = team_of_player->teamPlayers_byRank.get_prev_inorder(*player_to_remove);
    team_of_player->teamPlayers_byID.remove(playerId);
    team_of_player->teamPlayers_byRank.remove(*player_to_remove);

    //checks for new team top scorer.
    if (playerId == team_of_player->t_topScorerId) {
        if (prevPlayer_in_team != nullptr) {
            team_of_player->t_topScorerId = prevPlayer_in_team->playerId;
            team_of_player->t_topScorerGoals = prevPlayer_in_team->goals;
            team_of_player->t_topScorerCards = prevPlayer_in_team->cards;
        } else {
            team_of_player->t_topScorerId = 0;
            team_of_player->t_topScorerGoals = 0;
            team_of_player->t_topScorerCards = 0;
        }
    }
    //removes player from database.
    PlayersTree.remove(playerId);
    RankingTree.remove(*player_to_remove);

    //Updates new close players.
    if (PrevPlayer != nullptr) {
        PrevPlayer->close_NextPlayer = NextPlayer;
    }

    if (NextPlayer != nullptr) {
        NextPlayer->close_PrevPlayer = PrevPlayer;
    }


    //Checks for new top scorer in the world cup.
    if (playerId == g_topScorerID && PrevPlayer != nullptr) {
        g_topScorerID = PrevPlayer->playerId;
        g_topScorerGoals = PrevPlayer->goals;
        g_topScorerCards = PrevPlayer->cards;
    }
    else
    {
        g_topScorerID = 0;
        g_topScorerGoals = 0;
        g_topScorerCards = 0;
    }




    g_playersCount--;
    return StatusType::SUCCESS;
}

StatusType world_cup_t::update_player_stats(int playerId, int gamesPlayed,
                                            int scoredGoals, int cardsReceived) {
    if (playerId <= 0 || gamesPlayed < 0 || scoredGoals < 0 || cardsReceived < 0) {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Player> player_to_update;
    if (PlayersTree.find(playerId, &player_to_update) == AVL_TREE_DOES_NOT_EXIST) {
        return StatusType::FAILURE;
    }
    try {

        // Remove the old player from the ranking tree
        RankingTree.remove(*player_to_update);

        // Update team statistics
        std::shared_ptr<Team> team_of_player = player_to_update->teamP.lock();

        // Remove the old player from the ranking tree
        team_of_player->teamPlayers_byRank.remove(*player_to_update);

        player_to_update->goals += scoredGoals;
        player_to_update->gamesPlayed += gamesPlayed;
        player_to_update->cards += cardsReceived;
        std::shared_ptr<Player> player_to_insert(new Player(*player_to_update));


        // Insert the new player with updated stats into the ranking tree
        RankingTree.insert(*player_to_insert, playerId);

        // Insert the new player with updated stats into the ranking tree
        team_of_player->teamPlayers_byRank.insert(*player_to_insert, playerId);

        team_of_player->totalGoals += scoredGoals;
        team_of_player->totalCards += cardsReceived;

        //Checks for new team top scorer.
        if(team_of_player->t_topScorerId == playerId)
        {
            team_of_player->t_topScorerGoals = player_to_update->goals;
            team_of_player->t_topScorerCards = player_to_update->cards;
        }
        else if(player_to_update->goals >= team_of_player->t_topScorerGoals)
        {
            if(player_to_update->goals == team_of_player->t_topScorerGoals) {
                if (player_to_update->cards <= team_of_player->t_topScorerCards) {
                    if (player_to_update->cards == team_of_player->t_topScorerCards) {
                        if (playerId > team_of_player->t_topScorerId) {
                            team_of_player->t_topScorerId = playerId;
                        }
                    }
                    else
                    {
                        team_of_player->t_topScorerId = playerId;
                        team_of_player->t_topScorerCards = player_to_update->cards;
                    }
                }
            }
            else
            {
                team_of_player->t_topScorerId = playerId;
                team_of_player->t_topScorerGoals = player_to_update->goals;
                team_of_player->t_topScorerCards = player_to_update->cards;
            }
        }

    }
    catch (const std::bad_alloc &) {
        return StatusType::ALLOCATION_ERROR;
    }

    std::shared_ptr<Player> temp_closeNext = player_to_update->close_NextPlayer.lock();
    std::shared_ptr<Player> temp_closePrev = player_to_update->close_PrevPlayer.lock();

    //Updates old close players.
    if (temp_closeNext != nullptr) {
        temp_closeNext->close_PrevPlayer = temp_closePrev;

    }
    if (temp_closePrev != nullptr) {
        temp_closePrev->close_NextPlayer = temp_closeNext;
    }


    //Fixing up new close players.
    Player *next_Player = RankingTree.get_next_inorder(*player_to_update);
    Player *prev_Player = RankingTree.get_prev_inorder(*player_to_update);

    if (next_Player != nullptr) {
        std::shared_ptr<Player> next_player_ToSave;
        PlayersTree.find(next_Player->playerId, &(next_player_ToSave));
        player_to_update->close_NextPlayer = next_player_ToSave;
        next_player_ToSave->close_PrevPlayer = player_to_update;
    } else
    {
        g_topScorerID = player_to_update->playerId;
        g_topScorerGoals = player_to_update->goals;
        g_topScorerCards = player_to_update->cards;
        player_to_update->close_NextPlayer.reset();
    }
    if (prev_Player != nullptr) {
        std::shared_ptr<Player> prev_player_ToSave;
        PlayersTree.find(prev_Player->playerId, &(prev_player_ToSave));
        player_to_update->close_PrevPlayer = prev_player_ToSave;
        prev_player_ToSave->close_NextPlayer = player_to_update;
    }
    else
    {
        player_to_update->close_NextPlayer.reset();
    }


    return StatusType::SUCCESS;
}

//Simulates a match between two teams. Calculates accordingly and updates the gamesPlayed and points of each team that
//played.
StatusType world_cup_t::play_match(int teamId1, int teamId2) {

    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2) {
        return StatusType::INVALID_INPUT;
    }

    std::shared_ptr<Team> team1;
    std::shared_ptr<Team> team2;

    //Checks if teams are kosher(eligible) for the match.
    if (KosherTree.find(teamId1, &team1) != AVLTreeResult::AVL_TREE_SUCCESS) {
        return StatusType::FAILURE;
    }
    if (KosherTree.find(teamId2, &team2) != AVLTreeResult::AVL_TREE_SUCCESS) {
        return StatusType::FAILURE;
    }

    //Sums each team score.
    int score_Team1 = team1->points + team1->totalGoals - team1->totalCards;
    int score_Team2 = team2->points + team2->totalGoals - team2->totalCards;

    //Updates team stats after the match ended.
    if (score_Team1 > score_Team2) {
        team1->points += 3;
    } else if (score_Team1 == score_Team2) {
        team1->points++;
        team2->points++;
    } else {
        team2->points += 3;
    }

    team1->gamesPlayed++;
    team2->gamesPlayed++;

    return StatusType::SUCCESS;
}

//Returns the number of games played of a specific player within the world cup.
output_t<int> world_cup_t::get_num_played_games(int playerId) {
    if (playerId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    std::shared_ptr<Player> player_to_get;
    if (PlayersTree.find(playerId, &player_to_get) != AVL_TREE_SUCCESS) {
        return output_t<int>(StatusType::FAILURE);
    }
    std::shared_ptr<Team> team_of_player = player_to_get->teamP.lock();
    int playerCalcGames = player_to_get->gamesPlayed + team_of_player->gamesPlayed
                          - player_to_get->teamGamesPlayed_preAdd;
    return output_t<int>(playerCalcGames);
}


//Returns the points a specific team currently has.
output_t<int> world_cup_t::get_team_points(int teamId) {
    if (teamId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    std::shared_ptr<Team> team_to_get;
    if (TeamsTree.find(teamId, &team_to_get) != AVLTreeResult::AVL_TREE_SUCCESS) {
        return output_t<int>(StatusType::FAILURE);
    }
    int teamPoints = team_to_get->points;
    return output_t<int>(teamPoints);
}

//Unites two teams into one.
StatusType world_cup_t::unite_teams(int teamId1, int teamId2, int newTeamId) {
    if (teamId2 <= 0 || teamId1 <= 0 || newTeamId <= 0 || teamId1 == teamId2) {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Team> team1;
    std::shared_ptr<Team> team2;
    if (TeamsTree.find(teamId1, &team1) != AVL_TREE_SUCCESS) {
        return StatusType::FAILURE;
    }
    if (TeamsTree.find(teamId2, &team2) != AVL_TREE_SUCCESS) {
        return StatusType::FAILURE;
    }

    //Sum of both teams points.
    int newPoints = team1->points + team2->points;

    //Checks if the ID is an entirely new ID or is an ID of one of the teams to unite. And then acts accordingly.
    if (teamId1 != newTeamId && teamId2 != newTeamId) {
        if (TeamsTree.contains(newTeamId) == AVL_TREE_SUCCESS) {

            return StatusType::FAILURE;
        }
        std::shared_ptr<Team> newTeam(new Team(newTeamId, newPoints));

        int *array_key_team1 = new int[team1->playersCount];
        std::shared_ptr<Player> *array_data_team1 = new std::shared_ptr<Player>[team1->playersCount];

        //Updates each player stats in the both teams to their new team.
        team1->teamPlayers_byID.to_sorted_keys_and_data(array_key_team1, array_data_team1);
        for (int i = 0; i < team1->playersCount; i++) {
            array_data_team1[i]->teamId = newTeamId;
            array_data_team1[i]->teamP = newTeam;
            array_data_team1[i]->gamesPlayed += team1->gamesPlayed - array_data_team1[i]->teamGamesPlayed_preAdd;
            array_data_team1[i]->teamGamesPlayed_preAdd = 0;

        }

        int *array_key_team2 = new int[team2->playersCount];
        std::shared_ptr<Player> *array_data_team2 = new std::shared_ptr<Player>[team2->playersCount];
        team2->teamPlayers_byID.to_sorted_keys_and_data(array_key_team2, array_data_team2);
        for (int i = 0; i < team2->playersCount; i++) {
            array_data_team2[i]->teamId = newTeamId;
            array_data_team2[i]->teamP = newTeam;
            array_data_team2[i]->gamesPlayed += team2->gamesPlayed - array_data_team2[i]->teamGamesPlayed_preAdd;
            array_data_team2[i]->teamGamesPlayed_preAdd = 0;
        }

        //Checks for new top scorer.
        if(cmprTeam_topScorer(team1,team2))
        {
            newTeam->t_topScorerId = team1->t_topScorerId;
            newTeam->t_topScorerGoals = team1->t_topScorerGoals;
            newTeam->t_topScorerCards = team1->t_topScorerCards;
        }
        else
        {
            newTeam->t_topScorerId = team2->t_topScorerId;
            newTeam->t_topScorerGoals = team2->t_topScorerGoals;
            newTeam->t_topScorerCards = team2->t_topScorerCards;
        }

        //Sums rest of important team stats.
        newTeam->totalGoals = team1->totalGoals + team2->totalGoals;
        newTeam->totalCards = team1->totalCards + team2->totalCards;
        newTeam->gksCount = team1->gksCount + team2->gksCount;
        newTeam->playersCount = team1->playersCount + team2->playersCount;

        //Merging both teams inner player trees.
        newTeam->teamPlayers_byID.build_from_two_merged_trees(team1->teamPlayers_byID, team2->teamPlayers_byID);
        newTeam->teamPlayers_byRank.build_from_two_merged_trees(team1->teamPlayers_byRank, team2->teamPlayers_byRank);

        if (TeamsTree.insert(newTeamId, newTeam) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return StatusType::FAILURE;
        }

        //Checks for kosher, and acts accordingly.
        if (newTeam->playersCount >= 11 && newTeam->gksCount >= 1) {
            newTeam->isKosher = true;
            KosherTree.insert(newTeamId, newTeam);
            amount_of_kosher++;
        }

        if (team1->isKosher) {
            KosherTree.remove(teamId1);
            amount_of_kosher--;
        }
        if (team2->isKosher) {
            KosherTree.remove(teamId2);
            amount_of_kosher--;
        }

        //Removing the old teams and deleting data.
        TeamsTree.remove(teamId1);
        TeamsTree.remove(teamId2);
        delete[] array_key_team1;
        delete[] array_data_team1;
        delete[] array_key_team2;
        delete[] array_data_team2;

    } else if (teamId1 == newTeamId) {
        int *array_key_team2 = new int[team2->playersCount];
        std::shared_ptr<Player> *array_data_team2 = new std::shared_ptr<Player>[team2->playersCount];

        //Updates each player stats in the team2.
        team2->teamPlayers_byID.to_sorted_keys_and_data(array_key_team2, array_data_team2);
        for (int i = 0; i < team2->playersCount; i++) {
            array_data_team2[i]->teamId = newTeamId;
            array_data_team2[i]->teamP = team1;
            array_data_team2[i]->gamesPlayed += team2->gamesPlayed - array_data_team2[i]->teamGamesPlayed_preAdd;
            array_data_team2[i]->teamGamesPlayed_preAdd = team1->gamesPlayed;
        }


        //Checks for new top scorer
        if(cmprTeam_topScorer(team2,team1))
        {
            team1->t_topScorerId = team2->t_topScorerId;
            team1->t_topScorerGoals = team2->t_topScorerGoals;
            team1->t_topScorerCards = team2->t_topScorerCards;
        }


        //Sums rest of the teams stats.
        team1->totalGoals += team2->totalGoals;
        team1->totalCards += team2->totalCards;
        team1->playersCount += team2->playersCount;
        team1->points = newPoints;
        team1->gksCount += team2->gksCount;

        team1->teamPlayers_byID.build_from_two_merged_trees(team1->teamPlayers_byID, team2->teamPlayers_byID);
        team1->teamPlayers_byRank.build_from_two_merged_trees(team1->teamPlayers_byRank, team2->teamPlayers_byRank);

        //Checks for Kosher.
        if (!(team1->isKosher) && team1->playersCount >= 11 && team1->gksCount >= 1) {
            team1->isKosher = true;
            KosherTree.insert(newTeamId, team1);
            amount_of_kosher++;
        }

        if (team2->isKosher) {
            KosherTree.remove(teamId2);
            amount_of_kosher--;
        }

        //Deleting old team from
        TeamsTree.remove(teamId2);

        delete[] array_key_team2;
        delete[] array_data_team2;

    } else if (teamId2 == newTeamId) {
        int *array_key_team1 = new int[team1->playersCount];
        std::shared_ptr<Player> *array_data_team1 = new std::shared_ptr<Player>[team1->playersCount];
        team1->teamPlayers_byID.to_sorted_keys_and_data(array_key_team1, array_data_team1);

        //Updates each player stats in the team1.
        for (int i = 0; i < team1->playersCount; i++) {
            array_data_team1[i]->teamId = newTeamId;
            array_data_team1[i]->teamP = team2;
            array_data_team1[i]->gamesPlayed += team1->gamesPlayed - array_data_team1[i]->teamGamesPlayed_preAdd;
            array_data_team1[i]->teamGamesPlayed_preAdd = team2->gamesPlayed;
        }

        //Checks for new top scorer
        if(cmprTeam_topScorer(team1,team2))
        {
            team2->t_topScorerId = team1->t_topScorerId;
            team2->t_topScorerGoals = team1->t_topScorerGoals;
            team2->t_topScorerCards = team1->t_topScorerCards;
        }

        //Sums rest of the teams stats.
        team2->totalGoals += team1->totalGoals;
        team2->totalCards += team1->totalCards;
        team2->playersCount += team1->playersCount;
        team2->points = newPoints;
        team2->gksCount += team1->gksCount;

        team2->teamPlayers_byID.build_from_two_merged_trees(team1->teamPlayers_byID, team2->teamPlayers_byID);
        team2->teamPlayers_byRank.build_from_two_merged_trees(team1->teamPlayers_byRank, team2->teamPlayers_byRank);


        //Checks for kosher.
        if (!(team2->isKosher) && team2->playersCount >= 11 && team2->gksCount >= 1) {
            team2->isKosher = true;
            KosherTree.insert(newTeamId, team2);
            amount_of_kosher++;
        }
        if (team1->isKosher) {
            KosherTree.remove(teamId1);
            amount_of_kosher--;
        }

        //Removing old team from the database.
        TeamsTree.remove(teamId1);

        delete[] array_key_team1;
        delete[] array_data_team1;
    }

    return StatusType::SUCCESS;
}

//Returns the top scorer of a team or the top scorer of the entire world cup
output_t<int> world_cup_t::get_top_scorer(int teamId) {
    if (teamId == 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    if (teamId < 0) {
        if (g_playersCount <= 0) {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(g_topScorerID);
    }

    if (teamId > 0) {
        std::shared_ptr<Team> team_to_get;
        if (TeamsTree.find(teamId, &team_to_get) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return output_t<int>(StatusType::FAILURE);
        }
        if (team_to_get->playersCount <= 0) {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(team_to_get->t_topScorerId);
    }
    return output_t<int>(StatusType::FAILURE);
}

//Returns the amount of players in a team or in the entire world cup.
output_t<int> world_cup_t::get_all_players_count(int teamId) {
    if (teamId == 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if (teamId < 0) {
        return output_t<int>(g_playersCount);
    }

    if (teamId > 0) {
        std::shared_ptr<Team> team_to_get;
        if (TeamsTree.find(teamId, &team_to_get) != AVLTreeResult::AVL_TREE_SUCCESS) {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(team_to_get->playersCount);
    }
    return output_t<int>(StatusType::FAILURE);
}

//returns all the playing that are playing in a team or the entire world cup.
StatusType world_cup_t::get_all_players(int teamId, int *const output) {

    if (teamId == 0 || output == nullptr) {
        return StatusType::INVALID_INPUT;
    }

    std::shared_ptr<Team> team_to_get;
    if (teamId > 0) {

        if (TeamsTree.find(teamId, &team_to_get) != AVL_TREE_SUCCESS) {
            return StatusType::FAILURE;
        }
        team_to_get->teamPlayers_byRank.to_sorted_keys_and_data(nullptr, output);
    }
    if (teamId < 0) {
        if (g_playersCount < 1) {
            return StatusType::FAILURE;
        }

        RankingTree.to_sorted_keys_and_data(nullptr, output);


    }
    return StatusType::SUCCESS;
}

//Gets the closest player id, ranking-wise, to desired player.
output_t<int> world_cup_t::get_closest_player(int playerId, int teamId) {
    if (playerId <= 0 || teamId <= 0) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    std::shared_ptr<Team> team_of_player;
    std::shared_ptr<Player> player;
    if (TeamsTree.find(teamId, &team_of_player) != AVLTreeResult::AVL_TREE_SUCCESS) {
        return output_t<int>(StatusType::FAILURE);
    }

    if (team_of_player->teamPlayers_byID.find(playerId, &player) != AVLTreeResult::AVL_TREE_SUCCESS ||
        g_playersCount <= 1) {
        return output_t<int>(StatusType::FAILURE);
    }

    std::shared_ptr<Player> closer_Prev = player->close_PrevPlayer.lock();
    std::shared_ptr<Player> closer_Next = player->close_NextPlayer.lock();

    //Starts comparing between 2 closest players, by rank.
    if (closer_Next.get() == NULL && closer_Prev.get() == NULL)
    {
        return output_t<int>(StatusType::FAILURE);
    }
    else if (closer_Next == NULL) {
        return output_t<int>(closer_Prev->playerId);
    } else if (closer_Prev == NULL) {
        return output_t<int>(closer_Next->playerId);
    } else {
        int Next_Goals_Diff = abs(closer_Next->goals - player->goals);
        int Prev_Goals_Diff = abs(player->goals - closer_Prev->goals);
        if (Next_Goals_Diff < Prev_Goals_Diff) {
            return output_t<int>(closer_Next->playerId);
        } else if (Next_Goals_Diff > Prev_Goals_Diff) {
            return output_t<int>(closer_Prev->playerId);
        } else {
            int Prev_Cards_Diff = abs(closer_Prev->cards - player->cards);
            int Next_Cards_Diff = abs(closer_Next->cards - player->cards);
            if (Next_Cards_Diff > Prev_Cards_Diff) {
                return output_t<int>(closer_Prev->playerId);
            } else if (Next_Cards_Diff < Prev_Cards_Diff) {
                return output_t<int>(closer_Next->playerId);
            } else {
                int Prev_ID_Diff = abs(closer_Prev->playerId - player->playerId);
                int Next_ID_Diff = abs(closer_Next->playerId - player->playerId);
                if (Next_ID_Diff > Prev_ID_Diff) {
                    return output_t<int>(closer_Prev->playerId);
                } else if (Next_ID_Diff < Prev_ID_Diff) {
                    return output_t<int>(closer_Next->playerId);
                } else {
                    if (closer_Prev->playerId > closer_Next->playerId) {
                        return output_t<int>(closer_Prev->playerId);
                    } else {
                        return output_t<int>(closer_Next->playerId);
                    }
                }
            }
        }
    }
    return output_t<int>(StatusType::FAILURE);

}

//determines a winner from a set ranged of teams that are eligible for matches.
output_t<int> world_cup_t::knockout_winner(int minTeamId, int maxTeamId) {

    if (minTeamId < 0 || maxTeamId < 0 || maxTeamId < minTeamId) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if (amount_of_kosher < 1) {
        return output_t<int>(StatusType::FAILURE);
    }
    //Extracts eligible teams keys and data into sorted arrays.
    int *key_array = new int[amount_of_kosher];
    std::shared_ptr<Team> *data_array = new std::shared_ptr<Team>[amount_of_kosher];
    int amount_of_eligible = 0;
    KosherTree.to_sorted_ranged_keys_and_data(key_array, data_array, minTeamId,maxTeamId, &amount_of_eligible);

    //Calculating team score based on point,cards and goals. And prepares it for the elimination rounds.
    int *eligible_Keys = new int[amount_of_eligible];
    int *eligible_points = new int[amount_of_eligible];
    int iptr = 0;
    for (int i = 0; i < amount_of_eligible; i++) {
        if (key_array[i] >= minTeamId && key_array[i] <= maxTeamId) {
            eligible_Keys[iptr] = key_array[i];
            eligible_points[iptr++] = data_array[i]->points + data_array[i]->totalGoals - data_array[i]->totalCards;
        }
        if (key_array[i] > maxTeamId) {
            i = amount_of_eligible;
        }
    }

    //if no eligible teams within our range.
    int eligible_Amount = iptr;
    if (eligible_Amount < 1) {
        delete[] key_array;
        delete[] data_array;
        delete[] eligible_Keys;
        delete[] eligible_points;
        return output_t<int>(StatusType::FAILURE);
    }

    //Eliminations round, each round it merges a losing team into the winner team and prepares it for next match.
    int winners_Amount = eligible_Amount;
    int prev_amount = winners_Amount;
    int *winners_keys = new int[winners_Amount];
    int *winners_points = new int[winners_Amount];
    int jptr = 0;
    while (prev_amount > 1) {

        jptr = 0;
        for (int j = 0; j < prev_amount; j++) {
            if (prev_amount <= j + 1) {
                winners_keys[jptr] = eligible_Keys[j];
                winners_points[jptr++] = eligible_points[j];
            } else if (eligible_points[j] > eligible_points[j + 1]) {
                winners_keys[jptr] = eligible_Keys[j];
                winners_points[jptr++] = eligible_points[j] + eligible_points[j + 1] + 3;
                j++;
            } else if (eligible_points[j] < eligible_points[j + 1]) {
                winners_keys[jptr] = eligible_Keys[j + 1];
                winners_points[jptr++] = eligible_points[j] + eligible_points[j + 1] + 3;
                j++;
            } else if (eligible_points[j] == eligible_points[j + 1]) {
                winners_keys[jptr] = eligible_Keys[j + 1];
                winners_points[jptr++] = eligible_points[j] + eligible_points[j + 1] + 3;
                j++;
            }
        }

        //Setting next round of matches for last round winners.
        int *eKeysPtr = eligible_Keys;
        int *ePointsPtr = eligible_points;
        eligible_Keys = winners_keys;
        eligible_points = winners_points;
        winners_keys = eKeysPtr;
        winners_points = ePointsPtr;
        prev_amount = jptr;
    }

    const int winner_id = eligible_Keys[0];

    //Data deletion
    delete[] eligible_Keys;
    delete[] eligible_points;
    delete[] winners_keys;
    delete[] winners_points;
    delete[] key_array;
    delete[] data_array;

    return output_t<int>(winner_id);

}

