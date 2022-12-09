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


=======

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


    delete merged_tree;

    return 0;
}
