#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>
#include "fstream"

using namespace std;

/**
 * A struct to keep information about one symbol,
 * which includes it's code and frequency;
 * also it contains infromation to build Huffman tree
 */
struct Node{
public:
    /**
     * Node's constructor
     * @param ch symbol to encode
     */
    Node(char ch){
        data = ch;
        frequency = 1; // first meeting wtih this symbol
        code = ""; // for the begining the code is empty
    }

    /**
     * frequency of the symbol in a text
     */
    int frequency = -1;
    /**
     * code for the symbol
     */
    string code;
    /**
     * the symbol itself
     */
    char data;
    /**
     * left node in a tree (considering Huffman coding)
     */
    Node* left = nullptr;
    /**
     * right node in a tree (considering Huffman coding)
     */
    Node* right = nullptr;

    /**
     * comparison between two nodes using their frequency
     * @param n1 first node
     * @param n2 second node
     * @return true if the first node's frequency is bigger than the second node's frequency; false - otherwise
     */
    static bool comparator(Node* n1, Node* n2){
        return n1->frequency < n2->frequency;
    }
};

/**
 * A class that represents basic methods for both of the algorithms in the assignment
 */
class Code{
private:
    /**
     * method to read all bypes of the file
     * @param filename name of the file to read all bytes of it
     * @return the vector of chars of this file
     */
    std::vector<char> ReadAllBytes(char const* filename)
    {
        ifstream ifs(filename, ios::binary|ios::ate);
        ifstream::pos_type pos = ifs.tellg(); // number of chars inside the vector<char>

        std::vector<char>  result(pos); // vector to put information in

        ifs.seekg(0, ios::beg); // find the begining of the file
        ifs.read(&result[0], pos); // read all bytes of the file into a vector

        for (int i = 0; i < result.size() - 1; ++i) { // dealing with strf problem
            if (result[i] == '\r' && result[i+1] == '\n')
            {
                result.erase(result.begin() + i);
            }
        }
        return result;
    }

    /**
     * Method to convert char's cipher representation into binary representation
     * @param ch char to convert into binary string
     * @param f true if it's the last char in the file and isn't full
     * @param c flag t be aware if '0' or '1' were added to the needed info
     * @return string's binary representation of (int) ch
     */
    string dec_bin(char ch, bool f, char c){
        string s = "";

        int i = ch;
        if (i < 0)
        {
            i += 256; // making number positive
            steps += 1;
        }
        // decimal to binary
        while (i != 0){
            if (i % 2 == 1){
                s = "1" + s;
            } else
                s = "0" + s;
            i /= 2;
            steps += 5;
        }
        //if length of the string is lower than 8, complete
        while (s.length() < 8) {
            s = "0" + s;
            steps += 5;
        }
        // this is the situation when while writing encoded text, the last partition of the text was shorter than 8
        // and was filled with additional info (0 or 1)
        if (f) {
            Node *n;
            string s1;
            i = s.length() - 1;
            while ((c == 'a' && s[i] == '0') || (c == 'b' && s[i] == '1'))
            {
                i--;
                steps += 5;
            }
            s1 = s.substr(0, (unsigned int) (i + 1));
            steps += 10;
            return s1;
        }
        steps += 5;
        return s;
    }

    /**
     * Method to convert binary representation in string format to char
     * @param s string's representation of the code
     * @return char that is equal to binary data
     */
    unsigned char to_char(string s){
        unsigned char k = 0;
        int l = s.length() - 1; // length of the string to use as a index
        for (int i = l; i >= 0; --i) {
            if (s[l - i] == '1')
            {
                k += pow(2, i); // convert to decimal
            }
        }
        return k;
    }

protected:

    /**
     * Method to read the table and text and write decoded information into a new file
     * @param path to the file with encoded text
     * @param path1 to the file with decoded text to write into
     */
    virtual void ReadTable(string path, string path1){
        ifstream inputFile;
        inputFile.open(path);
        symbols.clear();

        vector<char> vector1 = ReadAllBytes(path.c_str()); // represents all bytes inside the path

        //first char in file is the number of symbols in the table
        int n = vector1[0] > 0 ? vector1[0] : 256 + vector1[0];

        Node* node;
        int i = 1;

        //read the table for decoding
        for (int j = 0; j < n; ) {
            node = new Node(vector1[i]);
            i += 2;
            char c = vector1[i];
            string code = "";
            while(vector1[i] != ' '){ //reading bytes of the code for a symbol
                code += vector1[i];
                i++;
            }
            node->code = code;
            symbols.push_back(node); // adding the read node to the vector
            j++;
            i++;
        }
        i++;

        string decodeT = ""; // string which contains decoded text
        string s = ""; // string which contains binary representation of the encoded text
        int size = vector1.size(); //size of the vector with chars
        for (i; i < size - 1; ++i) {
            s += dec_bin(vector1[i],
                         i == size - 2 && (vector1[size - 1] == 'a' || vector1[size - 1] == 'b'), vector1[size - 1]);
        }

        string temp = "";
        for (int k = 0; k < s.size(); ++k) {
            temp += s[k];
            node = getNode(temp);
            if (node != nullptr){
                decodeT += node->data; // adding oen more decoded symbol
                temp = "";
                steps += 3;
            }
            steps += 9;
        }
        ofstream output(path1);
        output << decodeT; // writing decoded text into a new file
        output.close();
    }

    /**
     * Method to write the decoded text
     * @param path to the file we are encoding
     * @param path1 to the file where to write the decoded info
     */
    virtual void WriteText(string path, string path1){
        ofstream fout(path1);
        fout.put((char) symbols.size()); // fisrt is the size of the table
        for (int i = 0; i < symbols.size(); ++i) {
            fout.put(symbols[i]->data);
            fout.put(' ');
            string s = symbols[i]->code;
            for (int j = 0; j < s.length(); ++j) {
                fout.put(s[j]);
            }
            fout.put(' '); // all other data in the table is splited with spaces
        }
        fout.put('\n');

        FILE *inputFile;
        inputFile = fopen( path.c_str(), "r" ); // read file once again

        string s = "";

        char ch = 'f'; // flag to no additional bytes

        while( fscanf( inputFile, "%c", &ch ) != EOF )
        {
            char c;
            s += getNode(ch)->code; //read text and dynamically add new symbols
        }
        fclose( inputFile );

        //convert code to sequence of bytes
        for (unsigned int i = 0; i < s.length(); i+=8)
        {
            unsigned char byte = 0;
            string str8 = "";
            //get 8 - length string of bytes
            if (i + 8 < s.length())
            {
                str8 = s.substr(i, 8);
            }
            else
            {
                str8 = s.substr(i, s.length() - i);
                // if last 8 - length string is shorter than 8, it must be filled to size
                char t = str8[str8.length()-1] == '0' ? '1' : '0';

                while (str8.size() < 8)
                {
                    str8 += t;
                }

                if (str8[7] == '0')
                {
                    ch = 'a';
                }
                else {
                    ch = 'b';
                }
            }

            byte = to_char(str8);
            fout.put(byte); // write final byte to new file
        }

        fout.put(ch); //write flag to the end of the file
        fout.close();
    }

    /**
     * Method to read the whole text and count symbols' frequncy to create codes
     * @param path to the file we'll be reading
     */
    void ReadText(string path){
        FILE *inputFile;
        inputFile = fopen( path.c_str(), "r" ); // open to read
        char ch;
        while( fscanf( inputFile, "%c", &ch ) != EOF )
        {
            Node* n = getNode(ch);
            if(n == nullptr) // if there's no Node with such data, then add a new Node
            {
                symbols.push_back(new Node(ch));
            }
            else // incriminate frequency for current Node
            {
                n->frequency++;
            }
            steps += 7;
        }
    }

    /**
     * Sort symbols by their frequency
     */
    void sortSymbols(){
        steps += 3;
        sort(symbols.begin(), symbols.end(), Node::comparator);
    }

    /**
     * Find node in the vector of known symbols in the text
     * @param s find node by it's code
     * @return Node if found; nullptr otherwise
     */
    Node* getNode(string s){
        for (int i = 0; i < symbols.size(); ++i) {
            if (symbols[i]->code == s)
                return symbols[i];
            steps += 7;
        }
        return nullptr;
    }

    /**
     * Find node in the vector of known symbols in the text
     * @param ch fin Node by it's char
     * @return Node if found; nullptr otherwise
     */
    Node* getNode(char ch){
        for (int i = 0; i < symbols.size(); ++i) {
            if (symbols[i]->data == ch)
                return symbols[i];
            steps += 7;
        }
        return nullptr;
    }

    /**
     * Steps of the algo counter
     */
    unsigned long long steps;
    /**
     * symbols of the text to be en/de-coded
     */
    vector<Node*> symbols;
};

/**
 * Struct that represents tree of nodes above for the Huffman algorithm
 */
struct Tree{
    /**
     * frequency of the overall
     */
    int sum_frequency;
    /**
     * root of the tree
     */
    Node* head;

    /**
     * concstructor of a Tree
     * @param h root node
     * @param freq overall frequency
     */
    Tree(Node* h, int freq){
        head = h;
        sum_frequency = freq;
    }
};

/**
 * Class specified for Huffman coding (implements from the Code class)
 */
class Huffman: protected Code{
public:
    // checking data to be empty
    Huffman(){
        tree = nullptr;
        trees.clear();
        symbols.clear();
        steps = 4;
    }

    //counting steps
    unsigned long long steps;

    /**
     * encoding method
     * @param path from which text is
     */
    void encode(string path){
        if (!ifstream(path)){
            cout << "File doesn't exist!\n";
            return;
        }

        if (path.find_last_of(".txt") != path.length() - 1){
            cout << "Type of file doesn't match! It must be txt\n";
            return;
        }

        steps = 0;
        Code::steps = 0;

        ReadText(path); // working with file part
        sortSymbols();

        BuildHuffmanTree(); // build a tree
        TraverseTree(tree->head); // assign codes to leaves

        string path1 = path;
        path1.replace(path1.end() - 3, path1.end(), "huff"); // create path for file to encode
        WriteText(path, path1); // write encoded text and table

        vector<Node*>().swap(symbols);
        vector<Node*>().swap(Code::symbols);
        vector<Tree*>().swap(trees);
        delete(tree);//free memory
        tree = nullptr;

        steps += 14 + Code::steps;
    }

    /**
     * Method to decode text
     * @param path from which to decode text
     */
    void decode(string path){
        if (!ifstream(path)){
            cout << "File doesn't exist!\n";
            return;
        }

        if (path.find_last_of(".huff") != path.length() - 1){
            cout << "Type of file doesn't match! It must be huff\n";
            return;
        }

        steps = 0;
        Code::steps = 0;

        string path1 = path;
        path1.replace(path1.end() - 5, path1.end(), "-unz-h.txt"); // path where write decoded text

        ReadTable(path, path1);

        vector<Node*>().swap(symbols);
        vector<Tree*>().swap(trees);
        delete(tree);// free memory
        tree = nullptr;

        steps += 10 + Code::steps;
    }

private:
    /**
     * build huffman tree
     */
    void BuildHuffmanTree(){
        // for the beginning create vector of trees with one node each
        for (int i = 0; i < symbols.size(); ++i) {
            trees.push_back(new Tree(symbols[i], symbols[i]->frequency));
            steps += 10;
        }
        steps++;
        // composing by two lowest frequency
        while (trees.size() > 1){
            extractMin();
            steps += 4;
        }
    }

    /**
     * insert new tree into trees vector
     * @param t new tree
     */
    void insert(Tree* t){
        bool f = false; // flag if there are trees with bigger frequencies
        steps += 2;
        for (int i = 0; i < trees.size(); ++i) {
            steps += 3;
            if (trees[i]->sum_frequency >= t->sum_frequency){ // if finding a tree with greater frequency
                vector<Tree*>::iterator it = trees.begin() + i; //insert before that tree
                trees.insert(it, t);
                f = true;
                steps += 8;
                break;
            }
        }
        if (!f)
        {
            trees.push_back(t);
            steps += 2;
        }
    }

    /**
     * Treverse the tree to assign codes for chars in this tree startign from head
     * @param n head of the tree
     */
    void  TraverseTree(Node * n){
        // '0' for left
        if (n->left != nullptr){
            n->left->code = n->code + "0";
            TraverseTree(n->left);
            steps += 6;
        }
        // '1' for right
        if (n->right != nullptr){
            n->right->code = n->code+"1";
            TraverseTree(n->right);
            steps += 6;
        }
        steps += 2;
    }

    /**
     * create new tree from two trees with lower frequecies
     */
    void extractMin(){
        int freq = trees[0]->sum_frequency + trees[1]->sum_frequency; // sum freq.
        Node* n = new Node(NULL); // create root for new tree
        n->frequency = freq; // assign values
        n->left = trees[0]->head; // assign values
        n->right = trees[1]->head; // assign values
        Tree* t = new Tree(n,freq); // create new tree
        if (trees.size() == 2){ // if there are only  two trees left
            tree = t;
            steps += 3;
        }
        trees.erase(trees.begin(), trees.begin() + 2); // erase old trees
        insert(t); // add new tree according two the order
        steps += 23;
    }

    /**
     * vector of trees for HuffmanTree()
     */
    vector<Tree*> trees;
    /**
     * main tree - result of HuffmanTree()
     */
    Tree* tree = nullptr;
};

/**
 * A class to represent Shannon-Fanno algorithm (implements the Code class)
 */
class Shen_Fan: protected Code{
public:
    // constructor, check for data
    Shen_Fan(){
        symbols.clear();
        steps = 0;
    }

    // steps counter
    unsigned long long steps;

    /**
     * method to encode text
     * @param path to the file which to encode
     */
    void encode(string path){
        if (!ifstream(path)){ // check existence
            cout << "File doesn't exist!\n";
            return;
        }

        if (path.find_last_of(".txt") != path.length() - 1){
            cout << "Type of file doesn't match! It must be txt\n";
            return;
        }

        steps = 0;
        Code::steps = 0;

        ReadText(path); // read the text from the file and fullfill symbols
        sortSymbols();

        SF(symbols); // build codes for symbols

        string path1 = path;
        path1.replace(path1.end() - 3, path1.end(), "shann"); // create path to a file where to encode

        WriteText(path, path1); // write encoded text

        steps += 10 + Code::steps;

        vector<Node*>().swap(symbols);
        vector<Node*>().swap(Code::symbols);
    }

    /**
     * method to decode text
     * @param path to the file from where to decode text
     */
    void decode(string path){
        if (!ifstream(path)){
            cout << "File doesn't exist!\n";
            return;
        }

        if (path.find_last_of(".shann") != path.length() - 1){
            cout << "Type of file doesn't match! It must be shann\n";
            return;
        }

        steps = 0;
        Code::steps = 0;

        string path1 = path;
        path1.replace(path1.end() - 6, path1.end(), "-unz-s.txt");// path where write decoded text

        ReadTable(path, path1);// read table and text and write decoded

        steps += 7 + Code::steps;

        vector<Node*>().swap(symbols);
        vector<Node*>().swap(Code::symbols);
    }

private:
    /**
     * method to count summ of the frequencies in the vector
     * @param v from which to count summary
     * @return sum
     */
    int getSum(vector<Node*> v){
        int sum = 0;

        for (int i = 0; i < v.size(); ++i) {
            sum += v[i]->frequency;
            steps += 7;
        }

        steps += 2;
        return sum;
    }

    /**
     * method to build codes for symbols by dividing tabla in two equal by frq-s parts
     * @param vector1 table
     */
    void SF(vector<Node*> vector1){
        int sum = getSum(vector1) / 2;
        int count = vector1[0]->frequency;
        int k = 0;
        for (int i = 1; i < vector1.size(); ++i) {
            if (abs(count + vector1[i]->frequency - sum) > abs(count - sum)){ // if sum of the one half is bigger than other
                k = i - 1; // index of the boundary node
                break;
            }
            count += vector1[i]->frequency; // kind of sum
        }
        //divide in two vectors
        vector<Node*> v1;
        for (int i = 0; i < k + 1; ++i) {
            vector1[i]->code += "0";
            v1.push_back(vector1[i]);
            steps += 9;
        }

        vector<Node*> v2;
        for (int i = k + 1; i < vector1.size(); ++i) {
            vector1[i]->code += "1";
            v2.push_back(vector1[i]);
            steps += 10;
        }

        //recursive division of parts
        if (v1.size() > 1){
            SF(v1);
            steps++;
        }
        if (v2.size() > 1){
            SF(v2);
            steps++;
        }
        steps += 13;
    }
};

/**
 * Method to get a required option in programm arguements
 * @param start of the arguements
 * @param end of the aruements
 * @param option required oprion to find
 * @return oprion characterisitic [here - filename]
 */
char* getOptCmd(char ** start, char ** end, const std::string & option)
{
    char ** it = std::find(start, end, option); //find needed option
    if (it != end && ++it != end) // if is not empty
    {
        return *it;
    }
    return 0;
}

/**
 * Method to check existence of the option
 * @param start of the args
 * @param end of the args
 * @param option required oprion to find
 * @return true - exists; false - otherwise
 */
bool optExist(char** start, char** end, const std::string& option)
{
    return std::find(start, end, option) != end;
}

/**
 * Method to work with command line
 */
void forCmd(int argc, char* argv[]){
    string help = "-h for help, -hf for Huffman, -sf for Shannon-Fanno, -c for encode, -d for decode, -f for filepath\n";

    if(optExist(argv, argv+argc, "-h"))
    {
        cout << help; // SOS-SOS-SOS-SOS-SOS
    }

    Huffman h;
    Shen_Fan shen_fan;

    bool sf = optExist(argv, argv + argc, "-sf"); // Shannon-F
    bool hf = optExist(argv, argv + argc, "-hf"); // Huffman
    bool c = optExist(argv, argv + argc, "-c"); // encode
    bool d = optExist(argv, argv + argc, "-d"); // decode
    bool f = optExist(argv, argv + argc, "-f"); // filename

    if (!(f && (c || d) && (sf || hf))){ // if not all of them in needed way
        cout << "One of the arguments is missing!\n";
        cout << help;
        return;
    }

    char * filename = getOptCmd(argv, argv + argc, "-f");
    string s = filename;
    if (!ifstream(s)){
        cout << "File doesn't exist!\n";
        return;
    }

    if (sf){
        if (c){
            shen_fan.encode(filename);
        }
        else {
            shen_fan.decode(filename);
        }
    }
    else{
        if (c){
            h.encode(filename);
        } else{
            h.decode(filename);
        }
    }
    cout << "Task finished successfully\n";
}

int main(int argc, char* argv[]) {
    forCmd(argc, argv);
    return 0;
}