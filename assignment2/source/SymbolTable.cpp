#include "SymbolTable.h"
/* Some tiny helpful function */
int removeSpace(string str, string strM[])
{
    string word = "";
    int i = 0;
    for (auto x : str)
    {
        if (x == '\r')
            break;
        if (x == ' ' && i < 3) //extend up 4, but only catch "space"
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
bool DefineName(string name)
{
    if (regex_match(name, regex("[a-z][A-Za-z0-9_]*")))
        return 1;
    else
        return 0;
}
// 0 number, 1 string, 2 function, -1 invalid
int typeOfDataInsert(string type)
{
    if (type == "number")
        return 0;
    else if (type == "string")
        return 1;
    else if (regex_match(type, regex("[(]((((number)|(string))([,]((number)|(string)))*)|)[)](->)((string)|(number))")))
        return 2;
    else
        return -1;
}
void InspectType(string InstructionType, char &TypeReturn, string &FParament, int &FPindex)
{
    FPindex = 0;
    for (int i = 1; InstructionType[i] != '\0'; i++)
    {
        if (InstructionType[i] != ',' && InstructionType[i] != ')')
            continue;
        if (InstructionType[i] == ')')
        {
            if (i == 1)
                FParament = "NULL";
            else if (InstructionType[i - 1] == 'r')
                FParament = FParament + "n";
            else
                FParament = FParament + "s";
            if (InstructionType[i + 3] == 'n')
                TypeReturn = 'n';
            else
                TypeReturn = 's';
            break;
        }
        if (InstructionType[i - 1] == 'r')
            FParament = FParament + "n";
        else
            FParament = FParament + "s";
        FPindex++;
    }
}
//non change function -----------------------------------------
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
    Node *DelNode = this->head;
    this->detructionNode(DelNode);
    this->head = NULL;
    delete this->StateScope;
}
void SymbolTable::detructionNode(Node *root)
{
    if (root->left != NULL)
        this->detructionNode(root->left);
    if (root->right != NULL)
        this->detructionNode(root->right);
    delete root;
}
//main function------------------------------------------------
void SymbolTable::run(string filename)
{
    ifstream infile(filename);
    string line;
    while (getline(infile, line))
    {
        string getWord[4] = {""};
        int hashf = removeSpace(line, getWord);
        //cout << getWord[0] << " " << getWord[1] << " " <<  getWord[2] << " " << getWord[3] << '\n';
        if (hashf == 3 && getWord[0] == "INSERT")
            this->insert(getWord[1], getWord[2], getWord[3]);
        else if (hashf == 2 && getWord[0] == "ASSIGN")
            this->assign(getWord[1], getWord[2]);
        else if (hashf == 3 && getWord[0] == "ASSIGN") //special case
            this->assign(getWord[1], getWord[2] + " " + getWord[3]);
        else if (hashf == 0 && getWord[0] == "PRINT") //bug that I dont know what it is
            this->print();
        else if (hashf == 1 && getWord[0] == "LOOKUP")
            this->lookup(getWord[1]);
        else if (hashf == 0 && getWord[0] == "BEGIN")
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
//update right here
void SymbolTable::splay(Node *p)
{
    if (p == NULL)
        return;
    if (p->parent == NULL)
    {
        this->head = p;
        return;
    }
    //zig or zag
    if (p->parent->parent == NULL)
    {
        if (p->parent->left == p) //zig
        {
            Node *parent = p->parent;
            parent->left = p->right;
            if (parent->left != NULL)
                parent->left->parent = parent;
            p->right = parent;
            parent->parent = p;
            p->parent = NULL;
            this->head = p;
            return;
        }
        else //zag
        {
            Node *parent = p->parent;
            parent->right = p->left;
            if (parent->right != NULL)
                parent->right->parent = parent;
            p->left = parent;
            parent->parent = p;
            p->parent = NULL;
            this->head = p;
            return;
        }
    }
    else
    {
        if (p->parent->left == p)
        {
            if (p->parent->parent->left == p->parent) //zig zig
            {
                Node *parent = p->parent;
                Node *granfa = parent->parent;
                parent->left = p->right;
                if (parent->left != NULL)
                    parent->left->parent = parent;
                granfa->left = parent->right;
                if (granfa->left != NULL)
                    granfa->left->parent = granfa;
                p->right = parent;
                parent->right = granfa;
                p->parent = granfa->parent;
                parent->parent = p;
                granfa->parent = parent;
                if (p->parent != NULL)
                {
                    if (p->parent->left == granfa)
                        p->parent->left = p;
                    else
                        p->parent->right = p;
                }
                this->splay(p);
                return;
            }
            else //zag zig
            {
                Node *parent = p->parent;
                Node *granfa = parent->parent;
                parent->left = p->right;
                if (parent->left != NULL)
                    parent->left->parent = parent;
                granfa->right = p->left;
                if (granfa->right != NULL)
                    granfa->right->parent = granfa;
                p->right = parent;
                p->left = granfa;
                p->parent = granfa->parent;
                parent->parent = p;
                granfa->parent = p;
                if (p->parent != NULL)
                {
                    if (p->parent->left == granfa)
                        p->parent->left = p;
                    else
                        p->parent->right = p;
                }
                this->splay(p);
                return;
            }
        }
        else
        {
            if (p->parent->parent->right == p->parent) //zag zag
            {
                Node *parent = p->parent;
                Node *granfa = parent->parent;
                parent->right = p->left;
                if (parent->right != NULL)
                    parent->right->parent = parent;
                granfa->right = parent->left;
                if (granfa->right != NULL)
                    granfa->right->parent = granfa;
                p->left = parent;
                parent->left = granfa;
                p->parent = granfa->parent;
                parent->parent = p;
                granfa->parent = parent;
                if (p->parent != NULL)
                {
                    if (p->parent->left == granfa)
                        p->parent->left = p;
                    else
                        p->parent->right = p;
                }
                this->splay(p);
                return;
            }
            else // zig zag
            {
                Node *parent = p->parent;
                Node *granfa = parent->parent;
                parent->right = p->left;
                if (parent->right != NULL)
                    parent->right->parent = parent;
                granfa->left = p->right;
                if (granfa->left != NULL)
                    granfa->left->parent = granfa;
                p->left = parent;
                p->right = granfa;
                p->parent = granfa->parent;
                parent->parent = p;
                granfa->parent = p;
                if (p->parent != NULL)
                {
                    if (p->parent->left == granfa)
                        p->parent->left = p;
                    else
                        p->parent->right = p;
                }
                this->splay(p);
                return;
            }
        }
    }
}
int SymbolTable::CompareOrder(Node *src, Node *des)
{
    // def < -1, = 0, > 1
    if (src->scope < des->scope)
        return -1;
    else if (src->scope > des->scope)
        return 1;
    else
    {
        int result = src->name.compare(des->name);
        if (result < 0)
            return -1;
        else if (result > 0)
            return 1;
        else
            return 0;
    }
}
//Insert //Just test
void SymbolTable::insert(string name, string type, string sataticValue)
{
    if (!DefineName(name))
        throw InvalidInstruction("INSERT " + name + " " + type + " " + sataticValue);
    int defType = typeOfDataInsert(type);
    if (defType == -1)
        throw InvalidInstruction("INSERT " + name + " " + type + " " + sataticValue);
    if (sataticValue != "true" && sataticValue != "false")
        throw InvalidInstruction("INSERT " + name + " " + type + " " + sataticValue);
    /*
    Regex: [(](((number)|(string))([,]((number)|(string)))*)*[)](->)((string)|(number))
    */
    //create new node, must delete if not done
    Node *NewNode = new Node;
    if (sataticValue[0] == 't')
        NewNode->scope = 0;
    else
        NewNode->scope = this->ScopeStatus;
    //invalidDeclare
    if (defType == 2 && NewNode->scope != 0)
    {
        delete NewNode;
        throw InvalidDeclaration("INSERT " + name + " " + type + " " + sataticValue);
    }
    NewNode->name = name;
    if (defType == 0)
        NewNode->type = 'n';
    else if (defType == 1)
        NewNode->type = 's';
    else
        InspectType(type, NewNode->type, NewNode->FParament, NewNode->FPindex);
    //cout << NewNode->FPindex << "|" << NewNode->FParament << "|" << NewNode->type << '\n';
    //FPindex = index of last element example: (string)|() -> index = 0; (string,number) -> i = 1
    if (this->head == NULL)
    {
        this->head = NewNode;
        cout << "0 0" << '\n';
        if (NewNode->scope != 0)
        {
            NewNode->chain = this->StateScope[this->ScopeStatus];
            this->StateScope[this->ScopeStatus] = NewNode;
        }
        return;
    }
    if (sataticValue[0] == 't' && this->ScopeStatus != 0)
    {
        Node *RedeNode = this->StateScope[this->ScopeStatus];
        while (RedeNode != NULL)
        {
            if (RedeNode->name == NewNode->name)
            {
                delete NewNode;
                throw Redeclared("INSERT " + name + " " + type + " " + sataticValue);
            }
            RedeNode = RedeNode->chain;
        }
    }
    int NumCompare = 0, NumSplay = 0;
    Node *des = this->head;
    Node *check = this->head;
    while (check != NULL)
    {
        if (this->CompareOrder(NewNode, check) < 0)
        {
            NumCompare++;
            des = check;
            check = check->left;
        }
        else if (this->CompareOrder(NewNode, check) > 0)
        {
            NumCompare++;
            des = check;
            check = check->right;
        }
        else
        {
            delete NewNode;
            throw Redeclared("INSERT " + name + " " + type + " " + sataticValue);
        }
    }
    if (this->CompareOrder(NewNode, des) > 0)
        des->right = NewNode;
    else
        des->left = NewNode;
    NewNode->parent = des;
    this->splay(NewNode);
    NumSplay++;
    //put in to delete
    if (NewNode->scope != 0)
    {
        NewNode->chain = this->StateScope[this->ScopeStatus];
        this->StateScope[this->ScopeStatus] = NewNode;
    }
    cout << to_string(NumCompare) << " " << to_string(NumSplay) << '\n';
    return;
}
//print
void SymbolTable::print()
{
    if (this->head == NULL)
        return;
    string *ss = new string("");
    Node *PrintNode = this->head;
    *ss = *ss + PrintNode->name + "//" + to_string(PrintNode->scope);
    this->Print(ss, PrintNode->left);
    this->Print(ss, PrintNode->right);
    cout << *ss << '\n';
    delete ss;
}
void SymbolTable::Print(string *ss, Node *root)
{
    if (root == NULL)
        return;
    *ss = *ss + " " + root->name + "//" + to_string(root->scope);
    this->Print(ss, root->left);
    this->Print(ss, root->right);
}
//lookup
void SymbolTable::lookup(string name)
{
    if (this->head == NULL)
        throw Undeclared("LOOKUP " + name);
    Node *SearchNode = new Node; //must delete be done
    SearchNode->name = name;
    Node *Result = NULL;
    for (int i = this->ScopeStatus; i >= 0; i--)
    {
        SearchNode->scope = i;
        Node *check = this->head;
        while (check != NULL)
        {
            if (this->CompareOrder(SearchNode, check) < 0)
            {
                check = check->left;
            }
            else if (this->CompareOrder(SearchNode, check) > 0)
            {
                check = check->right;
            }
            else
            {
                Result = check;
                break;
            }
        }
        if (Result != NULL)
            break;
    }
    if (Result == NULL) //none match
    {
        delete SearchNode;
        throw Undeclared("LOOKUP " + name);
    }
    this->splay(Result);
    cout << to_string(Result->scope) << '\n';
    delete SearchNode;
    return;
}
//remove, key always be found
void SymbolTable::RemoveNode(Node *key)
{
    if (this->head == NULL)
        return;
    this->splay(key);
    if (this->head->left == NULL || this->head->right == NULL)
    {
        if (this->head->right != NULL)
        {
            Node *temp = this->head->right;
            delete this->head;
            this->head = temp;
            this->head->parent = NULL;
            return;
        }
        else
        {
            Node *temp = this->head->left;
            delete this->head;
            this->head = temp;
            if (this->head != NULL)
                this->head->parent = NULL;
            return;
        }
    }
    else //full
    {
        Node *temp = this->head->left;
        Node *RightNode = this->head->right;
        delete this->head;
        Node *RightTemp = temp;
        while (RightTemp->right != NULL)
        {
            RightTemp = RightTemp->right;
        }
        this->head = temp;
        this->head->parent = NULL;
        this->splay(RightTemp);
        this->head->right = RightNode;
        RightNode->parent = this->head;
        return;
    }
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
    this->RemoveNode(key);
    return;
}
//asssign testing and tiny function
//-1:bug, 0:number, 1:string, 2:name of data, 3:name of function
int InspectMethod(string method)
{
    if (regex_match(method, regex("[0-9]+")))
        return 0;
    if (regex_match(method, regex("['][A-Za-z0-9 ]*[']")))
        return 1;
    if (regex_match(method, regex("[a-z][A-Za-z0-9_]*")))
        return 2;
    if (regex_match(method, regex("([a-z][A-Za-z0-9_]*)[(](((([0-9]+)|(['][A-Za-z0-9 ]*['])|([a-z][A-Za-z0-9_]*))([,](([0-9]+)|(['][A-Za-z0-9 ]*['])|([a-z][A-Za-z0-9_]*)))*)|)[)]")))
        return 3;
    return -1;
}
//too much function is bad but Im lazzy :')
void SplitMethod(string method, string strM[])
{
    string word = "";
    int i = 0;
    for (auto x : method)
    {
        if (x == '(') //default is 2
        {
            strM[i] = word;
            i++;
            word = "";
        }
        else
        {
            if (x != ')')
                word = word + x;
        }
    }
    strM[i] = word;
}
//'''''''''''''''''''''''''''''''''''''''''''''''''
void SymbolTable::assign(string name, string method)
{
    if (this->head == NULL)
        Undeclared("ASSIGN " + name + " " + method);
    if (!DefineName(name))
        throw InvalidInstruction("ASSIGN " + name + " " + method);
    int methodINT = InspectMethod(method);
    if (methodINT == -1)
        throw InvalidInstruction("ASSIGN " + name + " " + method);
    Node *SearchNode = new Node;
    int numcompare = 0;
    int numsplay = 0;
    if (methodINT == 2)
    {
        //check not function and wrong type, if false delete Node and throw
        SearchNode->name = method;
        for (int i = this->ScopeStatus; i >= 0; i--)
        {
            SearchNode->scope = i;
            numcompare = 0;
            Node *check = this->head;
            while (check != NULL)
            {
                numcompare++;
                if (this->CompareOrder(SearchNode, check) < 0)
                {
                    check = check->left;
                }
                else if (this->CompareOrder(SearchNode, check) > 0)
                {
                    check = check->right;
                }
                else
                {
                    if (check->FPindex != -1)
                    {
                        delete SearchNode;
                        throw TypeMismatch("ASSIGN " + name + " " + method);
                    }
                    if (check->type == 'n')
                        methodINT = 0;
                    else
                        methodINT = 1;
                    if (check != this->head)
                    {
                        this->splay(check);
                        numsplay++;
                    }
                    break;
                }
            }
            if (methodINT != 2)
                break;
        }
        if (methodINT == 2)
        {
            delete SearchNode;
            throw Undeclared("ASSIGN " + name + " " + method);
        }
    }
    else if (methodINT == 3)
    {
        //Im so sad about this case
        //function always in global scope (0)
        //just this case left
        string header[2] = {""};
        SplitMethod(method, header); // 0 is name, 1 is sequence of parameter
        //cout << header[0] << "::" << header[1] << '\n';
        SearchNode->name = header[0];
        Node *check = this->head;
        while (check != NULL)
        {
            numcompare++;
            if (this->CompareOrder(SearchNode, check) < 0)
            {
                check = check->left;
            }
            else if (this->CompareOrder(SearchNode, check) > 0)
            {
                check = check->right;
            }
            else
            {
                if (check->FPindex == -1) //if not function
                {
                    delete SearchNode;
                    throw TypeMismatch("ASSIGN " + name + " " + method);
                }
                if (check->type == 'n')
                    methodINT = 0;
                else
                    methodINT = 1;
                if (check != this->head)
                {
                    this->splay(check);
                    numsplay++;
                }
                if (!this->Slove(check->FPindex, check->FParament, header[1], numcompare, numsplay, "ASSIGN " + name + " " + method))
                    throw TypeMismatch("ASSIGN " + name + " " + method);
                //last function here
                break;
            }
        }
        if (methodINT == 3)
        {
            delete SearchNode;
            throw Undeclared("ASSIGN " + name + " " + method);
        }
    }
    //simple love
    int numcompareSave = numcompare; //save satatus for numcompare
    SearchNode->name = name;
    for (int i = this->ScopeStatus; i >= 0; i--)
    {
        SearchNode->scope = i;
        numcompare = numcompareSave;
        Node *check = this->head;
        while (check != NULL)
        {
            numcompare++;
            if (this->CompareOrder(SearchNode, check) < 0)
            {
                check = check->left;
            }
            else if (this->CompareOrder(SearchNode, check) > 0)
            {
                check = check->right;
            }
            else
            {
                //check if is function
                if (check->FPindex != -1)
                {
                    delete SearchNode;
                    throw TypeMismatch("ASSIGN " + name + " " + method);
                }
                //here for 0,1,2 maybe 3
                if ((methodINT == 0 && check->type == 'n') || (methodINT == 1 && check->type == 's'))
                {
                    if (check != this->head)
                    {
                        this->splay(check);
                        numsplay++;
                    }
                    cout << to_string(numcompare) << " " << to_string(numsplay) << '\n';
                    delete SearchNode;
                    return;
                }
                //some case with 2 and 3 still here
                delete SearchNode;
                throw TypeMismatch("ASSIGN " + name + " " + method);
            }
        }
    }
    delete SearchNode;
    throw Undeclared("ASSIGN " + name + " " + method);
}
//last function
bool SymbolTable::Slove(int Pindex, string Phash, string RealParament, int &numcompare, int &numsplay, string WarningCode)
{
    int CountComma = 0;
    for (auto x : RealParament)
    {
        if (x == ',')
            CountComma++;
    }
    if (Pindex != CountComma)
        return false;
    if (Pindex == 0)
    {
        if ((Phash == "NULL" && RealParament != "") || (RealParament == "" && Phash != "NULL"))
            return false;
        if (Phash == "NULL" && RealParament == "")
            return true;
    }
    string AParament[CountComma + 1] = {""};
    int index = 0;
    string word = "";
    for (auto x : RealParament)
    {
        if (x == ',')
        {
            AParament[index] = word;
            index++;
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    AParament[CountComma] = word;
    for (int i = 0; i <= Pindex; i++)
    {
        //I dont know better solution now
        if (regex_match(AParament[i], regex("[0-9]+")))
        {
            if (Phash[i] == 's')
                return false;
        }
        else if (regex_match(AParament[i], regex("['][A-Za-z0-9 ]*[']")))
        {
            if (Phash[i] == 'n')
                return false;
        }
        else //important case
        {
            Node *SearchNode = new Node;
            SearchNode->name = AParament[i];
            int SaveCompare = numcompare;
            bool flag = false;
            for (int j = this->ScopeStatus; j >= 0; j--)
            {
                SearchNode->scope = j;
                numcompare = SaveCompare;
                Node *check = this->head;
                while (check != NULL)
                {
                    numcompare++;
                    if (this->CompareOrder(SearchNode, check) < 0)
                    {
                        check = check->left;
                    }
                    else if (this->CompareOrder(SearchNode, check) > 0)
                    {
                        check = check->right;
                    }
                    else
                    {
                        if (check->FPindex != -1)
                        {
                            delete SearchNode;
                            return false;
                        }
                        if (check->type != Phash[i])
                        {
                            delete SearchNode;
                            return false;
                        }
                        if (check != this->head)
                        {
                            this->splay(check);
                            numsplay++;
                        }
                        flag = true;
                        break;
                    }
                }
                if (flag)
                    break;
                if (SearchNode->scope == 0 && check == NULL)
                {
                    delete SearchNode;
                    throw Undeclared(WarningCode);
                }
            }
            delete SearchNode;
        }
    }
    return true;
}