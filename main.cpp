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

    AVLTree<int, player> player_tree1;
//    player_tree.print_inorder_with_bf();
    player_tree1.insert(2, player(1, 10, 17));
//    player_tree.print_inorder_with_bf();
    player_tree1.insert(4, player(2, 10, 17));
//    player_tree.print_inorder_with_bf();
    player_tree1.insert(6, player(3, 10, 17));
//    player_tree.print_inorder_with_bf();
    player_tree1.insert(8, player(4, 10, 17));
//    player_tree.print_inorder_with_bf();
    player_tree1.insert(10, player(5, 10, 17));
//    player_tree.print_inorder_with_bf();
    player_tree1.insert(12, player(6, 10, 17));
    player_tree1.print_inorder_with_bf();

    AVLTree<int, player> player_tree2;
    player_tree2.insert(1, player(1, 10, 17));
    player_tree2.insert(3, player(1, 10, 17));
    player_tree2.insert(5, player(1, 10, 17));
    player_tree2.insert(7, player(1, 10, 17));
    player_tree2.insert(9, player(1, 10, 17));
    player_tree2.insert(11, player(1, 10, 17));

    player_tree2.print_inorder_with_bf();

    AVLTree<int, player>* merged_tree = AVLTree<int, player>::merge_two_trees(&player_tree1, &player_tree2);
    merged_tree->print_inorder_with_bf();
    int* next1 = merged_tree->get_next_inorder(1);
    int* next2 = merged_tree->get_next_inorder(2);
    int* next3 = merged_tree->get_next_inorder(3);
    int* next4 = merged_tree->get_next_inorder(4);
    int* next5 = merged_tree->get_next_inorder(5);
    int* next6 = merged_tree->get_next_inorder(6);
    int* next7 = merged_tree->get_next_inorder(7);
    int* next8 = merged_tree->get_next_inorder(8);
    int* next9 = merged_tree->get_next_inorder(9);
    int* next10 = merged_tree->get_next_inorder(10);
    int* next11 = merged_tree->get_next_inorder(11);
    int* next12 = merged_tree->get_next_inorder(12);
    int* next13 = merged_tree->get_next_inorder(13);

    AVLTree<int, player> empty_tree;
    AVLTree<int, player>* merged_with_empty = AVLTree<int, player>::merge_two_trees(merged_tree, &empty_tree);

    merged_with_empty->print_inorder_with_bf();

    AVLTree<int, player>* merged_two_empties = AVLTree<int, player>::merge_two_trees(&empty_tree, &empty_tree);
    merged_two_empties->print_inorder_with_bf();
    //    player_tree.remove(4);
//    player_tree.print_inorder_with_bf();
//    player_tree.remove(5);
//    player_tree.print_inorder_with_bf();
//    player_tree.remove(6);
//    player_tree.print_inorder_with_bf();
//    player_tree.remove(2);
//    player_tree.print_inorder_with_bf();
//    player_tree.remove(3);
//    player_tree.print_inorder_with_bf();
//    player_tree.remove(1);
//    player_tree.print_inorder_with_bf();
//
//    player p;
//    AVLTreeResult res = player_tree1.find(6, &p);


    //Node<int, class player>* copied_player_tree;
    //copied_player_tree = copy_tree<int, class player>(player_tree.root(), NULL);

    return 0;
}
