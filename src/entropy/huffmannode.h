#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

class HuffmanNode{
private:
    //probability of value
    double m_probability;
    char m_value;
    //flag is needed to create codes
    bool m_flag;
    HuffmanNode *m_right, *m_left, *m_root;
public:
    /**
     * @brief Default Constructor
     */
    HuffmanNode();
    /**
     * @brief Copy Constructor
     * @param node
     */
    HuffmanNode(const HuffmanNode& node);
    /**
     * @brief Constructor to initialize the Node
     * @param value
     * @param probability
     */
    HuffmanNode(char value, double probability);
    ~HuffmanNode();
    /**
     * @brief returs the left childnode
     */
    HuffmanNode& getLeft();
    /**
     * @brief returns the right childnode
     */
    HuffmanNode& getRight();
    /**
     * @brief returs the rootnode
     */
    HuffmanNode& getRoot();
    HuffmanNode& operator=(const HuffmanNode& node);
    /**
     * @brief returns the flag
     */
    bool getFlag();
    /**
     * @brief setter for left childnode
     */
    void setLeft(HuffmanNode *left);
    /**
     * @brief setter for right childnode
     */
    void setRight(HuffmanNode *right);
    /**
     * @brief setter for rootnode
     */
    void setRoot(HuffmanNode *root);
    /**
     * @brief setter for the flag
     */
    void setFlag(bool flag);
    /**
     * @brief returns the value of this node
     */
    char getValue();
    /**
     * @brief returns the probability of the value of this node
     */
    double getProbability();
};

#endif // HUFFMANNODE_H
