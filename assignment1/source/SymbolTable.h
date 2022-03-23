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
        for  (int i = 0; i < 100; i++)
        {
            this->StateScope[i] = NULL;
        }
    }
    void run(string filename);
    void check();
    void insert(string name, string type);
    void assign(string name, string data);
    void begin();
    void end();
    void lookup(string name);
    void print();
    void rprint();
    // //
    void extend();
    void detruction();
    ~SymbolTable()
    {
        this->detruction();
        //cout << "happy" << '\n';
    }
    
};
class Node
{
private:
    string name, data, type;
    bool hide;
    int scope;
    Node *next;
    Node *prev;
    Node *hideNode;
    friend class SymbolTable;
public:
    Node()
    {
        this->name = "";this->data = "";this->type = "";
        this->scope = 0;
        this->hide = false;
        this->next = NULL;this->prev = NULL;this->hideNode = NULL;
    }
};
#endif