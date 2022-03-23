#include "SymbolTable.h"
//hàm tách chuỗi
void removeSpace(string str, string strM[])
{
    string word = "";
    int i = 0;
    for (auto x : str)
    {
        if (x == ' ' && i < 2) // mang nay chi co 3 phan tu, neu vuot qua k tach nua
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
}
/*thay đổi cấu trúc: sẽ là một danh sách trải dài, hiện thực như ngăn xếp, khi mở scope sẽ
chỉ tăng biến status, nhưng đồng thời node sẽ có 1 giá trị bool để thực hiện chức năng ẩn
hiện giá trị, Node scope sẽ dùng để lưu địa chỉ của biến bị ẩn do trùng, khi đóng scope
sẽ thực hiện 2 việc:
1. xóa node trong vùng của scope đó
2. hiện lại giá trị của node đã bị ẩn do trùng
3. trả biến lưu địa chỉ để dò về null (biến này giờ đây chỉ cần 1)
Bỏ bớt flag và mảng trong Symboltable*/
void SymbolTable::run(string filename)
{
    ifstream infile(filename);
    string line;
    while (getline(infile, line))
    {
        string getWord[3] = {""};
        removeSpace(line, getWord);
        if (getWord[0] == "INSERT" && getWord[1] != "" && getWord[2] != "")
            this->insert(getWord[1], getWord[2]);
        else if (getWord[0] == "BEGIN" && getWord[1] == "" && getWord[2] == "")
            this->begin();
        else if (getWord[0] == "END" && getWord[1] == "" && getWord[2] == "")
            this->end();
        else if (getWord[0] == "PRINT" && getWord[1] == "" && getWord[2] == "")
            this->print();
        else if (getWord[0] == "RPRINT" && getWord[1] == "" && getWord[2] == "")
            this->rprint();
        else if (getWord[0] == "LOOKUP" && getWord[1] != ""  && getWord[2] == "")
            this->lookup(getWord[1]);
        else if (getWord[0] == "ASSIGN" && getWord[1] != "" && getWord[2] != "")
            this->assign(getWord[1], getWord[2]);
        else
            throw InvalidInstruction(line);
    }
    if (this->ScopeStatus != 0)
        throw UnclosedBlock(this->ScopeStatus);
    infile.close();
}
// Hiện thực vào ngày thứ 5: insert, begin, end - 3 hàm này không được phép sai
/* [head] > [ 2 ] > [ 1 ], StateScope sẽ được gán giá trị khi vào scope, nó là
vùng phân định để kiểm soát lỗi redeclare và ẩn hiện node bên ngoài*/
void SymbolTable::insert(string name, string type)
{
    //Kiểm tra regex
    if (!regex_match(name, regex("[a-z][A-Za-z0-9_]*")))
        throw InvalidInstruction("INSERT " + name + " " + type);
    if (type != "string" && type != "number")
        throw InvalidInstruction("INSERT " + name + " " + type);
    Node *addNode = new Node;
    addNode->name = name;
    addNode->type = type;
    addNode->scope = this->ScopeStatus;
    //Logic
    if (this->head == NULL) //NULL case
    {
        this->head = addNode;
        cout << "success" << '\n';
        return;
    }
    Node *checkR = NULL;
    if (this->StateScope[this->ScopeStatus] != NULL) // ẩn node bị ghi đè
    {
        //kiểm tra lỗi redeclare trước
        checkR = this->head;
        while (checkR != this->StateScope[this->ScopeStatus])
        {
            if (checkR->name == addNode->name)
            {
                delete addNode;
                throw Redeclared("INSERT " + name + " " + type);
            }
            checkR = checkR->next;
        }
        // ẩn giá trị bị trùng
        checkR = this->StateScope[this->ScopeStatus];
        while (checkR != NULL)
        {
            if (checkR->name == addNode->name)
            {
                checkR->hide = true;
                addNode->hideNode = checkR;
                break;
            }
            checkR = checkR->next;
        }
    }
    else if (this->StateScope[this->ScopeStatus] == NULL) // chỉ cần kiểm tra redeclare
    {
        checkR = this->head;
        while (checkR != NULL)
        {
            if (checkR->name == addNode->name)
            {
                delete addNode;
                throw Redeclared("INSERT " + name + " " + type);
            }
            checkR = checkR->next;
        }
    }
    // tất cả đã an toàn, chỉ cần insert
    this->head->prev = addNode;
    addNode->next = this->head;
    this->head = addNode;
    cout << "success" << '\n';
    return;
}
/* cập nhật giá trị cho state và status scope */
void SymbolTable::begin()
{
    if (this->ScopeStatus >= this->capacity - 1)
        this->extend();
    this->ScopeStatus++;
    this->StateScope[this->ScopeStatus] = this->head;
}
/* kiểm tra tình trạng scope, xóa tất cả node trong scope, trả trạng thái
cho node bị ẩn, cập nhật giá trị cho state, status, head */
void SymbolTable::end()
{
    if (this->ScopeStatus == 0)
        throw UnknownBlock();
    Node *delNode = this->head;
    Node *keep = this->head;
    while (delNode != NULL)
    {
        if (delNode->scope != this->ScopeStatus)
            break;
        if (delNode->hideNode != NULL)
        {
            delNode->hideNode->hide = false;
            delNode->hideNode = NULL;
        }
        keep = delNode->next;
        delete delNode;
        delNode = keep;
    }
    this->head = keep;
    if (this->head != NULL)
        this->head->prev = NULL;
    this->StateScope[this->ScopeStatus] = NULL;
    this->ScopeStatus--;
}
// hàm test
void SymbolTable::check()
{
    if (this->head == NULL)
        return;
    Node *travel = this->head;
    while (travel != NULL)
    {
        if (!travel->hide)
            cout << "name: " << travel->name << "//scope: " << travel->scope << " ";
        travel = travel->next;
    }
    cout << '\n';
    travel = this->head;
    while (travel->next != NULL)
    {
        travel = travel->next;
    }
    while (travel != NULL)
    {
        cout << "name: " << travel->name << "//scope: " << travel->scope << " ";
        travel = travel->prev;
    }
    cout << '\n';
}
// thứ 6: hiện thực look up, print, rprint
void SymbolTable::lookup(string name)
{
    if (this->head == NULL)
        throw Undeclared("LOOKUP " + name);
    if (!regex_match(name, regex("[a-z][A-Za-z0-9_]*")))
        throw InvalidInstruction("LOOKUP " + name);
    Node *searchNode = this->head;
    while (searchNode != NULL)
    {
        if (searchNode->name == name && !searchNode->hide)
        {
            cout << to_string(searchNode->scope) << '\n';
            return;
        }
        searchNode = searchNode->next;
    }
    if (searchNode == NULL)
        throw Undeclared("LOOKUP " + name);
}
void SymbolTable::print()
{
    if (this->head == NULL)
        return;
    Node *printNode = this->head;
    string *ss = new string("");
    while (printNode != NULL)
    {
        if (!printNode->hide)
        {
            if (*ss == "")
                *ss = printNode->name + "//" + to_string(printNode->scope) + *ss;
            else
                *ss = printNode->name + "//" + to_string(printNode->scope) + " " + *ss;
        }
        printNode = printNode->next;
    }
    if (*ss != "")
        cout << *ss << '\n';
    delete ss;
}
void SymbolTable::rprint()
{
    if (this->head == NULL)
        return;
    Node *printNode = this->head;
    string *ss = new string("");
    while (printNode != NULL)
    {
        if (!printNode->hide)
        {
            if (*ss == "")
                *ss = *ss + printNode->name + "//" + to_string(printNode->scope);
            else
                *ss = *ss + " " + printNode->name + "//" + to_string(printNode->scope);
        }
        printNode = printNode->next;
    }
    if (*ss != "")
        cout << *ss << '\n';
    delete ss;
}
// hiện thực hàm hủy và assgin
void SymbolTable::detruction()
{
    if (this->head == NULL)
        return;
    for (int i = 0; i <= this->ScopeStatus; i++)
    {
        this->StateScope[i] = NULL;
    }
    delete this->StateScope;
    Node *delNode = this->head;
    while (delNode->next != NULL)
    {
        delNode = delNode->next;
        delete delNode->prev;
    }
    delete delNode;
    this->head = NULL;
    this->ScopeStatus = 0;
}
//assign
void SymbolTable::assign(string name, string data)
{
    if (!regex_match(data, regex("([0-9]+)|(['][A-Za-z0-9 ]*['])|([a-z][A-Za-z0-9_]*)")))
        throw InvalidInstruction("ASSIGN " + name + " " + data);
    if (!regex_match(name, regex("[a-z][A-Za-z0-9_]*")))
        throw InvalidInstruction("ASSIGN " + name + " " + data);
    //logic
    if (this->head == NULL)
        throw Undeclared("ASSIGN " + name + " " + data);
    Node *assignData = this->head;
    while (assignData != NULL)
    {
        if (!assignData->hide)
        {
            if (assignData->name == name)
            {
                //new case here
                if (regex_match(data, regex("[a-z][A-Za-z0-9_]*")))
                {
                    //data as name
                    Node *copyNode = this->head;
                    while (copyNode != NULL)
                    {
                        if (!copyNode->hide)
                        {
                            if (copyNode->name == data)
                            {
                                if (copyNode->type == assignData->type)
                                {
                                    assignData->data = copyNode->data;
                                    cout << "success" << '\n';
                                    return;
                                }
                                else
                                    throw TypeMismatch("ASSIGN " + name + " " + data);
                            }
                        }
                        copyNode = copyNode->next;
                    }
                    throw Undeclared("ASSIGN " + name + " " + data);
                }
                else if (regex_match(data, regex("[0-9]+")))
                {
                    //number case
                    if (assignData->type != "number")
                        throw TypeMismatch("ASSIGN " + name + " " + data);
                    assignData->data = data;
                    cout << "success" << '\n';
                    return;
                }
                else if (regex_match(data, regex("['][A-Za-z0-9 ]*[']")))//string case
                {
                    if (assignData->type != "string")
                        throw TypeMismatch("ASSIGN " + name + " " + data);
                    assignData->data = data;
                    cout << "success" << '\n';
                    return;
                }
            }
        }
        assignData = assignData->next;
    }
    throw Undeclared("ASSIGN " + name + " " + data);
}
void SymbolTable::extend()
{
    this->capacity = this->capacity * 2;
    Node **Chace = new Node*[this->capacity];
    for (int i = 0; i <= this->ScopeStatus;i++)
    {
        Chace[i] = this->StateScope[i];
    }
    for (int i =this->ScopeStatus + 1; i < this->capacity;i++)
    {
        Chace[i] = NULL;
    }
    delete this->StateScope;
    this->StateScope = Chace;
}