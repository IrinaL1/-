#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include <utility>
#include <vector>

using namespace std;
//проверка вхождения ...?
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

//считываем и переводим в float > 0
//!!!! написать итератор для dec_col в switch
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
    string s, condition = "start", now = "", id_name;
    vector<string> token;
    //таблица имя переменной-токен
    map<string, string> name_table;
    //таблица число-токен
    map<string, float> number_table;
    int i = 0, count_name = 1, count_number = 1, buf_int = 0, dec_kol = 1;
    float buf_float;
    //таблица спец имён-токен
    map<string, string> special_names = {{"defun","13"} /*токен = 3*/,
                                        {"print","11"} /*токен = 1*/,
                                         {"setq","12"} /*токен = 2*/,
                                         {"cond","17"} /*токен 7*/,
                                         {"car","15"} /*токен 5*/,
                                         {"cdr","16"} /*токен 6*/,
                                         {"t","18"} /*токен = 8*/,
                                         {"nil","14"} /*токен = 4*/};

//таблица переходов конечного автомата
    map<string, map<char, vector<string>>> transition_table = 
    {
    // состояния    {символ пришедший,{функция которую выполняем, в какое состояние переходим, - если надо сдвинуться на один назад, иначе 0}}
        {"start",   {{'l',{"add","name","0"}}, {'n',{"num_int_pos","int","0"}},      {')',{"push_s","start","0"}}, {' ',{"0","start","0"}},      {'\n',{"0","start","0"}},              {';',{"0","coment","0"}},           {'.',{"err","-1","0"}},      {'-',{"add", "minus", "0"}}, {'/',{"add","/","0"}}, {'>',{"add",">","0"}},     {'<',{"add","<","0"}},      {'=',{"push_s","start","0"}}, {'+',{"push_s","start","0"}}, {'*',{"push_s","start","0"}}, {'(',{"push_s","start","0"}}}},
        {"name",    {{'l',{"add","name","0"}}, {'n',{"add","name","0"}},             {')',{"push_w","start","-"}}, {' ',{"push_w","start","0"}}, {'\n',{"push_w", "start", "0"}},       {';',{"push_w", "start", "0"}},     {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"int",     {{'l',{"err","-1","0"}},   {'n',{"num_int_pos","int","0"}},      {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "start", "0"}},     {'.',{"itof","float","0"}},  {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"-int",    {{'l',{"err","-1","0"}},   {'n',{"num_int_neg","-int","0"}},     {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "start", "0"}},     {'.',{"itof","-float","0"}}, {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"float",   {{'l',{"err","-1","0"}},   {'n',{"num_float_pos","float","0"}},  {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "start", "0"}},     {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"-float",  {{'l',{"err","-1","0"}},   {'n',{"num_float_neg","-float","0"}}, {')',{"push_n","start","-"}}, {' ',{"push_n","start","0"}}, {'\n',{"push_n", "start", "0"}},       {';',{"push_n", "start", "0"}},     {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"/",       {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "start", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"add", "/=", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"<",       {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "start", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"add", "<=", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {">",       {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "start", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"add", ">=", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"minus",   {{'l',{"err","-1","0"}},   {'n',{"num_int_neg","-int","0"}},     {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "start", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"/=",      {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "start", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {">=",      {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "start", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"<=",      {{'l',{"err","-1","0"}},   {'n',{"err","-1","0"}},               {')',{"push_s_b","start","-"}}, {' ',{"push_s_b","start","0"}}, {'\n',{"push_s_b", "start", "0"}}, {';',{"push_s_b", "start", "0"}},   {'.',{"err","-1","0"}},      {'-',{"err", "-1", "0"}},  {'/',{"err", "-1", "0"}},{'>',{"err", "-1", "0"}},  {'<',{"err", "-1", "0"}},   {'=',{"err", "-1", "0"}},     {'+',{"err", "-1", "0"}},     {'*',{"err", "-1", "0"}},     {'(',{"err", "-1", "0"}}}},
        {"coment",  {{'l',{"0","coment","0"}}, {'n',{"0","coment","0"}},             {')',{"0","coment","0"}}, {' ',{"0","coment","0"}},             {'\n',{"0", "start", "0"}},        {';',{"0", "coment", "0"}},         {'.',{"0","coment","0"}},    {'-',{"0", "coment", "0"}},{'/',{"0", "coment", "0"}},{'>',{"0", "coment", "0"}},{'<',{"0", "coment", "0"}}, {'=',{"0", "coment", "0"}}, {'+',{"0", "coment", "0"}},   {'*',{"0", "coment", "0"}},   {'(',{"0", "coment", "0"}}}}
    };
    ifstream file("input.txt");
    if (!file)
    {
        cout << "Ошибка: файл не открылся\n";
        return -1;
    }
    else
    {
        char c;
        vector <string> buffer;
        while (!file.eof()){
            c = file.get();
            if(isalpha(c)) c = 'l';
            if(isdigit(c)) c = 'n';
            buffer = transition_table[condition][c];
            
        }
    }
    file.close();
    cout << num_float_neg(double(-12), '2', dec_kol) << '\n';
    cout << double(12) << '\n';

}
