#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Node {
    unsigned char data;
    int freq;
    std::string code;
    Node *left, *right;

    Node(int freq) : freq(freq), left(nullptr), right(nullptr) {}
};

// for priority queue to make minimum heap
struct Comparison {
    bool operator()(Node *left, Node *right) {
        return (left->freq) > (right->freq);
    }
};

// build huffman nodes
void arrange_freq(std::string fpath);
void build();
void generate_codes(Node *cur, std::string code);

// encode
int bin_conv(std::string s);
void construct_key();
void encode(std::string fpath);

// decode
std::string convert_char_to_binary(int dec);
void rebuild_codes(char letter, std::string huffman_code);
void decode_key(std::string fpath);

// steps
void compress_with_huffman(std::string in_fpath, std::string out_fpath);
void decompress_with_huffman(std::string in_fpath, std::string out_fpath);

// output file
void write_file(std::string fpath, bool f);

extern Node *st;
extern std::vector<Node *> char_arr;
extern std::priority_queue<Node *, std::vector<Node *>, Comparison>
    huffman_tree; // the heap to construct
extern std::string enc, decoded_string;

#endif