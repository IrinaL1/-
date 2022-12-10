#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <vector>


using namespace std;

bool in(vector <char> a, char b){
    for(int i = 0; i < a.size(); i++){
        if (a[i] == b) return true;
    }
    return false; 
}

bool in(vector <string> a, string b){
    for(int i = 0; i < a.size(); i++){
        if (a[i] == b) return true;
    }
    return false; 
}

int main()
{
    string s, condition = "start", now = "", id_name;
    getline(cin, s);
    vector <string> token;                  
    map <string, string> name_table;
    map <string, float> number_table;
    int i = 0, count_name = 1, count_number = 1, buf_int = 0;
    float buf_float;
    vector <string> special_names = {"defun"/*токен = 3*/, "print"/*токен = 1*/, "setq"/*токен = 2*/, "cond"/*токен 7*/, "car"/*токен 5*/, "cdr"/*токен 6*/, "t"/*токен = Т*/, "nil"/*токен = 4*/};
    vector <string> conditions = {"name",
        "d", "de", "def", "defu", "defun",
        "T",
        "N", "NI", "NIL",
        "p", "pr", "pri", "prin", "print",
        "s", "se", "set", "setq" 
    };
    
    while (i != s.size() && s[i] != '\n'){
        if (s[i] == '(' && condition == "start"){
            token.push_back("(");
        }
        else if (s[i] == ')' && condition == "start"){
            token.push_back(")");
        }
        else if (s[i] == '+' && condition == "start"){
            token.push_back("+");
        }
        else if (s[i] == '\'' && condition == "start"){
            token.push_back("'");
        }
        else if (s[i] == '-' && condition == "start"){
            now = "-";
            condition = "-";
        }
        else if (in({' ', ')'}, s[i]) && condition == "-"){
            token.push_back("-");
            now = "";
            condition = "start";
        }
        else if (s[i] == '*' && condition == "start"){
            token.push_back("*");
        }
        else if (in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && condition == "start"){
            buf_int = buf_int * 10 + (int(s[i]) - int('0'));
            condition = "int";
        }
        else if (in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && condition == "int"){
            buf_int = buf_int * 10 + (int(s[i]) - int('0'));
        }
        else if (s[i] == '.'  && condition == "int"){
            buf_float = buf_int;
            buf_int = 10;
            condition = "float";
        }
        else if (in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && condition == "float"){
            buf_float += float(int(s[i]) - int('0'))/ buf_int;
            buf_int *= 10;
        }
        else if (in({' ', ')'}, s[i]) && condition == "int"){
            id_name = "-0" + to_string(count_number);
            count_number ++;
            token.push_back(id_name);
            condition = "start";
            number_table[id_name] = buf_int;
            buf_int = 0;
            i--;
        }
        else if (in({' ', ')'}, s[i]) && condition == "float"){
            id_name = "-0" + to_string(count_number);
            count_number ++;
            token.push_back(id_name);
            condition = "start";
            number_table[id_name] = buf_float;
            buf_int = 0;
            buf_float = 0;
            i--;
        }
        else if (in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && condition == "-"){
            buf_int = buf_int * 10 - (int(s[i]) - int('0'));
            condition = "-int";
        }
        else if (in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && condition == "-int"){
            buf_int = buf_int * 10 - (int(s[i]) - int('0'));
        }
        else if (s[i] == '.'  && condition == "-int"){
            buf_float = buf_int;
            buf_int = 10;
            condition = "-float";
        }
        else if (in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && condition == "-float"){
            buf_float -= float(int(s[i]) - int('0'))/ buf_int;
            buf_int *= 10;
        }
        else if (in({' ', ')'}, s[i]) && condition == "-int"){
            id_name = "-0" + to_string(count_number);
            count_number ++;
            token.push_back(id_name);
            condition = "start";
            number_table[id_name] = buf_int;
            buf_int = 0;
            i--;
        }
        else if (in({' ', ')'}, s[i]) && condition == "-float"){
            id_name = "-0" + to_string(count_number);
            count_number ++;
            token.push_back(id_name);
            condition = "start";
            number_table[id_name] = buf_float;
            buf_int = 0;
            buf_float = 0;
            i--;
        }
        else if (!in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && (condition == "int" || condition == "float")){
            cout << "error wrong name\n";
            return -1;
        }
        else if (!in({'0','1','2','3','4','5','6','7','8','9'}, s[i]) && (condition == "-int" || condition == "-float")){
            cout << "error wrong name\n";
            return -1;
        }
        else if (s[i] == 'p' && condition == "start"){
            now += "p";
            condition = "p";
        }
        else if (s[i] == 'r' && condition == "p"){
            now += "r";
            condition = "pr";
        }
        else if (s[i] == 'i' && condition == "pr"){
            now += "i";
            condition = "pri";
        }
        else if (s[i] == 'n' && condition == "pri"){
            now += "n";
            condition = "prin";
        }
        else if (s[i] == 't' && condition == "prin"){
            now += "t";
            condition = "print";
        }
        else if (s[i] == ' ' && condition == "print"){
            token.push_back("1");
            now = "";
            condition = "start";
        }
        else if (s[i] == 's' && condition == "start"){
            now += "s";
            condition = "s";
        }
        else if (s[i] == 'e' && condition == "s"){
            now += "e";
            condition = "se";
        }
        else if (s[i] == 't' && condition == "se"){
            now += "t";
            condition = "set";
        }
        else if (s[i] == 'q' && condition == "set"){
            now += "q";
            condition = "setq";
        }
        else if (s[i] == ' ' && condition == "setq"){
            token.push_back("2");
            now = "";
            condition = "start";
        }
        else if (s[i] == 'd' && condition == "start"){
            now += "d";
            condition = "d";
        }
        else if (s[i] == 'e' && condition == "d"){
            now += "e";
            condition = "de";
        }
        else if (s[i] == 'f' && condition == "de"){
            now += 'f';
            condition = "def";
        }
        else if (s[i] == 'u' && condition == "def"){
            now += 'u';
            condition = "defu";
        }
        else if (s[i] == 'n' && condition == "defu"){
            now += 'n';
            condition = "defun";
        }
        else if (s[i] == ' ' && condition == "defun"){
            token.push_back("3");
            now = "";
            condition = "start";
        }
        else if (s[i] == 'T' && condition == "start"){
            now += 'T';
            condition = "T";
        }
        else if (in({' ', ')'}, s[i]) && condition == "T"){
            token.push_back("T");
            now = "";
            condition = "start";
        }
        else if (s[i] == 'N' && condition == "start"){
            now += 'N';
            condition = "N";
        }
        else if (s[i] == 'I' && condition == "N"){
            now += 'I';
            condition = "NI";
        }
        else if (s[i] == 'L' && condition == "NI"){
            now += 'L';
            condition = "NIL";
        }
        else if (in({' ', ')'}, s[i]) && condition == "NIL"){
            token.push_back("4");
            now = "";
            condition = "start";
        }
        else if (s[i] == 'c' && condition == "start"){
            now += 'c';
            condition = "c";
        }
        else if (s[i] == 'a' && condition == "c"){
            now += 'a';
            condition = "ca";
        }
        else if (s[i] == 'r' && condition == "ca"){
            now += 'r';
            condition = "car";
        }
        else if (in({' '}, s[i]) && condition == "car"){
            token.push_back("5");
            now = "";
            condition = "start";
        }
        else if (s[i] == 'd' && condition == "c"){
            now += 'd';
            condition = "cd";
        }
        else if (s[i] == 'r' && condition == "cd"){
            now += 'r';
            condition = "cdr";
        }
        else if (in({' '}, s[i]) && condition == "cdr"){
            token.push_back("6");
            now = "";
            condition = "start";
        }
        else if (s[i] == 'o' && condition == "c"){
            now += 'o';
            condition = "co";
        }
        else if (s[i] == 'n' && condition == "co"){
            now += 'n';
            condition = "con";
        }
        else if (s[i] == 'd' && condition == "con"){
            now += 'd';
            condition = "cond";
        }
        else if (in({' ', '('}, s[i]) && condition == "cond"){
            token.push_back("7");
            now = "";
            condition = "start";
        }
        //Новые разборы спец слов вводить после этой строки, токены заняты от 1 до 4
        else if (in({' ', ')'}, s[i]) && in(conditions, condition)){
            if (name_table.count(now) > 0){
                token.push_back(name_table[now]);
                condition = "start";
                now = "";
            }
            else{
                id_name = "0" + to_string(count_name);
                count_name ++;
                token.push_back(id_name);
                condition = "start";
                name_table[now] = id_name;
                now = "";
            }
            i--;
        }
        else if (!in({' ','0','1','2','3','4','5','6','7','8','9'}, s[i]) && condition == "start"){
            now += s[i];
            condition = "name";
        }
        else if (condition == "name" or in(conditions, condition)){
            now += s[i];
            condition = "name";
        }
        i++;
    }
    //Вывод количества токенов
    cout << token.size() << '\n' << '\n';
    //Вывод самих токенов
    for(i = 0; i < token.size(); i++) cout << token[i] << ' ';
    cout << endl;
    //Вывод чисел в number_table
    map <string, float> :: iterator it = number_table.begin();
    for (i = 0; it != number_table.end(); it++, i++) {  // выводим их
        cout << i << ") Ключ " << it->first << ", значение " << it->second << endl;
    }
    return 0;
}
