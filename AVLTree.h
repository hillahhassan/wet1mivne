//
// Created by hilla on 11/27/2022.
//

#ifndef WET1MIVNE_AVLTREE_H
#define WET1MIVNE_AVLTREE_H


typedef enum {
    AVL_TREE_MEMORY_ALLOCATION_ERROR, AVL_TREE_ALREADY_EXISTS, AVL_TREE_DOES_NOT_EXIST, AVL_TREE_SUCCESS
} AVLTreeResult;

template<class K, class T>
class Node {
public:
    int height{};
    K key;
    T data;
    Node<K, T> *parent{};
    Node<K, T> *left_son{};
    Node<K, T> *right_son{};

    explicit Node(const K &key, const T &data) : height(0), key(key), data(data), parent(NULL), left_son(NULL),
                                                 right_son(NULL) {}

    Node() = default;

    Node(const Node<K, T> &to_copy) = default;

    Node<K, T> &operator=(const Node<K, T> &node) = default;

    ~Node<K, T>() = default;

    void update_height();

    int balance_factor();
};

template<class K, class T>
void Node<K, T>::update_height() {
    int max = right_son->height > left_son->height ? right_son->height : left_son->height;
    height = max + 1;
}


template<class K, class T>
int Node<K, T>::balance_factor() {
    int bf = 0;
    if (left_son != NULL) {
        bf += left_son->height;
    }
    if (right_son != NULL) {
        bf -= right_son->height;
    }
    return bf;
}


template<class K, class T>
class AVLTree {
private:
    Node<K, T> *dummy_root;
    int size;

    Node<K, T> *root() const {
        return dummy_root->left_son;
    }

    void balance_nodes_in_search_path(Node<K, T> *last_in_path); //we want access to root

public:
    AVLTree() : dummy_root(new Node<K, T>()), size(0) {}

    AVLTree(const AVLTree<K, T> &);

    ~AVLTree();

    AVLTree<K, T> &operator=(const AVLTree<K, T> &);

    AVLTreeResult insert(const K &key, const T &data);

    AVLTreeResult remove(const K &key);

    AVLTreeResult find(const K &key, T *found_data);
};

template<class K, class T>
Node<K, T> *copy_tree(Node<K, T> *root, Node<K, T> *new_parent) {
    if (root == NULL) {
        return NULL;
    }
    Node<K, T> *new_root = new Node<K, T>(root);
    new_root->parent = new_parent;
    new_root->left_son = copy_tree(root->left_son, new_root);
    new_root->right_son = copy_tree(root->right_son, new_root);
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
    this->root() = copy_tree(to_copy.root(), dummy_root);
}

template<class K, class T>
AVLTree<K, T> &AVLTree<K, T>::operator=(const AVLTree<K, T> &tree) {
    if (this == &tree) {
        return *this;
    }
    destroy_tree(this->root()); //destroy the tree that begins at this->root()
    this->root() = copy_tree(tree.root(), dummy_root);
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

    v->update_height();
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

    v_r->update_height();
    v->update_height();

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

    if (find_aux(this->root(), key) != NULL) {
        return AVL_TREE_ALREADY_EXISTS;
    }

    Node<K, T> *parent_of_new_node = find_last_node_in_search_path(this->root(), key);

    Node<K, T> *new_node = new Node<K, T>(key, data);

    if (parent_of_new_node->key > key) {
        parent_of_new_node->left_son = new_node;
    } else {
        parent_of_new_node->right_son = new_node;
    }

    new_node->parent = parent_of_new_node;

    this->size++;

    this->balance_nodes_in_search_path(parent_of_new_node);
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
    Node<K, T> *next_inorder = to_remove->right_son;
    while (next_inorder->left_son != NULL) {
        next_inorder = next_inorder->left_son;
    }
}

template<class K, class T>
AVLTreeResult AVLTree<K, T>::remove(const K &key) {

}


#endif //WET1MIVNE_AVLTREE_H
