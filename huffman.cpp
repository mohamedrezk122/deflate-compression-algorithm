#include "huffman.h"

/// @brief internal function to get the frequency of each letter
/// @param fpath 
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

/// @brief building huffman tree
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

/// @brief generating binary code for each charachter
/// @param cur the node we are at 
/// @param code the code to be assigned to it
void generate_codes(Node *cur, string code)
{
    if (!cur)
        return;

    if (!cur->left && !cur->left)
    {
        cur->code = code;
        return;
    }
    generate_codes(cur->left, code + "0");
    generate_codes(cur->right, code + "1");
}


/// @brief this to convert biary represented as string to value so that it would be smaller in size
/// @param s the binary code
/// @return an integer representing it
int bin_conv(string s)
{

    // nullptr is the '\0' but when it was '\0' it didn't work
    // 2 is base 2 to specify it's binary
    return stoi(s, nullptr, 2);
}

// this key is for decoding it would be apppended in the begining

/// @brief generating key for decoding, it would be apppended in the begining
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

/// @brief compress the file
/// @param fpath 
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
    enc += (char)bin_conv(line.substr(0, 8));
    enc += (char)x;
}


/// @brief organizing the compressin process can be discarded by calling the functions in order
/// @param fpath 
void compress(string fpath)
{
    arrange_freq(fpath);

    build();
    generate_codes(st, "");
    encode(fpath);
}

/**************************************decompression**************************************************/

/// @brief takes the letter takin from the file and turn it to binary
/// @param dec the number coming from the binary file
/// @return the binary value as string
string dec_Bin_conv(int dec)
{
    string res = "";
    for (int i = 7; i > -1; i--)
    {
        // using bitmasks to get the binary value starting from MSB
        if ((dec >> i) & 1)
            res += '1';
        else
            res += '0';
    }
    return res;
}

/// @brief takes the charachter and it's code than reconstruct the huffman tree as 0 to left 1 to right
/// @param letter the letter that have this code
/// @param huffman_code the code extracted to decipher
void rebuild_codes(char letter, string huffman_code)
{
    // we will start at the root as it's the MSB
    Node *cur = st;
    for (int i = 0; i < huffman_code.size(); i++)
    {
        if (huffman_code[i] == '0')
        {
            if (!cur->left)
                cur->left = new Node(0);
            cur = cur->left;
        }
        else
        {
            if (!cur->right)
                cur->right = new Node(0);
            cur = cur->right;
        }
    }
    // after finishing the traversing and arriving at the node we set it's data to the letter
    cur->data = letter;
}

/// @brief decoding the key that was ued to encode
/// @param fpath the path of the file
void decode_key(string fpath)
{
    fstream enc_file(fpath, ios::in | ios::binary); // this will treat the file as binary
                                                    // as file will be read in binary and we want to take the input as charachters as we encoded
                                                    // we will use (reinterpret_cast) which is a way to cast the return var from function to the type we need
    char tree_size;
    enc_file.read(reinterpret_cast<char *>(&tree_size), 1); // read 1 byte as it's the size as explained in encoding

    // setting the st
    st = new Node(0);
    // each tree node contain the ascii letter = 1 byte
    // + huffman code encoded in 128 bit (to have all letter combinations as explained above)
    // it took an integar every 8 bits
    // 128 / 8 = 16 byte
    while (tree_size--)
    {
        char c; // the char
        enc_file.read(&c, 1);

        char code_int[16]; // the binary code of huffman
        enc_file.read(reinterpret_cast<char *>(code_int), 16);

        string huffman_code = "";
        for (int i = 0; i < 16; i++)
        {
            huffman_code += dec_Bin_conv(code_int[i]);
        }

        // Removing 0s by ignoring first and seperating 1 as stated above
        int i = 0;
        while (huffman_code[i] == '0')
        {
            i++;
        }
        huffman_code = huffman_code.substr(i + 1);

        rebuild_codes(c, huffman_code);
    }
    enc_file.close();
}


/// @brief decompress the file using the key after building the tree
/// @param fpath_in the file to decompress
/// @param fpath_out the file to output
void decompress(string fpath_in, string fpath_out)
{
    fstream enc_file(fpath_in, ios::in | ios::binary);


    decode_key(fpath_in);

    char tree_size;
    enc_file.read(reinterpret_cast<char *>(&tree_size), 1);

    // the key was (1 char for size + size * (char for letter +16 char for huffman))
    // we start after 17 *size +1 char
    // we might have trailing 0s to ignore
    // for getting the number of trailing 0s we will ste the pointer to the end of file with offset -1
    enc_file.seekg(-1, ios::end);
    char trail_zero;
    enc_file.read(reinterpret_cast<char *>(&trail_zero), 1);

    // now setting poiter to start after key and parse the file
    enc_file.seekg(1 + 17 * tree_size, ios::beg);

    string to_dec = "";
    char c;

    enc_file.read(reinterpret_cast<char *>(&c), 1);

    while (!enc_file.eof())
    {
        to_dec += c;
        enc_file.read(reinterpret_cast<char *>(&c), 1);
    }

    Node *cur = st;
    string huffman_code;
    for (int i = 0; i < to_dec.size()-1; i++)
    {
        // getting huffman code
        huffman_code = dec_Bin_conv(to_dec[i]);
        if (i == (to_dec.size() - 2))
        {
            huffman_code = huffman_code.substr(0, 8 - trail_zero);
        }

        // get data from tree

        for (int j = 0; j < huffman_code.size(); j++)
        {
            if (!cur)
            {
                cur = st;
                break;
            }

            if (huffman_code[j] == '0')
            {
                cur = cur->left;
            }
            else
            {
                cur = cur->right;
            }

            if (!cur->left && !cur->right)
            {
                decoded_string += (cur->data);
                cur = st;
            }
        }
    }
    enc_file.close();
}

/// @brief for writing the file
/// @param fpath the path to output to
/// @param f outputting : (1: decompressed) , (0:compressed)
void write_file(string fpath,bool f)
{
    fstream file;
    file.open(fpath, ios::out | ios::binary);

    file << (f ? decoded_string:enc);

    file.close();
}