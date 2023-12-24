#include "huffman.h"
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