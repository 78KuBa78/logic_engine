#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <set>
#include <unordered_map>
#include <stack>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <algorithm> // для std::transform

using namespace std;

// Общая доска — хранит состояние всей обработки
struct Blackboard {
    string rawExpression;                    // Исходное выражение
    vector<string> tokens;                   // Токены выражения
    set<string> variables;                   // Переменные из выражения
    unordered_map<string, string> values;   // Значения переменных
    unordered_map<string, string> params;    // параметры из файла
    vector<string> rpn;                      // Выражение в обратной польской нотации
    bool resultComputed = false;             // Флаг, что результат получен
    bool result = false;                     // Итоговый результат
};

// Утилиты
bool isOperator(const string& token) {
    static const set<string> ops = { "AND", "OR", ">", "<", "==", "!=", ">=", "<=",
                                    "and", "or" };
    return ops.count(token) > 0;
}

bool isParen(const string& token) {
    return token == "(" || token == ")";
}

int getPrecedence(const string& op) {
    if (op == "OR" || op == "or") return 1;
    if (op == "AND" || op == "and") return 2;
    if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=") return 3;
    return 0;
}

// Агент 1: Поиск переменных
struct VariableExtractor {
    void execute(Blackboard& bb) {
        regex re(R"(\b[A-Za-z_][A-Za-z0-9_]*\b)");
        sregex_iterator iter(bb.rawExpression.begin(), bb.rawExpression.end(), re), end;
        bb.variables.clear();

        while (iter != end) {
            string var = iter->str();
            if (!isOperator(var) && !isParen(var)) {
                bb.variables.insert(var);
            }
            ++iter;
        }
    }
};

// Агент 2: Запрос значений переменных
struct ValuePrompter {
    void execute(Blackboard& bb) {
        for (const auto& var : bb.variables) {
            if (bb.values.find(var) == bb.values.end()) {
                cout << "Введите значение для " << var << ": ";
                string val;
                cin >> val;
                bb.values[var] = val;
            }
        }
    }
};

// Агент 3: Замена переменных на значения
struct VariableReplacer {
    void execute(Blackboard& bb) {
        // Используем regex для замены
        regex re(R"(\b[A-Za-z_][A-Za-z0-9_]*\b)");
        string result;
        size_t lastPos = 0;

        auto& input = bb.rawExpression;
        auto iter = sregex_iterator(input.begin(), input.end(), re);
        auto end = sregex_iterator();

        result.clear();

        while (iter != end) {
            size_t start = iter->position();
            size_t len = iter->length();
            string var = iter->str();

            result += input.substr(lastPos, start - lastPos);
            if (bb.values.count(var)) {
                result += bb.values.at(var);
            }
            else {
                result += var;
            }
            lastPos = start + len;
            ++iter;
        }
        result += input.substr(lastPos);
        bb.rawExpression = result;  // обновляем rawExpression с подставленными значениями
    }
};

// Агент 4: Токенизация
struct Tokenizer {
    vector<string> tokenize(const string& expr) {
        vector<string> tokens;
        size_t i = 0;
        while (i < expr.length()) {
            if (isspace(expr[i])) {
                ++i;
                continue;
            }

            // Обработка отрицательных чисел
            if ((isdigit(expr[i]) ||
                (expr[i] == '-' &&
                    (i == 0 || expr[i - 1] == '(' || isOperator(tokens.empty() ? "" : tokens.back())) &&
                    i + 1 < expr.length() && isdigit(expr[i + 1])))) {

                string num;
                if (expr[i] == '-') {
                    num += expr[i++];
                }
                while (i < expr.length() && isdigit(expr[i])) {
                    num += expr[i++];
                }
                tokens.push_back(num);
            }
            // Идентификаторы и логические операторы
            else if (isalpha(expr[i])) {
                string word;
                while (i < expr.length() && isalnum(expr[i])) {
                    word += expr[i++];
                }
                tokens.push_back(word);
            }
            // Скобки и операторы сравнения
            else {
                string op(1, expr[i]);
                if ((i + 1 < expr.length()) && expr[i + 1] == '=') {
                    op += expr[++i];
                }
                tokens.push_back(op);
                ++i;
            }
        }
        return tokens;
    }

    void execute(Blackboard& bb) {
        bb.tokens = tokenize(bb.rawExpression);
    }
};

// Агент 5: Преобразование в ОПН
struct RPNConverter {
    void execute(Blackboard& bb) {
        vector<string> output;
        stack<string> ops;

        for (const auto& tok : bb.tokens) {
            if (isdigit(tok[0]) || (tok.length() > 1 && isdigit(tok[1]))) {
                output.push_back(tok);
            }
            else if (isOperator(tok)) {
                while (!ops.empty() && isOperator(ops.top()) &&
                    getPrecedence(ops.top()) >= getPrecedence(tok)) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.push(tok);
            }
            else if (tok == "(") {
                ops.push(tok);
            }
            else if (tok == ")") {
                while (!ops.empty() && ops.top() != "(") {
                    output.push_back(ops.top());
                    ops.pop();
                }
                if (!ops.empty() && ops.top() == "(") {
                    ops.pop();
                }
                else {
                    throw runtime_error("Несоответствие скобок");
                }
            }
            else {
                throw runtime_error("Неизвестный токен: " + tok);
            }
        }

        while (!ops.empty()) {
            if (isParen(ops.top())) throw runtime_error("Несоответствие скобок");
            output.push_back(ops.top());
            ops.pop();
        }

        bb.rpn = output;
    }
};

// Агент 6: Вычисление результата
struct RPNEvaluator {
    bool applyOp(int lhs, int rhs, const string& op) {
        if (op == "AND" || op == "and") return lhs && rhs;
        if (op == "OR" || op == "or") return lhs || rhs;
        if (op == "==") return lhs == rhs;
        if (op == "!=") return lhs != rhs;
        if (op == ">") return lhs > rhs;
        if (op == "<") return lhs < rhs;
        if (op == ">=") return lhs >= rhs;
        if (op == "<=") return lhs <= rhs;
        throw runtime_error("Неизвестная операция: " + op);
    }

    void execute(Blackboard& bb) {
        stack<int> st;
        for (const auto& tok : bb.rpn) {
            if (isdigit(tok[0]) || (tok.length() > 1 && isdigit(tok[1]))) {
                st.push(stoi(tok));
            }
            else {
                if (st.size() < 2) throw runtime_error("Недостаточно операндов");
                int rhs = st.top(); st.pop();
                int lhs = st.top(); st.pop();
                st.push(applyOp(lhs, rhs, tok));
            }
        }
        if (st.size() != 1) throw runtime_error("Неверное выражение");
        bb.result = st.top() != 0;
        bb.resultComputed = true;
    }
};

// Контроллер: запускает агентов по очереди
struct BlackboardController {
    VariableExtractor extractor;
    ValuePrompter prompter;
    VariableReplacer replacer;
    Tokenizer tokenizer;
    RPNConverter converter;
    RPNEvaluator evaluator;

    void run(Blackboard& bb) {
        try {
            extractor.execute(bb);          // Найти переменные
            prompter.execute(bb);           // Запросить значения
            replacer.execute(bb);           // Заменить переменные на значения
            tokenizer.execute(bb);          // Токенизировать выражение
            converter.execute(bb);          // Преобразовать в ОПН
            evaluator.execute(bb);          // Вычислить результат

            cout << "Результат: " << boolalpha << bb.result << endl;
        }
        catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
        }
    }
};

struct LogicalEngine {
    BlackboardController controller;

    void runFromConsole() {
        Blackboard bb;
        cout << "Введите логическое выражение: ";
        getline(cin, bb.rawExpression);
        controller.run(bb);
    }

    void runFromArg(const string& arg) {
        Blackboard bb;
        bb.rawExpression = arg;
        controller.run(bb);
    }

    void runFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Ошибка: не удалось открыть файл: " << filename << endl;
            return;
        }

        vector<string> lines;
        string line;
        while (getline(file, line)) {
            lines.push_back(line);
        }

        vector<string> expressions;
        string currentExpr;
        int continuationLine = -1;
        bool expectingContinuation = false;

        auto isLogicalOp = [](const string& word) {
            return word == "AND" || word == "OR" || word == "and" || word == "or";
            };

        auto trim = [](const string& s) -> string {
            size_t start = s.find_first_not_of(" \t\r\n");
            size_t end = s.find_last_not_of(" \t\r\n");
            return (start == string::npos) ? "" : s.substr(start, end - start + 1);
            };

        auto getLastWord = [](const string& s) -> string {
            istringstream iss(s);
            string word, lastWord;
            while (iss >> word) lastWord = word;
            return lastWord;
            };

        auto getFirstWord = [](const string& s) -> string {
            istringstream iss(s);
            string word;
            iss >> word;
            return word;
            };

        regex paramRegex(R"(\[\s*[A-Za-z_][A-Za-z0-9_]*\s*\]\s*=\s*.+)");

        for (size_t i = 0; i < lines.size(); ++i) {
            string trimmed = trim(lines[i]);
            if (trimmed.empty()) {
                if (expectingContinuation) {
                    cerr << "Ошибка в строке " << continuationLine + 1
                        << ": выражение заканчивается на логический оператор '"
                        << getLastWord(lines[continuationLine])
                        << "', но следующая строка пуста.\n";
                    expectingContinuation = false;
                }
                continue;
            }

            if (regex_match(trimmed, paramRegex)) {
                continue;
            }

            if (trimmed.front() == '[') {
                if (!regex_match(trimmed, paramRegex)) {
                    cerr << "Ошибка в строке " << i + 1 << ": некорректная строка параметра: " << trimmed << endl;
                    continue;
                }
                continue; 
            }


            string firstWord = getFirstWord(trimmed);
            string lastWord = getLastWord(trimmed);

            if (expectingContinuation) {
                if (isLogicalOp(firstWord)) {
                    cerr << "Ошибка в строке " << i + 1 << ": выражение не может начинаться с логического оператора '"
                        << firstWord << "' после оператора '" << getLastWord(lines[continuationLine])
                        << "' в строке " << continuationLine + 1 << endl;
                    expectingContinuation = false;
                    continue;
                }
                currentExpr += " " + trimmed;
                expectingContinuation = false;
            }
            else if (isLogicalOp(lastWord)) {
                currentExpr += (currentExpr.empty() ? "" : " ") + trimmed;
                expectingContinuation = true;
                continuationLine = i;
            }
            else if (isLogicalOp(firstWord)) {
                if (currentExpr.empty()) {
                    cerr << "Ошибка в строке " << i + 1 << ": выражение начинается с логического оператора '"
                        << firstWord << "' без предыдущей части\n";
                    continue;
                }
                currentExpr += " " + trimmed;
            }
            else {
                if (!currentExpr.empty()) {
                    expressions.push_back(currentExpr);
                    currentExpr.clear();
                }
                currentExpr = trimmed;
            }
        }

        if (expectingContinuation) {
            cerr << "Ошибка в строке " << continuationLine + 1
                << ": выражение заканчивается на логический оператор '"
                << getLastWord(lines[continuationLine])
                << "', но файл закончился\n";
        }
        else if (!currentExpr.empty()) {
            string lastWord = getLastWord(currentExpr);
            if (isLogicalOp(lastWord)) {
                cerr << "Ошибка: последнее выражение заканчивается на логический оператор '"
                    << lastWord << "' без продолжения.\n";
            }
            else {
                expressions.push_back(currentExpr);
            }
        }

        for (const auto& expr : expressions) {
            Blackboard bb;
            bb.rawExpression = expr;
            cout << "\nОбработка выражения: " << expr << endl;
            controller.run(bb);
        }
    }
};


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru");

    LogicalEngine engine;

    cout << "Выберите режим:\n";
    cout << "1. Ввести выражение вручную\n";
    cout << "2. Ввести выражение аргументом при запуске\n";
    cout << "3. Загрузить выражения из файла\n";
    cout << "Ваш выбор: ";
    int choice;
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        engine.runFromConsole();
        break;
    case 2:
        if (argc >= 2) {
            engine.runFromArg(argv[1]);
        }
        else {
            cout << "Нет аргумента в командной строке.\n";
        }
        break;
    case 3: {
        cout << "Введите имя файла: ";
        string fname;
        getline(cin, fname);
        engine.runFromFile(fname);
        break;
    }
    default:
        cout << "Неверный выбор.\n";
        break;
    }

    return 0;
}

