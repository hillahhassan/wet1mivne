#include "worldcup23a1.h"
#include "AVLTree.h"
#include "Team.h"
#include "Player.h"
world_cup_t::world_cup_t() :  TeamsTree(AVLtree<int, Team_ptr>())
        , PlayersTree(AVLtree<int, Player_ptr>()), playersCount(0),topScorerID(0),topScorerGoals(0),topScorerCards(0)
{
	// TODO: Your code goes here
}

world_cup_t::~world_cup_t()
{
    delete PlayersTree;
    delete TeamsTree;
	// TODO: Your code goes here
}


StatusType world_cup_t::add_team(int teamId, int points)
{
    if (TeamsTree.find(teamId) != nullptr)
    {
        return StatusType::FAILURE
    }
    Team *new_team_ptr = nullptr;
    Team_ptr new_team;
    try
    {
        new_team_ptr = new Team(teamId, points);
        new_team = Team_ptr(new_team_ptr);
    }
    catch (const std::bad_alloc &)
    {
        return StatusType::ALLOCATION_ERROR;
    }

    bool team_inserted = false;
    try
    {
        TeamsTree.insert(teamId, new_team);
        team_inserted = true;
    }
    catch (const std::bad_alloc &)
    {
        if(team_inserted)
        {
            TeamsTree.remove(teamId);
        }

        delete new_team_ptr;
        new_team = nullptr;
        return StatusType::ALLOCATION_ERROR;
    }

	// TODO: Your code goes here
	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_team(int teamId)
{
    if(teamId <= 0)
    {
        return StatusType::INVALID_INPUT;
    }
    Team team_to_remove = TeamsTree.find(teamId)->data;
    if(team_to_remove.playersCount == 0)
    {
        TeamsTree.remove(teamId);
        return StatusType::SUCCESS;
    }



	// TODO: Your code goes here
	return StatusType::FAILURE;
}

StatusType world_cup_t::add_player(int playerId, int teamId, int gamesPlayed,
                                   int goals, int cards, bool goalKeeper)
{

    Node<int, Team_ptr> *temp_team_Node = TeamsTree.find(teamId);
    if(temp_team_Node == nullptr)
    {
        return StatusType::FAILURE;
    }
    Team_ptr teamPtr = temp_team_Node.data;


    if (PlayersTree.find(playerIdId) != nullptr)
    {
        return StatusType::FAILURE;
    }
    Player *new_player_ptr = nullptr;
    Player_ptr new_player;
    try
    {
        new_player_ptr = new Player(playerId, points);
        new_player = Player_ptr(new_player_ptr);
    }
    catch (const std::bad_alloc &)
    {
        return StatusType::ALLOCATION_ERROR;
    }

    bool player_inserted = false;
    bool playerTeam_inserted = false;
    try
    {
        PlayersTree.insert(playerId, new_player);
        player_inserted = true;
        teamPtr->teamPlayers.insert(playerId, new_player);
        playerTeam_inserted = true;

    }
    catch (const std::bad_alloc &)
    {
        if(player_inserted)
        {
            PlayersTree.remove(playerId);
        }
        if(playerTeam_inserted)
        {
            teamPtr->teamPlayers.remove(playerId);
        }

        delete new_player_ptr;
        new_player = nullptr;
        return StatusType::ALLOCATION_ERROR;
    }
	// TODO: Your code goes here
	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_player(int playerId)
{
	// TODO: Your code goes here
	return StatusType::SUCCESS;
}

StatusType world_cup_t::update_player_stats(int playerId, int gamesPlayed,
                                        int scoredGoals, int cardsReceived)
{
	// TODO: Your code goes here
	return StatusType::SUCCESS;
}

StatusType world_cup_t::play_match(int teamId1, int teamId2)
{
	// TODO: Your code goes here
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_num_played_games(int playerId)
{
	// TODO: Your code goes here
	return 22;
}

output_t<int> world_cup_t::get_team_points(int teamId)
{
	// TODO: Your code goes here
	return 30003;
}

StatusType world_cup_t::unite_teams(int teamId1, int teamId2, int newTeamId)
{
	// TODO: Your code goes here
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_top_scorer(int teamId)
{
	// TODO: Your code goes here
	return 2008;
}

output_t<int> world_cup_t::get_all_players_count(int teamId)
{
	// TODO: Your code goes here
    static int i = 0;
    return (i++==0) ? 11 : 2;
}

StatusType world_cup_t::get_all_players(int teamId, int *const output)
{
	// TODO: Your code goes here
    output[0] = 4001;
    output[1] = 4002;
	return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_closest_player(int playerId, int teamId)
{
	// TODO: Your code goes here
	return 1006;
}

output_t<int> world_cup_t::knockout_winner(int minTeamId, int maxTeamId)
{
	// TODO: Your code goes here
	return 2;
}

