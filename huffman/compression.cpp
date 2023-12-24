#include "compression.h"

//this to convert biary represented as string
//to value so that it would be smaller in size
//this can be considered as the heart of compression
int bin_conv(string s){
    
    // nullptr is the '\0' but when it was '\0' it didn't work
    // 2 is base 2 to specify it's binary
    return stoi(s,nullptr,2);

}


//this key is for decoding it would be apppended in the begining
void construct_key(priority_queue<Node * ,vector<Node *>,Comparison> &codes){
    string temp="";
    // firstly we will add the size of the tree to reconstruct it
    enc+=codes.size();
    Node* cur;
    while(!codes.empty()){
        cur=codes.top();
        enc+=cur->data;// this the charachter
        // we will make each char 128 bits as we have 128 char used
        // if we know which chars would be input we could have made it smaller
        // we will make the MSB all 0s and put 1 as seperator then the code
        temp.assign(127-cur->code.size(),'0');
        temp+='1';temp+=cur->code;
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




void compress(string fpath){
    Huffman *comp=new Huffman(fpath);
    priority_queue<Node * ,vector<Node *>,Comparison> codes_tree=comp->get_children();
    enc="";
    construct_key(codes_tree);
    vector<Node *> codes=comp->get_codes();

    // reading file to compress
    string line;
    ifstream file;
    file.open(fpath);
    char c;
    while (!file.eof())
    {
        line += codes[c]->code;
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
    int x= 8-line.size();
    if(x<8)line.append(x,'0');
    enc+=(char)x;

}

void write_file(string fpath){
    fstream file;
    file.open(fpath,ios::out | ios::binary);

    file<<enc;

    file.close();
}
