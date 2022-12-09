#include "worldcup23a1.h"
#include "AVLTree.h"
#include "Team.h"
#include "Player.h"
/// Insert AVLTree<int, std::shared_ptr<Team>>(), AVLTree<std::shared_ptr<Player>, int>()

world_cup_t::world_cup_t() :  TeamsTree(), RankingTree()
        , PlayersTree(),KosherTree(), amount_of_kosher(0),g_playersCount(0),g_topScorerID(0),g_topScorerGoals(0),g_topScorerCards(0)
{
}

world_cup_t::~world_cup_t() = default;
/*{

	// TODO: Your code goes here
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
    if(close_next != NULL)
    {
        PlayersTree.find(close_next->playerId, &new_player_ptr->close_NextPlayer);
    }

    if(close_prev != NULL)
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
    team_of_player->playersCount--;
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

        player_to_update->goals += scoredGoals;
        player_to_update->gamesPlayed += gamesPlayed;
        player_to_update->cards += cardsReceived;

        std::shared_ptr<Player> player_to_insert(new Player(*player_to_update));

        // Remove the old player from the ranking tree
        RankingTree.remove(*player_to_update);

        // Insert the new player with updated stats into the ranking tree
        RankingTree.insert(*player_to_insert,playerId);

        // Update team statistics
        std::shared_ptr<Team> team_of_player = player_to_update->teamP;
        team_of_player->totalGoals += scoredGoals;
        team_of_player->gamesPlayed += gamesPlayed;
        team_of_player->totalCards += cardsReceived;

    }
    catch (const std::bad_alloc &)
    {
        return StatusType::ALLOCATION_ERROR;
    }

    std::shared_ptr<Player>  old_prev_player = player_to_update->close_NextPlayer->close_PrevPlayer;
    player_to_update->close_NextPlayer->close_PrevPlayer = player_to_update->close_PrevPlayer->close_NextPlayer;
    player_to_update->close_PrevPlayer->close_NextPlayer = old_prev_player;
    Player* next_Player = RankingTree.get_next_inorder(*player_to_update);
    Player* prev_Player = RankingTree.get_prev_inorder(*player_to_update);
    PlayersTree.find(next_Player->playerId,&player_to_update->close_NextPlayer);
    PlayersTree.find(prev_Player->playerId,&player_to_update->close_PrevPlayer);
    if(player_to_update->close_PrevPlayer != NULL)
    {
        player_to_update->close_PrevPlayer->close_NextPlayer = player_to_update;
    }
    if(player_to_update->close_NextPlayer != NULL) {
        player_to_update->close_NextPlayer->close_PrevPlayer = player_to_update;
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
        if (TeamsTree.find(newTeamId, nullptr) == AVL_TREE_SUCCESS) {
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

        newTeam->playersCount = team1->playersCount + team2->playersCount;
        newTeam->teamPlayers_byID.merge_two_trees(&team1->teamPlayers_byID ,&team2->teamPlayers_byID);
        newTeam->teamPlayers_byRank.merge_two_trees(&team1->teamPlayers_byRank ,&team2->teamPlayers_byRank);

    }

    else if(teamId1 == newTeamId)
    {
        int * array_key_team2 = new int[team2->playersCount];
        std::shared_ptr<Player> *array_data_team2;
        team2->teamPlayers_byID.to_sorted_keys_and_data(array_key_team2,array_data_team2);
        for(int i=0;i<team2->playersCount;i++)
        {
            array_data_team2[i]->teamId = newTeamId;
            array_data_team2[i]->teamP = newTeam;
            array_data_team2[i]->gamesPlayed += team2->gamesPlayed - array_data_team2[i]->teamGamesPlayed_preAdd;
        }
        team1->teamPlayers_byID.merge_two_trees(&team1->teamPlayers_byID,&team2->teamPlayers_byID);
        team1->teamPlayers_byRank.merge_two_trees(&team1->teamPlayers_byRank,&team2->teamPlayers_byRank);
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
        team2->teamPlayers_byID.merge_two_trees(&team1->teamPlayers_byID, &team2->teamPlayers_byID);
        team2->teamPlayers_byRank.merge_two_trees(&team1->teamPlayers_byRank, &team2->teamPlayers_byRank);
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

    if(teamId == 0 || output==NULL)
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
        Player key_array[team_to_get->playersCount];
        team_to_get->teamPlayers_byRank.to_sorted_keys_and_data(key_array,output);
        int wow = 0;
    }
    if(teamId < 0)
    {
        if(g_playersCount < 1)
        {
            return StatusType::FAILURE;
        }

        RankingTree.to_sorted_keys_and_data(nullptr,output);
    }
    //output[0] = 4001;
    //output[1] = 4002;
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

    if(player->close_NextPlayer < player->close_PrevPlayer)
    {
        return output_t<int>(player->close_PrevPlayer->playerId);
    }
    else
    {
        return output_t<int>(player->close_NextPlayer->playerId);
    }

}

output_t<int> world_cup_t::knockout_winner(int minTeamId, int maxTeamId)
{

    if(minTeamId < 0 || maxTeamId < 0 || maxTeamId<minTeamId )
    {
        return output_t<int>(StatusType::INVALID_INPUT);
    }


	// TODO: Your code goes here
	return 2;
}
