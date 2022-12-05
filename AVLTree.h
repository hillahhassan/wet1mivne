//
// Created by hilla on 11/27/2022.
//

#ifndef WET1MIVNE_AVLTREE_H
#define WET1MIVNE_AVLTREE_H

#include <cmath>


typedef enum {
    AVL_TREE_MEMORY_ALLOCATION_ERROR, AVL_TREE_ALREADY_EXISTS, AVL_TREE_DOES_NOT_EXIST, AVL_TREE_SUCCESS
} AVLTreeResult;

template<class K, class T>
class Node {
public:
    int height;
    K key;
    T data;
    Node<K, T> *parent;
    Node<K, T> *left_son;
    Node<K, T> *right_son;

    explicit Node(const K &key, const T &data) : height(0), key(key), data(data), parent(NULL), left_son(NULL),
                                                 right_son(NULL) {}

    Node() : height(0), key(key), parent(NULL), left_son(NULL),
             right_son(NULL) {}

    Node(const Node<K, T> &to_copy) = default;

    Node<K, T> &operator=(const Node<K, T> &node) = default;

    ~Node<K, T>() = default;

    void update_height();

    int balance_factor();
};

template<class K, class T>
void Node<K, T>::update_height() {
    if (left_son != NULL && right_son != NULL) {
        int max = right_son->height > left_son->height ? right_son->height : left_son->height;
        height = max + 1;
    } else if (left_son != NULL) {
        height = left_son->height + 1;
    } else if (right_son != NULL) {
        height = right_son->height + 1;
    } else {
        height = 0;
    }
}


template<class K, class T>
int Node<K, T>::balance_factor() {
    int bf;
    if (left_son != NULL && right_son != NULL) {
        bf = left_son->height - right_son->height;
    } else if (left_son != NULL) {
        bf = left_son->height - -1;
    } else if (right_son != NULL) {
        bf = -1 - right_son->height;
    } else {
        bf = 0;
    }

    return bf;
}


template<class K, class T>
class AVLTree {
private:
    Node<K, T> *dummy_root;
    int size;

    void balance_nodes_in_search_path(Node<K, T> *last_in_path); //we want access to root

public:
    AVLTree() : dummy_root(new Node<K, T>()), size(0) {}

    AVLTree(K sorted_key_array[], T sorted_data_array[], int size);

    AVLTree(const AVLTree<K, T> &);

    ~AVLTree();

    AVLTree<K, T> &operator=(const AVLTree<K, T> &);

    AVLTreeResult insert(const K &key, const T &data);

    AVLTreeResult remove(const K &key);

    AVLTreeResult find(const K &key, T *found_data);

    void print_inorder_with_bf();

    Node<K, T> *root() {
        return dummy_root->left_son;
    }

    void sorted_keys_and_data(K key_array[], T data_array[]) const;
};

template<class K, class T>
Node<K, T> *copy_tree(Node<K, T> *root, Node<K, T> *new_parent) {
    if (root == NULL) {
        return NULL;
    }
    Node<K, T> *new_root = new Node<K, T>(*root);
    new_root->parent = new_parent;
    new_root->left_son = copy_tree(root->left_son, new_root);
    new_root->right_son = copy_tree(root->right_son, new_root);

    return new_root;
}

template<class K, class T>
void destroy_tree(Node<K, T> *root) {
    if (root == NULL) {
        return;
    }
    destroy_tree(root->left_son);
    destroy_tree(root->right_son);
    delete root;
}

template<class K, class T>
AVLTree<K, T>::AVLTree(const AVLTree<K, T> &to_copy) :
        dummy_root(new Node<K, T>()), size(to_copy.size) {
    this->dummy_root->left_son = copy_tree(to_copy.root(), dummy_root);
}

template<class K, class T>
AVLTree<K, T> &AVLTree<K, T>::operator=(const AVLTree<K, T> &tree) {
    if (this == &tree) {
        return *this;
    }
    destroy_tree(this->root()); //destroy the tree that begins at this->root()
    this->dummy_root->left_son = copy_tree(tree.root(), dummy_root);
    size = tree.size;

    return *this;
}

template<class K, class T>
AVLTree<K, T>::~AVLTree<K, T>() {
    destroy_tree(dummy_root);
}

template<class K, class T>
Node<K, T> *LL(Node<K, T> *v) {
    Node<K, T> *v_l = v->left_son;
    v->left_son = v_l->right_son;
    v_l->right_son = v;

    v->update_height(); //v is now the son, so update first
    v_l->update_height();

    v_l->parent = v->parent;
    v->parent = v_l;

    if (v_l->parent->left_son == v) {
        v_l->parent->left_son = v_l;
    } else {
        v_l->parent->right_son = v_l;
    }

    return v_l;
}

template<class K, class T>
Node<K, T> *RR(Node<K, T> *v) {
    Node<K, T> *v_r = v->right_son;
    v->right_son = v_r->left_son;
    v_r->left_son = v;

    v->update_height(); //update v first since now he is the son
    v_r->update_height();

    v_r->parent = v->parent;
    v->parent = v_r;

    if (v_r->parent->left_son == v) {
        v_r->parent->left_son = v_r;
    } else {
        v_r->parent->right_son = v_r;
    }

    return v_r;
}

template<class K, class T>
Node<K, T> *LR(Node<K, T> *c) {
    RR(c->left_son);
    return LL(c);
}

template<class K, class T>
Node<K, T> *RL(Node<K, T> *c) {
    LL(c->right_son);
    return RR(c);
}

template<class K, class T>
Node<K, T> *find_aux(Node<K, T> *root, const K &key) {
    if (root == NULL) {
        return NULL;
    }

    if (root->key == key) {
        return root;
    } else if (root->key > key) {
        return find_aux(root->left_son, key);
    } else {
        return find_aux(root->right_son, key);
    }
}

template<class K, class T>
Node<K, T> *find_last_node_in_search_path(Node<K, T> *root, const K &key) {
    //will return a pointer to the leaf which we need to attach next element to
    if (root == NULL) {
        return NULL;
    }

    if (root->key == key) {
        return root; //if the key exists then the last node in the search path is the node containing said key
    }

    if (root->key > key) {
        if (root->left_son == NULL) {
            return root;
        }
        return find_last_node_in_search_path(root->left_son, key);
    } else {
        if (root->right_son == NULL) {
            return root;
        }
        return find_last_node_in_search_path(root->right_son, key);
    }
}


template<class K, class T>
AVLTreeResult AVLTree<K, T>::find(const K &key, T *found_data) {
    Node<K, T> *found_node = find_aux(this->root(), key);
    if (found_node == NULL) {
        return AVL_TREE_DOES_NOT_EXIST;
    }

    *found_data = found_node->data;
    return AVL_TREE_SUCCESS;
}

template<class K, class T>
void AVLTree<K, T>::balance_nodes_in_search_path(Node<K, T> *last_in_path) {
    //last_node = last node in search path (not the inserted/ removed node)
    if (last_in_path == NULL) {
        return;
    }

    last_in_path->update_height();

    if (last_in_path->balance_factor() == 2) {
        if (last_in_path->left_son->balance_factor() >= 0) {
            last_in_path = LL(last_in_path);
        } else {
            last_in_path = LR(last_in_path);
        }
    }
    if (last_in_path->balance_factor() == -2) {
        if (last_in_path->right_son->balance_factor() == 1) {
            last_in_path = RL(last_in_path);
        } else {
            last_in_path = RR(last_in_path);
        }
    }

    if (last_in_path == this->root()) {
        return;
    }

    if (last_in_path->parent->height >= last_in_path->height + 1) {
        return;
    }

    balance_nodes_in_search_path(last_in_path->parent);
}

template<class K, class T>
AVLTreeResult AVLTree<K, T>::insert(const K &key, const T &data) {
//check whether key already exists in the tree
    if (find_aux(this->root(), key) != NULL) {
        return AVL_TREE_ALREADY_EXISTS;
    }

    this->size++;

    Node<K, T> *parent_of_new_node = find_last_node_in_search_path(this->root(), key);

    Node<K, T> *new_node = new Node<K, T>(key, data);

    if (parent_of_new_node == NULL) { //epmty tree
        this->dummy_root->left_son = new_node;
        new_node->parent = this->dummy_root;
    } else {
        if (parent_of_new_node->key > key) {
            parent_of_new_node->left_son = new_node;
        } else {
            parent_of_new_node->right_son = new_node;
        }

        new_node->parent = parent_of_new_node;

        this->balance_nodes_in_search_path(parent_of_new_node);
    }

    return AVL_TREE_SUCCESS;
}

template<class K, class T>
Node<K, T> *remove_leaf(Node<K, T> *to_remove) {
    Node<K, T> *parent = to_remove->parent;

    if (parent->left_son == to_remove) {
        parent->left_son = NULL;
    } else {
        parent->right_son = NULL;
    }

    delete to_remove;
    return parent;
}

template<class K, class T>
Node<K, T> *remove_with_one_child(Node<K, T> *to_remove) {
    Node<K, T> *parent = to_remove->parent;
    Node<K, T> *child = to_remove->left_son != NULL ? to_remove->left_son : to_remove->right_son;

    if (parent->left_son == to_remove) {
        parent->left_son = child;
    } else {
        parent->right_son = child;
    }
    child->parent = parent;

    delete to_remove;
    return parent;
}

template<class K, class T>
Node<K, T> *remove_with_two_children(Node<K, T> *to_remove) {
    Node<K, T> *next_inorder = to_remove->right_son; //no need to check for null since node for sure has two sons
    while (next_inorder->left_son != NULL) {
        next_inorder = next_inorder->left_son;
    }

    to_remove->key = next_inorder->key;
    to_remove->data = next_inorder->data;

    if (next_inorder->right_son != NULL) {
        return remove_with_one_child(next_inorder); //returning the parent of the node we actually
        // remove, so that we can make necessary updates and rotations upward
    } else {
        return remove_leaf(next_inorder);
    }
}

template<class K, class T>
AVLTreeResult AVLTree<K, T>::remove(const K &key) {
//find the node to remove
    Node<K, T> *node_to_remove = find_aux(this->root(), key);
    if (node_to_remove == NULL) {
        return AVL_TREE_DOES_NOT_EXIST;
    }

    Node<K, T> *parent_of_removed;
    if (node_to_remove->left_son == NULL && node_to_remove->right_son == NULL) {
        parent_of_removed = remove_leaf(node_to_remove);
    } else if (node_to_remove->left_son != NULL && node_to_remove->right_son != NULL) {
        parent_of_removed = remove_with_two_children(node_to_remove);
    } else {
        parent_of_removed = remove_with_one_child(node_to_remove);
    }

    this->size--;

    if (parent_of_removed != this->dummy_root) {
        this->balance_nodes_in_search_path(parent_of_removed);
    }

    return AVL_TREE_SUCCESS;
}

template<class K, class T>
void print_inorder_with_bf_aux(Node<K, T> *root) {
    if (root == NULL) {
        return;
    }
    print_inorder_with_bf_aux(root->left_son);
    std::cout << "K:" << root->key << " h:" << root->height << " bf:" << root->balance_factor() << std::endl;
    print_inorder_with_bf_aux(root->right_son);
}

template<class K, class T>
void AVLTree<K, T>::print_inorder_with_bf() {
    std::cout << "Now printing tree in order " << std::endl;
    print_inorder_with_bf_aux(this->root());
    std::cout << std::endl;
}

template<class K, class T>
Node<K, T> *complete_tree(int height, Node<K, T> *parent) {
    if (height < 0) {
        return NULL;
    }

    Node<K, T> *new_node = new Node<K, T>();
    new_node->left_son = complete_tree(height - 1, new_node);
    new_node->right_son = complete_tree(height - 1, new_node);
    new_node->parent = parent;

    new_node->update_height();

    return new_node;
}

bool is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int find_minimal_complete_tree_height(int size) {
    do {
        size++;
    } while (!is_power_of_two(size));

    return log2(size) - 1;
}

template<class K, class T>
void make_almost_complete_tree(Node<K, T> *root, int *current_size_ptr, int final_size) {
    //root is of complete tree, and we are erasing elements from right to left
    if (root == NULL || *current_size_ptr == final_size) {
        return;
    }

    make_almost_complete_tree(root->right_son, current_size_ptr, final_size);

    if (root->left_son == NULL && root->right_son == NULL) {
        if (root->parent->right_son == root) {
            root->parent->right_son = NULL;
        } else {
            root->parent->left_son = NULL;
        }
        delete root;
        *current_size_ptr--;
    } else {
        make_almost_complete_tree(root->left_son, current_size_ptr, final_size);
    }
}

template<class K, class T>
void populate_empty_tree(Node<K, T> *root, K key_array[], T data_array[], int *iptr) {
    if (root == NULL) {
        return;
    }

    populate_empty_tree(root->left_son, key_array, data_array, iptr);

    root->key = key_array[*iptr];
    root->data = data_array[*iptr];
    *iptr++;

    populate_empty_tree(root->right_son, key_array, data_array, iptr);
}


template<class K, class T>
// this c'tor will help us in unite teams, where will build a united tree out of a sorted array of the union of players from both teams
AVLTree<K, T>::AVLTree(K sorted_key_array[], T sorted_data_array[], int size) : dummy_root(new Node<K, T>()), size(0) {
    int height = find_minimal_complete_tree_height(size);
    dummy_root->left_son = complete_tree(height, dummy_root);
    int current_size = pow(2, height + 1) - 1;
    make_almost_complete_tree(this->root(), &current_size, size);

    this->size = size;

    int i = 0;
    populate_empty_tree(this->root(), sorted_key_array, sorted_data_array, &i);
}

template<class K, class T>
void inorder_tree_to_array(Node<K, T> *root, K key_array[], T data_array[], int *iptr) {
    if (root == NULL) {
        return;
    }

    inorder_tree_to_array(root->left_son, key_array, data_array, iptr);

    key_array[*iptr] = root->key;
    data_array[*iptr] = root->data;
    *iptr++;

    inorder_tree_to_array(root->right_son, key_array, data_array, iptr);
}

//copies tree data and keys into respective arrays
template<class K, class T>
void AVLTree<K, T>::sorted_keys_and_data(K key_array[], T data_array[]) const {
    int i = 0;
    inorder_tree_to_array(this->root(), key_array, data_array, &i);
}

//take two sorted data arrays and merge them into a single sorted data array
//take two sorted key arrays and merge them into a single sorted key array

template<class K, class T>
void
merge_key_arrays_and_data_arrays(K key_array1[], K key_array2[], T data_array1[], T data_array2[], int size1, int size2,
                                 K merged_key_array[], T merged_data_array[]) {
    int i1 = 0;
    int i2 = 0;
    int i3 = 0;

    while (i1 < size1 && i2 < size2) {
        if (key_array1[i1] < key_array2[i2]) {
            merged_key_array[i3] = key_array1[i1];
            merged_data_array[i3] = data_array1[i1];
            i1++;
        } else {
            merged_key_array[i3] = key_array2[i2];
            merged_data_array[i3] = data_array1[i2];
            i2++;
        }
        i3++;
    }
    while (i1 < size1) {
        merged_key_array[i3] = key_array1[i1];
        merged_data_array[i3] = data_array1[i1];
        i1++;
        i3++;
    }
    while (i2 < size2) {
        merged_key_array[i3] = key_array1[i2];
        merged_data_array[i3] = data_array1[i2];
        i2++;
        i3++;
    }
}

#endif //WET1MIVNE_AVLTREE_H
