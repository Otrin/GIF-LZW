#include "node.h"

Node::Node(){
    value = '\0';
    frequenzy = -INT_MAX;
    left = NULL;
    right = NULL;
}

Node::Node(const Node &node){
    this->value = node.value;
    this->frequenzy = node.frequenzy;
    this->left = node.left;
    this->right = node.right;
}

Node::Node(unsigned char value, int frequenzy){
    this->value = value;
    this->frequenzy = frequenzy;
    left = NULL;
    right = NULL;
}

Node& Node::operator= (const Node& node){
    this->value = node.value;
    this->frequenzy = node.frequenzy;
    this->left = node.left;
    this->right = node.right;
    return *this;
}

Node::~Node(){

}

unsigned char Node::getValue()const{
    return value;
}

int Node::getFrequenzy()const{
    return frequenzy;
}

Node& Node::getLeft(){
    return *left;
}

Node& Node::getRight(){
    return *right;
}

void Node::setLeft(Node *left){
    this->left = left;
}

void Node::setRight(Node *right){
    this->right = right;
}
