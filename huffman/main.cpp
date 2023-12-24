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
int bin_conv(string s);
void construct_key(priority_queue<Node * ,vector<Node *>,Comparison> &codes);
void compress(string fpath);
void write_file(string fpath);
string enc;
int main(){
    string s=".\\sample.xml";
    compress(s);
    write_file(".\\here.xml");
}
void Huffman:: arrange_freq(string xml){
    // initializing charachter arrays for taking frequency
    Node *temp=nullptr;
    for(int i=0;i<128;i++){
        temp = new Node(0);temp->data=i;
        this->char_arr.push_back(temp);
    }

    // getting frequency (the redundancy could have been avoided by map)
    for(int i=0;i<xml.size();i++){
        this->char_arr[xml[i]]->freq++;
    }

    // making the minimum heap for building ordered tree
    for(int i=0;i<128;i++){
        if(this->char_arr[i]->freq > 0)
            this->children.push(this->char_arr[i]);
    }
    build();

}
/*
huffman coding works as following : 
1/ we need to assign each charachter to a binary code
2/ we want the most frequent to have the shortest binary code
3/ so the lowest frequency must be at the bottom
4/ in our children prior queue we have the minimum frequency ready to be popped
5/ our root will be at the bottom after we constructed our tree
*/

void Huffman::build(){
    Node*l,*r;
    priority_queue<Node * ,vector<Node *>,Comparison> temp(this->children);
    while(temp.size()>1){
        l=temp.top();temp.pop();
        r=temp.top();temp.pop();

        this->root=new Node(l->freq+r->freq);
        this->root->left=l;
        this->root->right=r;

        temp.push(root);
    }
    generate_codes(this->root,"");
    
}


// for generating code : we assign 0 to left and 1  to right
void Huffman::generate_codes(Node *cur,string code){
    if(!cur)return;

    if(!(cur->left||cur->left)){
        cur->code=code;
        return;
    }
    generate_codes(cur->left,"0");
    generate_codes(cur->right,"1");
}


/******************finished constructing the tree*********************/

//public
Huffman::Huffman(string fpath){
    string xml;
    char c;
    ifstream file;
    file.open(fpath);
    file.get(c);
    while (!file.eof())
    {
        xml += c;
        file.get(c);
    }
    arrange_freq(xml);
        
}

Node * Huffman::get_root(){
    return  this->root;
}

priority_queue<Node * ,vector<Node *>,Comparison> Huffman::get_children(){
    return this->children;
}

vector<Node *> Huffman::get_codes(){
    return this->char_arr;
}





/********************************************************/

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
