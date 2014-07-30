#ifndef NODE_H
#define NODE_H

#include <climits>
#include <stdio.h>

class Node
{
private:
    unsigned char value;
    int frequenzy;
    Node *left, *right;
public:
    Node();
    Node(const Node& node);
    Node(unsigned char value, int frequenzy);
    Node& operator= (const Node& node);
    ~Node();
    unsigned char getValue()const;
    int getFrequenzy()const;
    Node& getLeft();
    Node& getRight();
    void setLeft(Node *left);
    void setRight(Node *right);
};

#endif // NODE_H
