#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <iostream>
#include <queue>
#include <vector>
#include <fstream>

using namespace std;

struct Node
{
    char data;
    int freq;
    string code;
    Node *left, *right;

    Node(int freq) : freq(freq), left(nullptr), right(nullptr) {}
};

// for priority queue to make minimum heap
struct Comparison
{
    bool operator()(Node *left, Node *right)
    {
        return (left->freq) > (right->freq);
    }
};

Node *st;
vector<Node *> char_arr;
priority_queue<Node *, vector<Node *>, Comparison> huffman_tree; // the heap to construct
string enc,dec;


// build huffman nodes
void arrange_freq(string fpath);
void build();
void generate_codes(Node *cur, string code);

//encode
int bin_conv(string s);
void construct_key(priority_queue<Node * ,vector<Node *>,Comparison> &codes);
void encode(string fpath);

//decode
string dec_Bin_conv(int decValue);
void rebuild_enco(char ASCII_Code, string path);
void decode();

//steps
void compress(string fpath);
void decompress(string fpath);

//output file
void write_file(string fpath);

#endif