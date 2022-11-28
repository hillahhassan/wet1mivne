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
};

template<class K, class T>
void Node<K, T>::update_height() {
    int max = right_son->height > left_son->height ? right_son->height : left_son->height;
    height = max + 1;
}

template<class K, class T>
class AVLTree {
private:
    Node<K, T> *dummy_root;
    int size;

    Node<K, T> *root() const {
        return dummy_root->left_son;
    }

public:
    AVLTree() : dummy_root(new Node<K, T>()), size(0) {}

    AVLTree(const AVLTree<K, T> &);

    ~AVLTree();

    AVLTree<K, T> &operator=(const AVLTree<K, T> &);

    AVLTreeResult insert(const T &data);

    AVLTreeResult remove(const T &data);

    AVLTreeResult find(const T &data, T *found_data);
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
int balance_factor(const Node<K, T> *root) {
    return root->left_son->height - root->right_son->height;
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
AVLTreeResult AVLTree<K, T>::insert(const T &data) {

}


#endif //WET1MIVNE_AVLTREE_H
