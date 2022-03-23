#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"
class Node;
class SymbolTable
{
private:
    Node *head;
    Node **StateScope;
    int ScopeStatus;
    int capacity;
public:
    SymbolTable()
    {
        this->head = NULL;
        this->ScopeStatus = 0;
        this->capacity = 100;
        this->StateScope = new Node*[this->capacity];
        for  (int i = 0; i < this->capacity; i++)
        {
            this->StateScope[i] = NULL;
        }
    }
    void run(string filename);
    void insert(string name,string type, string sataticValue);
    void print();
    void lookup(string name);
    void assign(string name, string method);
    // Splay function
    void RemoveNode(Node* key);
    //Maybe update splay: insert, lookup, assign
    void splay(Node* p);
    //---------------
    int CompareOrder(Node* src, Node* des);
    void Print(string* ss, Node *root);
    void endDestroy(Node *key);
    // TINY function
    bool Slove(int Pindex, string Phash, string RealParament, int& numcompare, int& numsplay, string WarningCode);
    // non print function
    void begin();
    void end();
    void extend();
    void detruction();
    void detructionNode(Node *root);
    ~SymbolTable()
    {
        this->detruction();
        //cout << "happy";
    }
};
class Node
{
private:
    string name, FParament;
    int scope;
    int FPindex;
    char type;
    Node *left;
    Node *right;
    Node *chain;
    Node *parent;
    friend class SymbolTable;
public:
    Node()
    {
        this->name = "";this->FParament = "";
        this->type = '\0';
        this->scope = 0;
        this->FPindex = -1;
        this->left = NULL;this->right = NULL;this->chain = NULL;this->parent = NULL;
    }
};
#endif