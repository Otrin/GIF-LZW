#include "huffmannode.h"

HuffmanNode::HuffmanNode(){
    m_probability = 0;
    m_value = '\0';
    m_right = 0;
    m_left = 0;
    m_root = 0;
}

HuffmanNode::HuffmanNode(const HuffmanNode &node){
    m_probability = node.m_probability;
    m_value = node.m_value;
    m_left = node.m_left;
    m_right = node.m_right;
    m_root = node.m_root;
}

HuffmanNode::HuffmanNode(char value, double probability){
    m_probability = probability;
    m_value = value;
    m_left = 0;
    m_right = 0;
    m_root = 0;
}

HuffmanNode::~HuffmanNode(){

}

HuffmanNode& HuffmanNode::operator =(const HuffmanNode& node){
    m_probability = node.m_probability;
    m_value = node.m_value;
    m_left = node.m_left;
    m_right = node.m_right;
    m_root = node.m_root;
}

HuffmanNode& HuffmanNode::getLeft(){
    return *m_left;
}

HuffmanNode& HuffmanNode::getRight(){
    return *m_right;
}

HuffmanNode& HuffmanNode::getRoot(){
    return *m_root;
}

bool HuffmanNode::getFlag(){
    return m_flag;
}

void HuffmanNode::setLeft(HuffmanNode *left){
    m_left = left;
}

void HuffmanNode::setRight(HuffmanNode *right){
    m_right = right;
}

void HuffmanNode::setRoot(HuffmanNode *root){
    m_root = root;
}

void HuffmanNode::setFlag(bool flag){
    m_flag = flag;
}

char HuffmanNode::getValue(){
    return m_value;
}

double HuffmanNode::getProbability(){
    return m_probability;
}
