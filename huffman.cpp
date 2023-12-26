#include "huffman.h"
void arrange_freq(string fpath)
{

    // initializing charachter arrays for taking frequency
    Node *temp = nullptr;
    for (int i = 0; i < 128; i++)
    {
        temp = new Node(0);
        temp->data = i;
        char_arr.push_back(temp);
    }

    ifstream file;
    file.open(fpath);
    char c;
    file.get(c);

    while (!file.eof())
    {
        char_arr[c]->freq++;
        file.get(c);
    }
    file.close();
    // making the minimum heap for building ordered tree
    for (int i = 0; i < 128; i++)
    {
        if (char_arr[i]->freq > 0)
            huffman_tree.push(char_arr[i]);
    }
}

/*
huffman coding works as following :
1/ we need to assign each charachter to a binary code
2/ we want the most frequent to have the shortest binary code
3/ so the lowest frequency must be at the bottom
4/ in our huffman_tree prior queue we have the minimum frequency ready to be popped
5/ our st will be at the bottom after we constructed our tree
*/

void build()
{
    Node *l, *r;
    priority_queue<Node *, vector<Node *>, Comparison> temp(huffman_tree);
    while (temp.size() > 1)
    {
        l = temp.top();
        temp.pop();
        r = temp.top();
        temp.pop();

        st = new Node((l->freq + r->freq));
        st->left = l;
        st->right = r;

        temp.push(st);
    }
}
// for generating code : we assign 0 to left and 1  to right
void generate_codes(Node *cur, string code)
{
    if (!cur)
        return;

    if (!cur->left && !cur->left)
    {
        cur->code = code;
        return;
    }
    generate_codes(cur->left, "0");
    generate_codes(cur->right, "1");
}
// this to convert biary represented as string
// to value so that it would be smaller in size
// this can be considered as the heart of compression
int bin_conv(string s)
{

    // nullptr is the '\0' but when it was '\0' it didn't work
    // 2 is base 2 to specify it's binary
    return stoi(s, nullptr, 2);
}

// this key is for decoding it would be apppended in the begining
void construct_key()
{
    string temp = "";
    // firstly we will add the size of the tree to reconstruct it
    priority_queue<Node *, vector<Node *>, Comparison> codes(huffman_tree);
    enc += (char)huffman_tree.size();
    Node *cur;
    while (!codes.empty())
    {
        cur = codes.top();
        enc += cur->data; // this the charachter
        // we will make each char 128 bits as we have 128 char used
        // if we know which chars would be input we could have made it smaller
        // we will make the MSB all 0s and put 1 as seperator then the code
        temp.assign(127 - cur->code.size(), '0');
        temp += '1';
        temp += cur->code;
        enc += (char)bin_conv(temp.substr(0, 8));
        // as each char in ascii is 8 bits we will divide it to 8 bits
        for (int i = 0; i < 15; i++)
        {
            temp = temp.substr(8);
            enc += (char)bin_conv(temp.substr(0, 8));
        }
        codes.pop();
    }
}

void encode(string fpath)
{
    construct_key();

    // reading file to compress
    string line;
    ifstream file;
    file.open(fpath);
    char c;
    while (!file.eof())
    {
        line += char_arr[c]->code;
        // encoding every 8 bit
        while (line.length() > 8)
        {
            enc += (char)bin_conv(line.substr(0, 8));
            line = line.substr(8);
        }
        file.get(c);
    }
    file.close();

    // seeing if there are remaining non-encoded bits
    int x = 8 - line.size();
    if (x < 8)
        line.append(x, '0');
    enc += (char)x;
}

void write_file(string fpath)
{
    fstream file;
    file.open(fpath, ios::out | ios::binary);

    file << enc;

    file.close();
}

void compress(string fpath)
{
    arrange_freq(fpath);
    build();
    generate_codes(st, "");
}

/****************************************/
