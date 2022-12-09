#include <iostream>
#include "worldcup23a1.h"


int main() {
    world_cup_t *obj = new world_cup_t();

    obj->add_team(1, 10000);
    obj->add_team(2, 20000);
    obj->add_team(3, 30000);
    obj->add_team(4, 40000);

    obj->add_player(1001, 1, 10, 0, 0, false);
    obj->add_player(1002, 1, 10, 0, 0, false);
    obj->add_player(1003, 1, 10, 0, 0, false);
    obj->add_player(1004, 1, 10, 0, 2, false);
    obj->add_player(1005, 1, 10, 0, 0, false);
    obj->add_player(1006, 1, 10, 4, 3, false);
    obj->add_player(1007, 1, 10, 0, 0, false);
    obj->add_player(1008, 1, 10, 0, 0, true);
    obj->add_player(1009, 1, 10, 0, 0, false);
    obj->add_player(1010, 1, 10, 0, 0, false);
    obj->add_player(1011, 1, 10, 0, 0, false);
    obj->add_player(1012, 1, 10, 0, 0, false);

    obj->add_player(2001, 2, 20, 0, 0, false);
    obj->add_player(2002, 2, 20, 0, 0, false);
    obj->add_player(2003, 2, 20, 0, 0, false);
    obj->add_player(2004, 2, 20, 0, 2, false);
    obj->add_player(2005, 2, 20, 0, 0, false);
    obj->add_player(2006, 2, 20, 4, 3, false);
    obj->add_player(2007, 2, 20, 0, 0, false);
    obj->add_player(2008, 2, 20, 6, 4, true);
    obj->add_player(2009, 2, 20, 0, 0, false);
    obj->add_player(2010, 2, 0, 0, 0, false);
    obj->add_player(2011, 2, 20, 0, 0, false);

    obj->add_player(3001, 3, 30, 0, 0, false);
    obj->add_player(3002, 3, 30, 0, 0, false);
    obj->add_player(3003, 3, 30, 0, 0, false);
    obj->add_player(3004, 3, 30, 0, 2, false);
    obj->add_player(3005, 3, 30, 2, 0, false);
    obj->add_player(3006, 3, 30, 4, 3, false);
    obj->add_player(3007, 3, 30, 0, 0, false);
    obj->add_player(3008, 3, 30, 6, 4, false);
    obj->add_player(3009, 3, 30, 0, 2, false);
    obj->add_player(3010, 3, 30, 0, 0, true);
    obj->add_player(3011, 3, 30, 0, 0, true);

    obj->add_player(4001, 4, 2, 1, 2, false);
    obj->add_player(4002, 4, 2, 2, 2, false);

    obj->update_player_stats(3011, 2, 1, 1);

    obj->play_match(2, 3);
    obj->play_match(1, 2);

//    int res = obj->get_num_played_games(2003) << std::endl;
//    std::cout << obj->get_team_points(3) << std::endl;

    obj->unite_teams(1, 2, 2);

//    std::cout << obj->get_top_scorer(-1) << std::endl;
//    std::cout << obj->get_all_players_count(3) << std::endl;
//    std::cout << obj->get_all_players(4) << std::endl;
//    std::cout << obj->get_closest_player(2008, 2) << std::endl;

    obj->knockout_winner(0, 3);

    obj->remove_team(2);

    obj->remove_player(3008);



    return 0;
}
