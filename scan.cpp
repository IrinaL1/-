#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include <utility>
#include <vector>

using namespace std;
//проверка вхождения ...
bool in(vector<char> a, char b) 
{
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] == b)
            return true;
    }
    return false;
}

bool in(vector<string> a, string b)
{
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] == b)
            return true;
    }
    return false;
}

//функция ошибки
int err()
{
    cout << "Error";
    return -1;
}

//добавляем символ в буфер buff (для имен)
string add(string buf, char l)
{
    buf.push_back(l); 
    return buf;
}

//считываем и переводим в int > 0
int num_int_pos(int buf_int, char n)
{
    return (buf_int * 10 + n - '0');
}

//считываем и переводим в int < 0
int num_int_neg(int buf_int, char n)
{
    return (buf_int * 10 - n + '0');
}

//int to float
double itof(int buf_int) 
{
    return double(buf_int);
}

// добавляем токен числа в выходную последовательность в виде +n или -n, где n - само число
vector<string> push_n(vector<string> token, double buf_num){
    if (buf_num < 0) token.push_back("-" + to_string(buf_num));
    else token.push_back("+" + to_string(buf_num));
    return token;
}
vector<string> push_n(vector<string> token, int buf_num){
    if (buf_num < 0) token.push_back("-" + to_string(buf_num));
    else token.push_back("+" + to_string(buf_num));
    return token;
}

// добавляем токен имени переменной в выходную последовательность,(внутри проверяем спец. имен и имен переменных)
vector<string> push_w(vector<string> token, string buf, map<string, string> *name_table, map<string, string> special_names){
    if (special_names.count(buf) > 0) token.push_back(special_names[buf]);
    else if ((*name_table).count(buf) > 0) token.push_back((*name_table)[buf]);
    else{
        int count = (*name_table).size() + 1;
        (*name_table)[buf] = "0" + to_string(count);
        token.push_back((*name_table)[buf]);
    }
    return token;
}

// добавить символ {'(', ')', '*', '+', '=','''} в выходную последовательность
vector<string> push_s(vector<string> token, string c, map<string,string> simvol_token){
    token.push_back(simvol_token[c]);
    return token;
}

//добавить символ из буфера
vector<string> push_s_b(vector<string> token, string buf, map<string,string> simvol_token){
    token.push_back(simvol_token[buf]);
    return token;
}

//считываем и переводим в float > 0
double num_float_pos(double buf_float, char n, int dec_kol)  //dec_kol - количество знаков после запятой
{
    buf_float += double((n - '0'))/pow(10, dec_kol);
    return buf_float;
}

//считываем и переводим в float < 0
double num_float_neg(double buf_float, char n, int dec_kol)
{
    buf_float -= double((n - '0'))/pow(10, dec_kol);
    return buf_float; 
}

int main()
{
    string s, condition = "start", buf = "", id_name;
    vector<string> token;
    //таблица имя переменной-токен
    map<string, string> name_table;
    //таблица число-токен
    map<string, float> number_table;
    int i = 0, count_name = 1, count_number = 1, buf_int = 0, dec_kol = 1;
    double buf_float = 0;
    map<string,string> simvol_token = { {"(", "s1"},
                                        {")", "s2"},
                                        {"+", "s3"},
                                        {"*", "s4"},
                                        {"=", "s5"},
                                        {"-", "s6"},
                                        {">", "s7"},
                                        {"<", "s8"},
                                        {"/", "s9"},
                                        {">=", "s10"},
                                        {"<=", "s11"},
                                        {"/=", "s12"},
                                        {"\'", "s13"}
   };
    //таблица спец имён-токен
    map<string, string> special_names = {{"defun","11"} /*токен = 1*/,
                                         {"print","12"} /*токен = 2*/,
                                         {"setq","13"} /*токен = 3*/,
                                         {"cond","14"} /*токен 4*/,
                                         {"car","15"} /*токен 5*/,
                                         {"cdr","16"} /*токен 6*/,
                                         {"t","17"} /*токен = 7*/,
                                         {"nil","18"} /*токен = 8*/};

//таблица переходов конечного автомата
    map<string, map<char, vector<string>>> transition_table = 
    {
    // состояния    {символ пришедший,{функция которую выполняем, в какое состояние переходим, - если надо сдвинуться на один назад, иначе 0}}
        {"start",   {{'l',{"add","name","0"}}, {'n',{"num_int_pos","int","0"}},      {')',{"push_s","start","0"}}, {' ',{"0","start","0"}},      {'\n',{"0","start","0"}},              {';',{"0","coment","0"}},           {'.',{"err","-1","0"}},      {'-',{"add", "minus", "0"}}, {'/',{"add","/","0"}}, {'>',{"add",">","0"}},     {'<',{"add","<","0"}},      {'=',{"push_s","start","0"}}, {'+',{"push_s","start","0"}}, {'*',{"push_s","start","0"}}, {'(',{"push_s","start","0"}}, {'\'',{"push_s", "start","0"}}}},
        {"name",    {{'l',{"add","name","0"}}, {'n',{"add","name","0"}},             {')',{"push_w","start","-"}}, {' ',{"push_w","start","0"}}, {'\n',{"push_w", "start", "0"}},       {';',{"push_w", "coment", "0"}},     {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"int",     {{'l',{"err","-1","0"}},   {'n',{"num_int_pos","int","0"}},      {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "coment", "0"}},     {'.',{"itof","float","0"}},  {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"-int",    {{'l',{"err","-1","0"}},   {'n',{"num_int_neg","-int","0"}},     {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "coment", "0"}},     {'.',{"itof","-float","0"}}, {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"float",   {{'l',{"err","-1","0"}},   {'n',{"num_float_pos","float","0"}},  {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "coment", "0"}},     {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"-float",  {{'l',{"err","-1","0"}},   {'n',{"num_float_neg","-float","0"}}, {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "coment", "0"}},     {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"/",       {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "coment", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"add", "/=", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"<",       {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "coment", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"add", "<=", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {">",       {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "coment", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"add", ">=", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"minus",   {{'l',{"err","-1","0"}},   {'n',{"num_int_neg","-int","0"}},     {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "coment", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"/=",      {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "coment", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {">=",      {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "coment", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"<=",      {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "coment", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}},     {'\'',{"err", "-1","0"}}}},
        {"coment",  {{'l',{"0","coment","0"}}, {'n',{"0","coment","0"}},             {')',{"0","coment","0"}}, {' ',{"0","coment","0"}},             {'\n',{"0", "start", "0"}},        {';',{"0", "coment", "0"}},         {'.',{"0","coment","0"}},    {'-',{"0", "coment", "0"}},{'/',{"0", "coment", "0"}},{'>',{"0", "coment", "0"}},{'<',{"0", "coment", "0"}}, {'=',{"0", "coment", "0"}}, {'+',{"0", "coment", "0"}},   {'*',{"0", "coment", "0"}},   {'(',{"0", "coment", "0"}},   {'\'',{"0", "coment","0"}}}}
    };
    bool flag = true;
    ifstream file("input.txt");
    if (!file)
    {
        cout << "Ошибка: файл не открылся\n";
        return -1;
    }
    else
    {
        char c, c_in;
        vector <string> buffer;
        while (!file.eof()){
            if (flag){
                c = file.get();
                c_in = c;
            }
            if(isalpha(c)) c = 'l';
            if(isdigit(c)) c = 'n';
            buffer = transition_table[condition][c];
            condition = buffer[1];
            if (buffer[0] == "add"){
                buf = add(buf, c_in);
            }
            else if (buffer[0] == "push_w"){
                token = push_w(token, buf, &name_table, special_names);
                buf = "";
            }
            else if (buffer[0] == "num_int_pos"){
                buf_int = num_int_pos(buf_int, c_in);
            }
            else if (buffer[0] == "num_int_neg"){
                buf_int = num_int_neg(buf_int, c_in);
            }
            else if (buffer[0] == "num_float_pos"){
                buf_float = num_float_pos(buf_float, c_in, dec_kol);
                dec_kol++;
            }
            else if (buffer[0] == "num_float_neg"){
                buf_float = num_float_neg(buf_float, c_in, dec_kol);
                dec_kol++;
            }
            else if (buffer[0] == "itof"){
                buf_float = itof(buf_int);
            }
            else if (buffer[0] == "push_n"){
                if (buf_float == 0) token = push_n(token, buf_int);
                else token = push_n(token, buf_float);
                buf_int = 0;
                buf_float = 0;
                dec_kol = 1;
                buf = "";
            }
            else if (buffer[0] == "push_s"){
                string s_in;
                s_in += c_in;
                token = push_s(token, s_in, simvol_token);
                buf = "";
            }
            else if (buffer[0] == "push_s_b"){
                token = push_s_b(token, buf, simvol_token);
                buf = "";
            }
            else if (buffer[0] == "err"){
                err();
                return -1;
            }
            if (buffer[2] == "-") {
                flag = false;
                continue;
            }
            flag = true;
        }
    }
    file.close();

    cout << token.size() << '\n'
         << '\n';
    // Вывод самих токенов
    for (i = 0; i < token.size(); i++)
        cout << token[i] << ' ';
    cout << endl;
    cout << '\n' << "Токены имён переменных" << endl;
    map<string, string>::iterator it2 = name_table.begin();
    for (i = 0; it2 != name_table.end(); it2++, i++)
    { // выводим их
        cout << i << ") Токен " << it2->second << ", значение " << it2->first << endl;
    }
    cout << '\n' << "Токены спец слов" << endl;
    it2 = special_names.begin();
    for (i = 0; it2 != special_names.end(); it2++, i++)
    { // выводим их
        cout << i << ") Токен " << it2->second << ", значение " << it2->first << endl;
    }
    cout << '\n' << "Токены символов" << endl;
    it2 = simvol_token.begin();
    for (i = 0; it2 != simvol_token.end(); it2++, i++)
    { // выводим их
        cout << i << ") Токен " << it2->second << ", значение " << it2->first << endl;
    }


    vector <string> stek = {"#"};
    token.push_back("#");
    flag = true;
    i = 0;
    vector<string> stek_condition = {""};
    int size = 0, j = 0;
    cout << stek[stek.size()-1];
    while (flag){
        size = stek.size();
/*1*/   if (stek[size-1] == "OP" && token[i] == "#"){
            stek.pop_back();
            stek.push_back("PROG");
            continue;
        }
/*1.1*/ else if (size >= 2 && stek[size-1] == "PROG" && stek[size-2] == "OP" && token[i] == "#"){
            for(j = 0; j < 2; j++) stek.pop_back();
            stek.push_back("PROG");
            continue;
        }
/*2*/   else if(size >= 4 && stek[size-1] == "s2" && stek[size-2] == "OP" && stek[size-3] == "12" && stek[size-4]=="s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 4; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*3*/   else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "OP" && stek[size-3][0] == '0' && stek[size-4]=="13" && stek[size-5]=="s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*4*/   else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "F" && stek[size-3][0] == '0' && stek[size-4]=="13" && stek[size-5]=="s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*5*/   else if(size >= 6 && stek[size-1] == "s2" && stek[size-2] == "OP" && stek[size-3] == "A" && stek[size-4][0] == '0' && stek[size-5]=="11" && stek[size-6]=="s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 6; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*6*/   else if(size >= 4 && stek[size-1] == "s2" && stek[size-2] == "MOP" && stek[size-3] == "14" && stek[size-4] == "s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 4; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*7*/   else if(size >= 4 && stek[size-1] == "s2" && stek[size-2] == "OP" && stek[size-3] == "15" && stek[size-4]=="s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 4; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*8*/   else if(size >= 4 && stek[size-1] == "s2" && stek[size-2] == "OP" && stek[size-3] == "16" && stek[size-4]=="s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 4; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*9*/   else if(size >= 2 && stek[size-1] == "A" && stek[size-2] == "s13" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 2; j++) stek.pop_back();
            stek.push_back("OP");
            stek_condition.pop_back();
            continue;
        }
/*10*/  else if(size >= 2 && stek[size-1] == "OP" && stek[size-2] == "M" && in({"s2"}, token[i])){
            for(j = 0; j < 2; j++) stek.pop_back();
            stek.push_back("OP");
            continue;
        }
/*11*/  else if(size >= 4 && stek[size-1] == "s2" && stek[size-2] == "OP" && stek[size-3] == "s1" && stek[size-4] == "MOP" && in({"s1", "s2"}, token[i])){
            for(j = 0; j < 4; j++) stek.pop_back();
            stek.push_back("MOP");
            continue;
        }
/*12*/  else if(size >= 3 && stek[size-1] == "s2" && stek[size-2] == "OP" && stek[size-3] == "s1" && in({"s1", "s2"}, token[i])){
            for(j = 0; j < 3; j++) stek.pop_back();
            stek.push_back("MOP");
            continue;
        }
/*13+*/  else if(size >= 1 && stek[size-1] == "M" && (in({"s2","#"}, token[i]) || stek_condition[stek_condition.size()-1] == "")){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("OP");
            continue;
        }
/*14*/  else if(size >= 1 && stek[size-1] == "17"){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("M");
            continue;
        }
/*15*/  else if(size >= 1 && stek[size-1] == "18"){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("M");
            continue;
        }
/*16*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "E" && stek[size-3] == "M" && stek[size-4] == "s8" && stek[size-5] == "s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("M");
            continue;
        }
/*17*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "E" && stek[size-3] == "M" && stek[size-4] == "s5" && stek[size-5] == "s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("M");
            stek_condition.pop_back();
            continue;
        }
/*18*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "E" && stek[size-3] == "M" && stek[size-4] == "s7" && stek[size-5] == "s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("M");
            stek_condition.pop_back();
            continue;
        }
/*19*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "E" && stek[size-3] == "M" && stek[size-4] == "s11" && stek[size-5] == "s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("M");
            stek_condition.pop_back();
            continue;
        }
/*20*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "E" && stek[size-3] == "M" && stek[size-4] == "s10" && stek[size-5] == "s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("M");
            stek_condition.pop_back();
            continue;
        }
/*21*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "E" && stek[size-3] == "M" && stek[size-4] == "s12" && stek[size-5] == "s1" && in({"#","s1","s2"}, token[i])){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("M");
            stek_condition.pop_back();
            continue;
        }
/*22*/  else if(size >= 1 && stek[size-1] == "E" && ((in({"s5", "s12", "s8", "s11", "s7", "s10"}, stek_condition[stek_condition.size() - 1]) && token[i] != "s2")|| in({"","11","12","13","14","15","16"},stek_condition[stek_condition.size() - 1]))){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("M");
            continue;
        }
/*23*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "T" && stek[size-3] == "E" && stek[size-4] == "s3" && stek[size-5] == "s1" && (in({"#","s1","s2"}, token[i]) || in({'0','-','+'},token[i][0]))){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("E");
            stek_condition.pop_back();
            continue;
        }
/*24*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "T" && stek[size-3] == "E" && stek[size-4] == "s6" && stek[size-5] == "s1" && (in({"#","s1","s2"}, token[i]) || in({'0','-','+'},token[i][0]))){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("E");
            stek_condition.pop_back();
            continue;
        }
/*25*/  else if(size >= 1 && stek[size-1] == "T" && (in({"s5", "s12", "s7", "s8", "s10", "s11","","11","12","13","14","15","16"}, stek_condition[stek_condition.size() - 1]) || (in({"s3","s6"},stek_condition[stek_condition.size() - 1]) && token[i] != "s2"))){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("E");
            continue;
        }
/*26*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "F" && stek[size-3] == "T" && stek[size-4] == "s4" && stek[size-5] == "s1" && (in({"#", "s1", "s2"}, token[i]) || in({'0', '-', '+'}, token[i][0]))){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("T");
            stek_condition.pop_back();
            continue;
        }
/*27*/  else if(size >= 5 && stek[size-1] == "s2" && stek[size-2] == "F" && stek[size-3] == "T" && stek[size-4] == "s9" && stek[size-5] == "s1" && (in({"#", "s1", "s2"}, token[i]) || in({'0', '-', '+'}, token[i][0]))){
            for(j = 0; j < 5; j++) stek.pop_back();
            stek.push_back("T");
            stek_condition.pop_back();
            continue; 
        }
/*28*/  else if(size >= 1 && stek[size-1] == "F" && (in({"s3", "s6", "s5", "s12", "s7", "s8", "s10", "s11","","12","13","14","15","16"}, stek_condition[stek_condition.size() - 1]) || (in({"s4","s9"},stek_condition[stek_condition.size() - 1]) && token[i] != "s2"))){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("T");
            continue;
        }
/*29*/  else if(size >= 1 && in({'0','-','+'}, stek[size-1][0]) && (!(in({"11", "13"}, stek_condition[stek_condition.size() - 1])) || (in({"13"}, stek_condition[stek_condition.size() - 1]) && token[i] == "s2") || (in({"11"}, stek_condition[stek_condition.size() - 1]) && token[i] != "s1"))){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("F");
            continue;
        }
/*30*/  else if(size >= 3 && stek[size-1] == "s2" && stek[size-2] == "L" && stek[size-3] == "s1" && in({"#", "s1", "s2"}, token[i])){
            for(j = 0; j < 3; j++) stek.pop_back();
            stek.push_back("A");
            continue;
        }  
/*31*/  else if(size >= 1 && stek[size-1] == "F" && !(in({"s4","s9","s3", "s6", "s5", "s12", "s7", "s8", "s10", "s11"}, stek_condition[stek_condition.size() - 1]))){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("H");
            continue;
        }
/*32*/  else if(size >= 2 && stek[size-1] == "L" && stek[size-2] == "H"){
            for(j = 0; j < 2; j++) stek.pop_back();
            stek.push_back("L");
            continue;
        }
/*33*/  else if(size >= 1 && stek[size-1] == "H" && in({"s2"}, token[i])){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("L");
            continue;
        }
/*34*/  else if(size >= 4 && stek[size-1] == "s2" && stek[size-2] == "ARG" && stek[size-3] == "F" && stek[size-4] == "s1" && in({"#", "s1", "s2"}, token[i])){
            for(j = 0; j < 4; j++) stek.pop_back();
            stek.push_back("OP");
            continue;
        }
        /*
/*35  else if(size >= 2 && stek[size-1] == "OP" && stek[size-2] == "ARG"){
            for(j = 0; j < 2; j++) stek.pop_back();
            stek.push_back("ARG");
            continue;
        }
/*36  else if(size >= 1 && stek[size-1] == "OP"){
            for(j = 0; j < 1; j++) stek.pop_back();
            stek.push_back("ARG");
            continue;
        }
*/
/*в стэк*/
        else if(i < token.size()){
            stek.push_back(token[i]);
            if (in({"11","12","13","14","15","16","s4","s3","s6","s9","s12","s8","s11","s5","s7","s10","s13"},token[i])){
                stek_condition.push_back(token[i]);
            }
            i++;
            continue;
        }
        else{
            break;
        }
    }
if(stek.size() == 3 && stek[0] == "#" && stek[1] == "PROG" && stek[2] == "#"){
    cout << "Синтаксис программы верен";
}
else{
    cout << "Ошибка в синтаксисе программы";
    return -1;
}
return 0;
}
