#ifndef HUFFMAN_H
#define HUFFMAN_H
#include<iostream>
#include<queue>
#include<vector>
#include <fstream>

using namespace std;

struct Node
{
    char data;
    int freq;
    string code;
    Node *left, *right;

    Node(int freq):freq(freq),left(nullptr),right(nullptr){}
};

// for priority queue to make minimum heap
struct Comparison
{
    bool operator()(Node *left, Node *right)
    {
        return (left->freq) > (right->freq);
    }
};

class Huffman
{
    private :
        Node *root;
        vector<Node *>char_arr;
        priority_queue<Node * ,vector<Node *>,Comparison> children;

        void arrange_freq(string xml);
        void build();
        void generate_codes(Node *cur,string code);
    public :
        Huffman(string fpath);
        Node *get_root();
        priority_queue<Node * ,vector<Node *>,Comparison> get_children();
        vector<Node *> get_codes();
};
#endif