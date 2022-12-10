#include "worldcup23a1.h"
#include "AVLTree.h"
#include "Team.h"
#include "Player.h"
#include <stdio.h>
/// Insert AVLTree<int, std::shared_ptr<Team>>(), AVLTree<std::shared_ptr<Player>, int>()

world_cup_t::world_cup_t() : PlayersTree(),KosherTree(), amount_of_kosher(0),g_topScorerGoals(0),g_topScorerCards(0)
{
}

world_cup_t::~world_cup_t() = default;
/*{


}*/


StatusType world_cup_t::add_team(int teamId, int points)
{
    if (teamId <= 0 || points < 0)
    {
        return StatusType::INVALID_INPUT;
    }

    std::shared_ptr<Team> new_team_ptr;
    try
    {
        new_team_ptr.reset(new Team(teamId, points));
        if(TeamsTree.insert(teamId, new_team_ptr) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
    }

    catch (const std::bad_alloc &)
    {
        new_team_ptr.reset();
        return StatusType::ALLOCATION_ERROR;
    }

	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_team(int teamId)
{
    if(teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Team> team_to_remove;
    if(TeamsTree.find(teamId, &team_to_remove) != AVLTreeResult::AVL_TREE_SUCCESS)
    {
        return StatusType::FAILURE;
    }
    if(team_to_remove->playersCount == 0)
    {
        if(TeamsTree.remove(teamId) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
        return StatusType::SUCCESS;
    }
    return StatusType::FAILURE;
}

StatusType world_cup_t::add_player(int playerId, int teamId, int gamesPlayed,
                                   int goals, int cards, bool goalKeeper) {
    if (playerId <= 0 || teamId <= 0 || gamesPlayed < 0 || goals < 0 || cards < 0)
    {
        return StatusType::INVALID_INPUT;
    }
    if (gamesPlayed == 0 && (cards > 0 || goals > 0))
    {
        return StatusType::INVALID_INPUT;
    }


    std::shared_ptr<Team> team_of_player;
    std::shared_ptr<Player> new_player_ptr;

    if(TeamsTree.find(teamId,&team_of_player) == AVL_TREE_DOES_NOT_EXIST)
    {
        return StatusType::FAILURE;
    }

    if (PlayersTree.find(playerId,&new_player_ptr) == AVL_TREE_ALREADY_EXISTS)
    {
        return StatusType::FAILURE;
    }

    try
    {
        new_player_ptr.reset(new Player(playerId, teamId, gamesPlayed, goals,
                                              cards, goalKeeper, team_of_player));
    }
    catch (const std::bad_alloc &)
    {
        return StatusType::ALLOCATION_ERROR;
    }

    bool player_inserted = false;
    bool playerTeam_inserted = false;
    bool ranking_inserted = false;
    bool teamRanking_inserted = false;
    new_player_ptr->teamGamesPlayed_preAdd = team_of_player->gamesPlayed;
    try
    {
        if(PlayersTree.insert(playerId, new_player_ptr) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
        player_inserted = true;

        if(team_of_player->teamPlayers_byID.insert(playerId, new_player_ptr) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
        playerTeam_inserted = true;

        if(RankingTree.insert(*new_player_ptr,playerId) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
        ranking_inserted = true;

        if(team_of_player->teamPlayers_byRank.insert(*new_player_ptr,playerId) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
        teamRanking_inserted = true;

    }
    catch (const std::bad_alloc &)
    {
        if(player_inserted)
        {
            PlayersTree.remove(playerId);
        }
        if(playerTeam_inserted)
        {
            team_of_player->teamPlayers_byID.remove(playerId);
        }
        if(ranking_inserted)
        {
            RankingTree.remove(*new_player_ptr);
        }
        if(teamRanking_inserted)
        {
            team_of_player->teamPlayers_byRank.remove(*new_player_ptr);
        }
        if(new_player_ptr != nullptr) {
        }
        return StatusType::ALLOCATION_ERROR;
    }

    team_of_player->totalGoals += goals;
    team_of_player->totalCards += cards;
    team_of_player->gksCount += goalKeeper;
    team_of_player->playersCount++;
    if(!team_of_player->isKosher)
    {
        if(team_of_player->gksCount >= 1 && team_of_player->playersCount >= 11)
        {
            KosherTree.insert(teamId,team_of_player);
            team_of_player->isKosher = true;
            amount_of_kosher++;
        }
    }
    if(goals > team_of_player->t_topScorerGoals ||
        (goals == team_of_player->t_topScorerGoals && (cards < team_of_player->t_topScorerCards ||
        (cards == team_of_player->t_topScorerCards && playerId > team_of_player->t_topScorerId))))
    {
        team_of_player->t_topScorerId = playerId;
        team_of_player->t_topScorerGoals = goals;
        team_of_player->t_topScorerCards = cards;
    }

    if(goals > g_topScorerGoals ||
       (goals == g_topScorerGoals && (cards < g_topScorerCards ||
                                                  (cards == g_topScorerCards && playerId > g_topScorerID))))
    {
        g_topScorerID = playerId;
        g_topScorerGoals = goals;
        g_topScorerCards = cards;
    }

    Player* close_next = RankingTree.get_next_inorder(*new_player_ptr);
    Player* close_prev = RankingTree.get_prev_inorder(*new_player_ptr);
    if(close_next != nullptr)
    {
        PlayersTree.find(close_next->playerId, &new_player_ptr->close_NextPlayer);
    }

    if(close_prev != nullptr)
    {
        PlayersTree.find(close_prev->playerId, &new_player_ptr->close_PrevPlayer);
    }


    //Updates for neighbor ranked players
    if(new_player_ptr->close_PrevPlayer != nullptr)
    {
        new_player_ptr->close_PrevPlayer->close_NextPlayer = new_player_ptr;
    }

    if(new_player_ptr->close_NextPlayer != nullptr)
    {
        new_player_ptr->close_NextPlayer->close_PrevPlayer = new_player_ptr;
    }



    g_playersCount++;

    //End of redundant
	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_player(int playerId)
{
    if(playerId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Player> player_to_remove;
    if(PlayersTree.find(playerId,&player_to_remove) != AVLTreeResult::AVL_TREE_SUCCESS)
    {
        return StatusType::FAILURE;
    }

    std::shared_ptr<Player> PrevPlayer = player_to_remove->close_PrevPlayer;
    std::shared_ptr<Player> NextPlayer = player_to_remove->close_NextPlayer;
    std::shared_ptr<Team> team_of_player = player_to_remove->teamP;
    team_of_player->totalGoals -= player_to_remove->goals;
    team_of_player->totalCards -= player_to_remove->cards;
    team_of_player->gksCount -= player_to_remove->goalKeeper;

    if(team_of_player->isKosher)
    {
        if(team_of_player->playersCount < 11 || team_of_player->gksCount < 1)
        {
            KosherTree.remove(team_of_player->teamId);
            team_of_player->isKosher = false;
            amount_of_kosher--;
        }
    }

    team_of_player->teamPlayers_byID.remove(playerId);
    team_of_player->teamPlayers_byRank.remove(*player_to_remove);
    if(playerId == team_of_player->t_topScorerId)
    {
        if(PrevPlayer != nullptr) {
            team_of_player->t_topScorerId = PrevPlayer->playerId;
            team_of_player->t_topScorerGoals = PrevPlayer->goals;
            team_of_player->t_topScorerCards = PrevPlayer->cards;
        }
        else
        {
            team_of_player->t_topScorerId = 0;
            team_of_player->t_topScorerGoals = 0;
            team_of_player->t_topScorerCards = 0;
        }
    }
    PlayersTree.remove(playerId);
    RankingTree.remove(*player_to_remove);

    //PlayersTree.find(NextPlayerId,NextPlayer);
    //PlayersTree.find(PrevPlayerId,PrevPlayer);
    //RankingTree.get_next_inorder(PrevPlayer)
    //RankingTree.get_prev_inorder(NextPlayer)
    if (PrevPlayer) {
        PrevPlayer->close_NextPlayer = NextPlayer;
    }

    if (NextPlayer) {
        NextPlayer->close_PrevPlayer = PrevPlayer;
    }

    
    if(!NextPlayer && PrevPlayer)
    {
        g_topScorerID = PrevPlayer->playerId;
        g_topScorerGoals = PrevPlayer->goals;
        g_topScorerCards = PrevPlayer->cards;
    }
    

    team_of_player->playersCount--;

    g_playersCount--;
	return StatusType::SUCCESS;
}

StatusType world_cup_t::update_player_stats(int playerId, int gamesPlayed,
                                        int scoredGoals, int cardsReceived)
{
    if(playerId <= 0 || gamesPlayed < 0 || scoredGoals<0 || cardsReceived < 0)
    {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Player> player_to_update;
    if(PlayersTree.find(playerId,&player_to_update) == AVL_TREE_DOES_NOT_EXIST)
    {
        return StatusType::FAILURE;
    }
    try
    {





        // Remove the old player from the ranking tree
        RankingTree.remove(*player_to_update);



        // Update team statistics
        std::shared_ptr<Team> team_of_player = player_to_update->teamP;

        // Remove the old player from the ranking tree
        team_of_player->teamPlayers_byRank.remove(*player_to_update);

        player_to_update->goals += scoredGoals;
        player_to_update->gamesPlayed += gamesPlayed;
        player_to_update->cards += cardsReceived;
        std::shared_ptr<Player> player_to_insert(new Player(*player_to_update));


        // Insert the new player with updated stats into the ranking tree
        RankingTree.insert(*player_to_insert,playerId);

        // Insert the new player with updated stats into the ranking tree
        team_of_player->teamPlayers_byRank.insert(*player_to_insert,playerId);

        team_of_player->totalGoals += scoredGoals;
        team_of_player->totalCards += cardsReceived;

    }
    catch (const std::bad_alloc &)
    {
        return StatusType::ALLOCATION_ERROR;
    }
    if(player_to_update->close_NextPlayer != nullptr)
    {
        std::shared_ptr<Player> old_prev_player(player_to_update->close_NextPlayer->close_PrevPlayer);
        if(player_to_update->close_PrevPlayer != nullptr)
        {
            player_to_update->close_PrevPlayer->close_NextPlayer = old_prev_player;
        }

    }

    if(player_to_update->close_NextPlayer != nullptr)
    {
        if(player_to_update->close_PrevPlayer != nullptr) {
            player_to_update->close_NextPlayer->close_PrevPlayer = player_to_update->close_PrevPlayer->close_NextPlayer;
        }
    }


    Player* next_Player = RankingTree.get_next_inorder(*player_to_update);
    Player* prev_Player = RankingTree.get_prev_inorder(*player_to_update);
    if(next_Player != nullptr)
    {
        PlayersTree.find(next_Player->playerId,&(player_to_update->close_NextPlayer));
    }
    if(prev_Player != nullptr) {
        PlayersTree.find(prev_Player->playerId, &(player_to_update->close_PrevPlayer));
    }
    if(player_to_update->close_PrevPlayer != nullptr)
    {
        player_to_update->close_PrevPlayer->close_NextPlayer = player_to_update;
    }

    if(player_to_update->close_NextPlayer != nullptr) {
        player_to_update->close_NextPlayer->close_PrevPlayer = player_to_update;
    }
    else
    {
        g_topScorerID = player_to_update->playerId;
        g_topScorerGoals = player_to_update->goals;
        g_topScorerCards = player_to_update->cards;
    }


	return StatusType::SUCCESS;
}

StatusType world_cup_t::play_match(int teamId1, int teamId2)
{

    if(teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2)
    {
        return StatusType::INVALID_INPUT;
    }

    std::shared_ptr<Team> team1;
    std::shared_ptr<Team> team2;
    if(KosherTree.find(teamId1, &team1) != AVLTreeResult::AVL_TREE_SUCCESS)
    {
        return StatusType::FAILURE;
    }
    if(KosherTree.find(teamId2, &team2) != AVLTreeResult::AVL_TREE_SUCCESS)
    {
        return StatusType::FAILURE;
    }

    int score_Team1 = team1->points + team1->totalGoals - team1->totalCards;
    int score_Team2 = team2->points + team2->totalGoals - team2->totalCards;

    if(score_Team1 > score_Team2)
    {
        team1->points += 3;
    }
    else if(score_Team1 == score_Team2)
    {
        team1->points++;
        team2->points++;
    }
    else
    {
        team2->points += 3;
    }

    team1->gamesPlayed++;
    team2->gamesPlayed++;

	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_num_played_games(int playerId)
{
    if (playerId <= 0)
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    std::shared_ptr<Player> player_to_get;
    if(PlayersTree.find(playerId,&player_to_get) != AVL_TREE_SUCCESS)
    {
        return output_t<int>(StatusType::FAILURE);
    }
    int playerCalcGames = player_to_get->gamesPlayed + player_to_get->teamP->gamesPlayed
            - player_to_get->teamGamesPlayed_preAdd;
    return output_t<int>(playerCalcGames);
}

output_t<int> world_cup_t::get_team_points(int teamId)
{
    if (teamId <= 0)
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    std::shared_ptr<Team> team_to_get;
    if(TeamsTree.find(teamId,&team_to_get) != AVLTreeResult::AVL_TREE_SUCCESS)
    {
        return output_t<int>(StatusType::FAILURE);
    }
    int teamPoints = team_to_get->points;
    return output_t<int>(teamPoints);
}

StatusType world_cup_t::unite_teams(int teamId1, int teamId2, int newTeamId)
{
    if(teamId2 <= 0 || teamId1 <= 0 || newTeamId <= 0 || teamId1 == teamId2)
    {
        return StatusType::INVALID_INPUT;
    }
    std::shared_ptr<Team> team1;
    std::shared_ptr<Team> team2;
    if(TeamsTree.find(teamId1,&team1) != AVL_TREE_SUCCESS)
    {
        return StatusType::FAILURE;
    }
    if(TeamsTree.find(teamId2,&team2) != AVL_TREE_SUCCESS)
    {
        return StatusType::FAILURE;
    }
    int newPoints = team1->points + team2->points;
    std::shared_ptr<Team> newTeam(new Team(newTeamId, newPoints));
    if(teamId1 != newTeamId && teamId2 != newTeamId) {
        if (TeamsTree.contains(newTeamId) == AVL_TREE_SUCCESS) {

            return StatusType::FAILURE;
        }


        int * array_key_team1 = new int[team1->playersCount];
        std::shared_ptr<Player> * array_data_team1 = new std::shared_ptr<Player>[team1->playersCount];
        team1->teamPlayers_byID.to_sorted_keys_and_data(array_key_team1,array_data_team1);
        for(int i=0;i<team1->playersCount;i++)
        {
            array_data_team1[i]->teamId = newTeamId;
            array_data_team1[i]->teamP = newTeam;
            array_data_team1[i]->gamesPlayed += team1->gamesPlayed - array_data_team1[i]->teamGamesPlayed_preAdd;
        }

        int * array_key_team2 = new int[team2->playersCount];
        std::shared_ptr<Player> * array_data_team2 = new std::shared_ptr<Player>[team2->playersCount];
        team2->teamPlayers_byID.to_sorted_keys_and_data(array_key_team2,array_data_team2);
        for(int i=0;i<team2->playersCount;i++)
        {
            array_data_team2[i]->teamId = newTeamId;
            array_data_team2[i]->teamP = newTeam;
            array_data_team2[i]->gamesPlayed += team2->gamesPlayed - array_data_team2[i]->teamGamesPlayed_preAdd;
        }
        newTeam->totalGoals = team1->totalGoals + team2->totalGoals;
        newTeam->totalCards = team1->totalCards + team2->totalCards;
        newTeam->gksCount = team1->gksCount + team2->gksCount;
        newTeam->playersCount = team1->playersCount + team2->playersCount;
        
        newTeam->teamPlayers_byID.build_from_two_merged_trees(team1->teamPlayers_byID ,team2->teamPlayers_byID);
        newTeam->teamPlayers_byRank.build_from_two_merged_trees(team1->teamPlayers_byRank ,team2->teamPlayers_byRank);

         if(TeamsTree.insert(newTeamId,newTeam) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
        if(newTeam->playersCount >= 11 && newTeam->gksCount >= 1)
        {
            newTeam->isKosher = true;
            KosherTree.insert(newTeamId,newTeam);
            amount_of_kosher++;
        }

        if(team1->isKosher)
        {
            KosherTree.remove(teamId1);
            amount_of_kosher--;
        }
        if(team2->isKosher)
        {
            KosherTree.remove(teamId2);
            amount_of_kosher--;
        }
        TeamsTree.remove(teamId1);
        TeamsTree.remove(teamId2);
        delete[] array_key_team1;
        delete[] array_data_team1;
        delete[] array_key_team2;
        delete[] array_data_team2;
    }

    else if(teamId1 == newTeamId)
    {
        int * array_key_team2 = new int[team2->playersCount];
        std::shared_ptr<Player> *array_data_team2 = new std::shared_ptr<Player>[team2->playersCount];
        team2->teamPlayers_byID.to_sorted_keys_and_data(array_key_team2,array_data_team2);
        for(int i=0;i<team2->playersCount;i++)
        {
            array_data_team2[i]->teamId = newTeamId;
            array_data_team2[i]->teamP = newTeam;
            array_data_team2[i]->gamesPlayed += team2->gamesPlayed - array_data_team2[i]->teamGamesPlayed_preAdd;
        }
        team1->totalGoals += team2->totalGoals;
        team1->totalCards += team2->totalCards;
        team1->playersCount += team2->playersCount;
        team1->gksCount += team2->gksCount;

        team1->teamPlayers_byID.build_from_two_merged_trees(team1->teamPlayers_byID,team2->teamPlayers_byID);
        team1->teamPlayers_byRank.build_from_two_merged_trees(team1->teamPlayers_byRank,team2->teamPlayers_byRank);

        if(!(team1->isKosher) && team1->playersCount >= 11 && team1->gksCount >= 1)
        {
            team1->isKosher = true;
            KosherTree.insert(newTeamId,team1);
            amount_of_kosher++;
        }

        if(team2->isKosher)
        {
            KosherTree.remove(teamId2);
            amount_of_kosher--;
        }
        TeamsTree.remove(teamId2);


        delete[] array_key_team2;
        delete[] array_data_team2;
    }
    else if(teamId2 == newTeamId)
    {
        int * array_key_team1 = new int[team1->playersCount];
        std::shared_ptr<Player> * array_data_team1 = new std::shared_ptr<Player>[team1->playersCount];
        team1->teamPlayers_byID.to_sorted_keys_and_data(array_key_team1,array_data_team1);
        for(int i=0;i<team1->playersCount;i++)
        {
            array_data_team1[i]->teamId = newTeamId;
            array_data_team1[i]->teamP = newTeam;
            array_data_team1[i]->gamesPlayed += team1->gamesPlayed - array_data_team1[i]->teamGamesPlayed_preAdd;
        }


        team2->totalGoals += team1->totalGoals;
        team2->totalCards += team1->totalCards;
        team2->playersCount += team1->playersCount;
        team2->gksCount += team1->gksCount;

        team2->teamPlayers_byID.build_from_two_merged_trees(team1->teamPlayers_byID, team2->teamPlayers_byID);
        team2->teamPlayers_byRank.build_from_two_merged_trees(team1->teamPlayers_byRank, team2->teamPlayers_byRank);

        if(!(team2->isKosher) && team2->playersCount >= 11 && team2->gksCount >= 1)
        {
            team2->isKosher = true;
            KosherTree.insert(newTeamId,team2);
            amount_of_kosher++;
        }
        TeamsTree.remove(teamId1);

        if(team1->isKosher)
        {
            KosherTree.remove(teamId1);
            amount_of_kosher--;
        }

        delete[] array_key_team1;
        delete[] array_data_team1;
    }

	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_top_scorer(int teamId)
{
    if(teamId == 0)
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    if(teamId < 0)
    {
        if (g_playersCount <= 0)
        {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(g_topScorerID);
    }

    if(teamId > 0)
    {
        std::shared_ptr<Team> team_to_get;
        if(TeamsTree.find(teamId,&team_to_get) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return output_t<int>(StatusType::FAILURE);
        }
        if (team_to_get->playersCount <= 0)
        {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(team_to_get->t_topScorerId);
    }
    return output_t<int>(StatusType::FAILURE);
}

output_t<int> world_cup_t::get_all_players_count(int teamId)
{
    if(teamId == 0)
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if(teamId < 0)
    {
        return output_t<int>(g_playersCount);
    }

    if(teamId > 0)
    {
        std::shared_ptr<Team> team_to_get;
        if(TeamsTree.find(teamId,&team_to_get) != AVLTreeResult::AVL_TREE_SUCCESS)
        {
            return output_t<int>(StatusType::FAILURE);
        }
        return output_t<int>(team_to_get->playersCount);
    }
    return output_t<int>(StatusType::FAILURE);
}

StatusType world_cup_t::get_all_players(int teamId, int *const output)
{

    if(teamId == 0 || output == nullptr)
    {
        return StatusType::INVALID_INPUT;
    }

    std::shared_ptr<Team> team_to_get;
    if(teamId > 0)
    {

        if(TeamsTree.find(teamId,&team_to_get) != AVL_TREE_SUCCESS)
        {
            return StatusType::FAILURE;
        }
        Player *key_array = new Player[team_to_get->playersCount];
        team_to_get->teamPlayers_byRank.to_sorted_keys_and_data(nullptr,output);
        delete[] key_array;
    }
    if(teamId < 0)
    {
        if(g_playersCount < 1)
        {
            return StatusType::FAILURE;
        }
        Player *key_array = new Player[g_playersCount];

        RankingTree.to_sorted_keys_and_data(nullptr,output);

        delete[] key_array;
    }
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_closest_player(int playerId, int teamId)
{
    if(playerId <= 0 || teamId <= 0)
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    std::shared_ptr<Team> team_of_player;
    std::shared_ptr<Player> player;
    if(TeamsTree.find(teamId,&team_of_player) != AVLTreeResult::AVL_TREE_SUCCESS )
    {
        return output_t<int>(StatusType::FAILURE);
    }

    if(team_of_player->teamPlayers_byID.find(playerId,&player) != AVLTreeResult::AVL_TREE_SUCCESS || g_playersCount <= 1)
    {
        return output_t<int>(StatusType::FAILURE);
    }


    if(player->close_NextPlayer == nullptr)
    {
        return output_t<int>(player->close_PrevPlayer->playerId);
    }
    else if(player->close_PrevPlayer == nullptr)
    {
        return output_t<int>(player->close_NextPlayer->playerId);
    }
    else
    {
        int Next_Goals_Diff = abs(player->close_NextPlayer->goals - player->goals);
        int Prev_Goals_Diff = abs(player->goals - player->close_PrevPlayer->goals);
        if(Next_Goals_Diff < Prev_Goals_Diff)
        {
            return output_t<int>(player->close_NextPlayer->playerId);
        }
        else if(Next_Goals_Diff > Prev_Goals_Diff)
        {
            return output_t<int>(player->close_PrevPlayer->playerId);
        }
        else
        {
            int Prev_Cards_Diff = abs(player->close_PrevPlayer->cards - player->cards);
            int Next_Cards_Diff = abs(player->close_NextPlayer->cards - player->cards);
            if(Next_Cards_Diff > Prev_Cards_Diff)
            {
                return output_t<int>(player->close_NextPlayer->playerId);
            }
            else if(Next_Cards_Diff < Prev_Cards_Diff)
            {
                return output_t<int>(player->close_PrevPlayer->playerId);
            }
            else{
                int Prev_ID_Diff = abs(player->close_PrevPlayer->playerId - player->playerId);
                int Next_ID_Diff = abs(player->close_NextPlayer->playerId - player->playerId);
                if(Next_ID_Diff > Prev_ID_Diff)
                {
                    return output_t<int>(player->close_NextPlayer->playerId);
                }
                else if(Next_ID_Diff < Prev_ID_Diff)
                {
                    return output_t<int>(player->close_PrevPlayer->playerId);
                }
                else
                {
                    if(player->close_PrevPlayer->playerId > player->close_NextPlayer->playerId)
                    {
                        return output_t<int>(player->close_PrevPlayer->playerId);
                    }
                    else
                    {
                        return output_t<int>(player->close_NextPlayer->playerId);
                    }
                }
            }
        }
    }

}

output_t<int> world_cup_t::knockout_winner(int minTeamId, int maxTeamId)
{

    if(minTeamId < 0 || maxTeamId < 0 || maxTeamId < minTeamId )
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    if(amount_of_kosher < 1)
    {
        return output_t<int>(StatusType::FAILURE);
    }
    int* key_array = new int[amount_of_kosher];
    std::shared_ptr<Team>* data_array = new std::shared_ptr<Team>[amount_of_kosher];
    KosherTree.to_sorted_keys_and_data(key_array,data_array);
    const int keys_length = amount_of_kosher;
    int *eligible_Keys = new int[keys_length];
    int *eligible_points = new int[keys_length];
    int iptr = 0;
    for(int i=0;i<keys_length;i++)
    {
        if(key_array[i] >= minTeamId && key_array[i] <= maxTeamId)
        {
            eligible_Keys[iptr] = key_array[i];
            eligible_points[iptr++] = data_array[i]->points + data_array[i]->totalGoals - data_array[i]->totalCards;
        }
        if(key_array[i] > maxTeamId)
        {
            i = keys_length;
        }
    }
    int eligible_Amount = iptr;
    if(eligible_Amount < 1)
    {
        delete[] key_array;
        delete[] data_array;
        delete[] eligible_Keys;
        delete[] eligible_points;
        return output_t<int>(StatusType::FAILURE);
    }
    int winners_Amount = eligible_Amount;
    int prev_amount = winners_Amount;
    int *winners_keys = new int[winners_Amount];
    int *winners_points = new int[winners_Amount];
    int jptr = 0;
    while (prev_amount > 1)
    {

        jptr = 0;
        for (int j = 0; j < prev_amount; j++) {
            if (prev_amount <= j + 1)
            {
                winners_keys[jptr] = eligible_Keys[j];
                winners_points[jptr++] = eligible_points[j];
            }
            else if(eligible_points[j] > eligible_points[j+1])
            {
                winners_keys[jptr] = eligible_Keys[j];
                winners_points[jptr++] = eligible_points[j] + eligible_points[j+1] + 3;
                j++;
            }
            else if(eligible_points[j] < eligible_points[j+1])
            {
                winners_keys[jptr] = eligible_Keys[j+1];
                winners_points[jptr++] = eligible_points[j] + eligible_points[j+1] + 3;
                j++;
            }
            else if(eligible_points[j] == eligible_points[j+1])
            {
                winners_keys[jptr] = eligible_Keys[j+1];
                winners_points[jptr++] = eligible_points[j] + eligible_points[j+1] + 3;
                j++;
            }
        }
        //(eligible_Keys, winners_keys, jptr * sizeof(int));
        //(eligible_points, winners_points, jptr * sizeof(int));
        int *eKeysPtr = eligible_Keys;
        int *ePointsPtr = eligible_points;
        eligible_Keys = winners_keys;
        eligible_points = winners_points;
        winners_keys = eKeysPtr;
        winners_points = ePointsPtr;
        prev_amount = jptr;
    }
    const int winner_id = eligible_Keys[0];
    delete[] eligible_Keys;
    delete[] eligible_points;
    delete[] winners_keys;
    delete[] winners_points;
    delete[] key_array;
    delete[] data_array;

    return output_t<int>(winner_id);

}

