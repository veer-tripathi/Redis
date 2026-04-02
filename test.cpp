#include <bits/stdc++.h>
using namespace std;

class node
{
public:
    int value;
    node *left = nullptr;
    node *right = nullptr;
    node *parent = nullptr;

    int height = 1;
    int size;

    // public:
    node(int value)
    {
        this->value = value;
        size = 1;
    }

    friend class BST;
};

class BST
{
    protected:
    int height;
    node *root = nullptr;
    friend class AVL;

public:
    BST()
    {
        height = 0;
    }

    BST(int value)
    {
        height = 0;
        root = new node(value);
    }

    int getheight()
    {
        return height;
    }

    virtual node *insert(int value)
    {
        if (root == nullptr)
        {
            root = new node(value);
            return root;
        }

        node *curr = root;
        node *parent = nullptr;

        while (true)
        {
            parent = curr;
            curr->size++;

            if (value < curr->value)
            {
                if (curr->left)
                {
                    parent = curr;
                    curr = curr->left;
                }
                else
                {
                    parent = curr;
                    curr->left = new node(value);
                    curr->left->parent = curr;
                    return curr->left;
                }
            }
            else if (value > curr->value)
            {
                if (curr->right)
                {
                    parent = curr;
                    curr = curr->right;
                }
                else
                {
                    parent = curr;
                    curr->right = new node(value);
                    curr->right->parent = curr;
                    return curr->right;
                }
            }
            else
            {
                curr->size--;
                return nullptr;
            }
        }
    }
};

class AVL : public BST
{
    // Rotation
    // 4 methods :- 1-straight right and  2-straight left
    // 3-zig zag right and 4- zig zag left
    public:

    int getheight(node *n)
    {
        return n ? n->height : 0;
    }
    int getBalance(node *n)
    {
        return (getheight(n->left) - getheight(n->right));
    }
    node* getroot(){
        return BST::root;
    }
    void update(node *n)
    {
        if (!n)
            return;

        n->height = 1 + max(getheight(n->left), getheight(n->right));

        n->size = 1;
        if (n->left)
            n->size += n->left->size;
        if (n->right)
            n->size += n->right->size;
    }

    void LR(node *z, node *x, node *y)
    {
        node *temp = x->left;
        x->left = z;
        z->right = temp;
        x->parent = z->parent;
        // z->parent=x;
        // y->parent=x;
        // // z->right->parent=z;
        // if(z->right){
        //     z->right->parent=z;
        // }
        if (z->parent)
        {
            if (z->parent->left == z)
            {
                z->parent->left = x;
            }
            else
            {
                z->parent->right = x;
            }
        }
        else
        {
            BST::root = x;
        }

        z->parent = x;
        if (temp)
        {
            temp->parent = z;
        }
        update(z);
        update(x);
    }

    void RR(node *z, node *y, node *x)
    {
        node *temp = x->right;
        x->right = z;
        z->left = temp;
        x->parent = z->parent;
        if (z->parent)
        {
            if (z->parent->left == z)
            {
                z->parent->left = x;
            }
            else
            {
                z->parent->right = x;
            }
        }
        else
        {
            BST::root = x;
        }

        z->parent = x;
        if (temp)
        {
            temp->parent = z;
        }
        update(z);
        update(x);
    }

public:
    node *insert(int value)
    {
        node *inserted = BST::insert(value);
        if (!inserted)
            return nullptr;
        node *curr = inserted;
        while (curr)
        {
            update(curr);
            int balance = getBalance(curr);

            if (abs(balance) > 1)
            {
                node *z = curr;
                node *x = (getheight(z->left) > getheight(z->right)) ? (z->left) : (z->right);
                node *y = (getheight(x->left) > getheight(x->right)) ? (x->left) : (x->right);

                if (z->left == x && x->left == y)
                {
                    // RR(x)
                    RR(z, y, x);
                }

                else if (z->right == x && x->right == y)
                {
                    // LR(z)
                    LR(z, x, y);
                }
                else if (z->right == x && x->left == y)
                {
                    // RR(x)
                    // LR(z)
                    RR(x, nullptr, y);
                    LR(z, y, x);
                }
                else
                {
                    // LR(x)
                    // RR(z)
                    LR(x, y, nullptr);
                    RR(z, x, y);
                }
            }
            curr = curr->parent;
        }
        return inserted;
    }
};

void inorder(node *root)
{
    if (!root)
        return;
    inorder(root->left);
    cout << root->value << " ";
    inorder(root->right);
}

void levelOrder(node *root)
{
    if (!root)
        return;

    queue<node *> q;
    q.push(root);

    while (!q.empty())
    {
        int sz = q.size();
        while (sz--)
        {
            node *curr = q.front();
            q.pop();

            cout << curr->value
                 << "(h=" << curr->height
                 << ",s=" << curr->size << ") ";

            if (curr->left)
                q.push(curr->left);
            if (curr->right)
                q.push(curr->right);
        }
        cout << "\n";
    }
}

int main()
{
    AVL tree;

    vector<int> vals = {10, 20, 30, 25, 28, 27, 5, 4, 3};

    for (int x : vals)
    {
        cout << "\nInserting: " << x << "\n";
        tree.insert(x);

        cout << "Level Order:\n";
        levelOrder(tree.getroot()); 

        cout << "Inorder: ";
        inorder(tree.getroot());
        cout << "\n---------------------\n";
    }

    return 0;
}