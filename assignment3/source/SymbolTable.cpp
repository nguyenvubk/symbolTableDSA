#include "SymbolTable.h"
int SymbolTable::removeSpace(string str, string *strM)
{
    string word = "";
    int i = 0;
    for (auto x : str)
    {
        if (x == '\r')
            break;
        if (x == ' ' && i < 2)
        {
            strM[i] = word;
            i++;
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    strM[i] = word;
    return i;
}
void SymbolTable::run(string filename)
{
    ifstream infile(filename);
    string line;
    bool first = 1;
    while (getline(infile, line))
    {
        if (first){
            //cout << line << endl;
            this->set(line);
            first = 0;
            continue;
        }
        string getWord[3] = {""};
        int hashf = this->removeSpace(line, getWord);
        //cout << getWord[0] << " " << getWord[1] << " " <<  getWord[2] << '\n';
        if (hashf == 0 && getWord[0] == "BEGIN")
            this->begin();
        else if (hashf == 0 && getWord[0] == "END")
            this->end();
        else
            throw InvalidInstruction(line);
    }
    if (this->ScopeStatus != 0)
        throw UnclosedBlock(this->ScopeStatus);
    infile.close();
}
void SymbolTable::set(string instruction)
{
    // LI :0 QU:1 DO:2
    if (regex_match(instruction, regex("(LINEAR)[ ][0-9]{1,6}[ ][0-9]{1,6}")))
    {
        this->key = new int[2];
        this->decode(instruction,this->key);
        this->head = new Node*[this->key[0]];
        for (int i = 0; i < this->key[0]; i++)
            this->head[i] = NULL;
        this->mode = 0;
        return;
    }
    if (regex_match(instruction, regex("(QUADRATIC)[ ][0-9]{1,6}[ ][0-9]{1,6}[ ][0-9]{1,6}")))
    {
        this->key = new int[3];
        this->decode(instruction,this->key);
        this->head = new Node*[this->key[0]];
        for (int i = 0; i < this->key[0]; i++)
            this->head[i] = NULL;
        this->mode = 1;
        return;
    }
    if (regex_match(instruction, regex("(DOUBLE)[ ][0-9]{1,6}[ ][0-9]{1,6}")))
    {
        this->key = new int[2];
        this->decode(instruction,this->key);
        this->head = new Node*[this->key[0]];
        for (int i = 0; i < this->key[0]; i++)
            this->head[i] = NULL;
        this->mode = 3;
        return;
    }
    throw InvalidInstruction(instruction);
}
int SymbolTable::decode(string instruction, int key[])
{
    string word = "";
    int i = 0;
    bool flag = 0;
    for (auto x : instruction)
    {
        if (x == '\r')
            break;
        if (!flag){
            if (x == ' ')
                flag = 1;
            continue;
        }
        if (x == ' '){
            key[i] = stoi(word);
            i++;
            word = "";
        }
        else{
            word = word + x;
        }
    }
    key[i] = stoi(word);
    return i;
}
//BEGIN END or something else
void SymbolTable::extend()
{
    this->capacity = this->capacity * 2;
    Node **Chace = new Node *[this->capacity];
    for (int i = 0; i <= this->ScopeStatus; i++)
    {
        Chace[i] = this->StateScope[i];
    }
    for (int i = this->ScopeStatus + 1; i < this->capacity; i++)
    {
        Chace[i] = NULL;
    }
    delete this->StateScope;
    this->StateScope = Chace;
}
void SymbolTable::detruction()
{
    if (this->head == NULL)
        return;
    for (int i = 0; i < this->key[0];i++)
    {
        delete this->head[i];
    }
    delete this->head;
    delete this->StateScope;
    delete this->key;
}
//Begin
void SymbolTable::begin()
{
    if (this->ScopeStatus >= this->capacity - 1)
        this->extend();
    this->ScopeStatus++;
}
//end
void SymbolTable::end()
{
    if (this->ScopeStatus == 0)
        throw UnknownBlock();
    Node *DelNode = this->StateScope[this->ScopeStatus];
    if (DelNode != NULL)
    {
        this->endDestroy(DelNode);
    }
    this->StateScope[this->ScopeStatus] = NULL;
    this->ScopeStatus--;
}
void SymbolTable::endDestroy(Node *key)
{
    if (key->chain != NULL)
        this->endDestroy(key->chain);
    this->head[key->location] = NULL;
    delete key;
    return;
}