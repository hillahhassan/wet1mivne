//
// Created by hilla on 11/27/2022.
//

#ifndef WET1MIVNE_AVLTREE_H
#define WET1MIVNE_AVLTREE_H


typedef enum {
    AVL_TREE_MEMORY_ALLOCATION_ERROR, AVL_TREE_ALREADY_EXISTS, AVL_TREE_DOES_NOT_EXIST, AVL_TREE_SUCCESS
} AVLTreeResult;

template<class T>
class Node {
public:
    int height{};
    T data;
    Node<T> *parent;
    Node<T> *left_son;
    Node<T> *right_son;

    explicit Node(const T &data) : height(0), data(data), parent(NULL), left_son(NULL), right_son(NULL) {}

    Node() = default;

    Node(const Node<T> &to_copy) = default;

    Node<T> &operator=(const Node<T> &node) = default;

    ~Node<T>() = default;

    void update_height();
};

template<class T>
void Node<T>::update_height() {
    int max = right_son->height > left_son->height ? right_son->height : left_son->height;
    height = max + 1;
}

template<class T>
class AVLTree {
private:
    Node<T> *dummy_root;
    int size;

public:
    AVLTree() : dummy_root(new Node<T>()), size(0) {}

    AVLTree(const AVLTree<T> &);

    ~AVLTree();

    AVLTree<T> &operator=(const AVLTree<T> &);

    AVLTreeResult insert(const T &data);

    AVLTreeResult remove(const T &data);

    AVLTreeResult find(const T &data, T *found_data);

    Node<T> *root() const {
        return dummy_root->left_son;
    }
};

template<class T>
Node<T> *copy_tree(Node<T> *root, Node<T> *new_parent) {
    if (root == NULL) {
        return NULL;
    }
    Node<T> *new_root = new Node<T>(root);
    new_root->parent = new_parent;
    new_root->left_son = copy_tree(root->left_son, new_root);
    new_root->right_son = copy_tree(root->right_son, new_root);
}

template<class T>
void destroy_tree(Node<T> *root) {
    if (root == NULL) {
        return;
    }
    destroy_tree(root->left_son);
    destroy_tree(root->right_son);
    delete root;
}

template<class T>
AVLTree<T>::AVLTree(const AVLTree<T> &to_copy) :
        dummy_root(new Node<T>()), size(to_copy.size) {
    this->root() = copy_tree(to_copy.root(), dummy_root);
}

template<class T>
AVLTree<T> &AVLTree<T>::operator=(const AVLTree<T> &tree) {
    if (this == &tree) {
        return *this;
    }
    destroy_tree(this->root()); //destroy the tree that begins at this->root()
    this->root() = copy_tree(tree.root(), dummy_root);
    size = tree.size;

    return *this;
}

template<class T>
AVLTree<T>::~AVLTree<T>() {
    destroy_tree(dummy_root);
}

template<class T>
int balance_factor(const Node<T> *root) {
    return root->left_son->height - root->right_son->height;
}

template<class T>
Node<T> *LL(Node<T> *v) {
    Node<T> *v_l = v->left_son;
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

template<class T>
Node<T> *RR(Node<T> *v) {
    Node<T> *v_r = v->right_son;
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

template<class T>
Node<T> *LR(Node<T> *c) {
    RR(c->left_son);
    return LL(c);
}

template<class T>
Node<T> *RL(Node<T> *c) {
    LL(c->right_son);
    return RR(c);
}

template<class T>
AVLTreeResult AVLTree<T>::insert(const T &data) {

}


#endif //WET1MIVNE_AVLTREE_H
