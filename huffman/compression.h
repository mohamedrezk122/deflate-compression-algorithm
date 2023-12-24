#ifndef COMPRESSION_H
#define COMPRESSION_H
#include"huffman.h"

using namespace std;

int bin_conv(string s);
void construct_key(priority_queue<Node * ,vector<Node *>,Comparison> &codes);
void compress(string fpath);
void write_file(string fpath);
string enc;
#endif