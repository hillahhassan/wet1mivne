#include <iostream>
#include "AVLTree.h"

class player{
public:
    int playerId;
    int teamId;
    int gamesPlayed;

    player(int player_id, int team_id, int games_played) : playerId(player_id), teamId(team_id), gamesPlayed(games_played){}
    player() = default;
};
int main() {
    AVLTree<int, class player> player_tree;
    player_tree.insert(1, player(1, 10, 17));
    player_tree.insert(2, player(2, 10, 17));
    player_tree.insert(3, player(3, 10, 17));
    player_tree.insert(4, player(4, 10, 17));
    player_tree.insert(5, player(5, 10, 17));
    player_tree.insert(6, player(6, 10, 17));


    //Node<int, class player>* copied_player_tree;
    //copied_player_tree = copy_tree<int, class player>(player_tree.root(), NULL);

    return 0;
}
