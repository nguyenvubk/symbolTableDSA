#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"
class Node;
class SymbolTable
{
private:
    Node **head;
    Node **StateScope;
    int ScopeStatus;
    int capacity;
    int *key;
    int mode;
    string hashname(string name, int index = 0)
    {
        //aBC12
        if (name[index] == '\0')
            return "";
        return to_string(name[index] - 48) + hashname(name, index + 1);
    }
    string hash(string name)
    {
        return to_string(this->ScopeStatus) + hashname(name, 0);
    }
    

public:
    SymbolTable()
    {
        this->head = NULL;
        this->ScopeStatus = 0;
        this->capacity = 20;
        this->StateScope = new Node *[this->capacity];
        for (int i = 0; i < this->capacity; i++)
        {
            this->StateScope[i] = NULL;
        }
        this->key = NULL;
        this->mode = -1;
    }
    void set(string instruction);
    void run(string filename);
    int decode(string instruction,int key[]);
    void begin();
    void end();
    void endDestroy(Node *key);
    void extend();
    void detruction();
    int removeSpace(string str, string strM[]);
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
    Node *chain;
    int location;
    friend class SymbolTable;

public:
    Node()
    {
        this->name = "";
        this->FParament = "";
        this->type = '\0';
        this->scope = 0;
        this->FPindex = -1;
        this->chain = NULL;
        this->location = -1;
    }
};
#endif