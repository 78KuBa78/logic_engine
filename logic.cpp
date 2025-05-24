#include "logic.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stack>
#include <algorithm>


bool isOperator(const QString& token) {
    static const QSet<QString> ops = {
        "AND", "OR", ">", "<", "==", "!=", ">=", "<=", "and", "or"
    };
    return ops.contains(token);
}

bool isParen(const QString& token) {
    return token == "(" || token == ")";
}

int getPrecedence(const QString& op) {
    if (op == "OR" || op == "or") return 1;
    if (op == "AND" || op == "and") return 2;
    if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=") return 3;
    return 0;
}


// VariableExtractor Агент 1: Поиск переменных
void VariableExtractor::execute(Blackboard& bb) {
    QRegularExpression re(R"(\b[A-Za-z_][A-Za-z0-9_]*\b)");
    bb.variables.clear();

    auto extractVariablesFromExpr = [&](const QString& expr) {
        QRegularExpressionMatchIterator i = re.globalMatch(expr);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString var = match.captured(0);
            if (!isOperator(var) && !isParen(var)) {
                bb.variables.insert(var);
            }
        }
    };

    if (!bb.expressions.isEmpty()) {
        for (const QString& expr : bb.expressions) {
            extractVariablesFromExpr(expr);
        }
    }
    else if (!bb.expression.isEmpty()) {
        extractVariablesFromExpr(bb.expression);
    }
}

// ValuePrompter Агент 2: Запрос значений переменных
void ValuePrompter::execute(Blackboard& bb) {
    for (const auto& var : bb.variables) {
        if (bb.values.find(var) == bb.values.end()) {
            QString value;
            bb.values[var] = value;
        }
    }
}


// VariableReplacer Агент 3: Замена переменных на значения
void VariableReplacer::execute(Blackboard& bb) {
    QRegularExpression re(R"(\b[A-Za-z_][A-Za-z0-9_]*\b)");

    auto replaceVariablesInExpr = [&](const QString& expr) -> QString {
        QRegularExpressionMatchIterator iter = re.globalMatch(expr);
        QString result;
        int lastPos = 0;

        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            int start = match.capturedStart();
            int len = match.capturedLength();
            QString var = match.captured();

            result += expr.mid(lastPos, start - lastPos);
            if (bb.values.find(var) != bb.values.end()) {
                result += bb.values.at(var);
            } else {
                result += var;
            }
            lastPos = start + len;
        }
        result += expr.mid(lastPos);
        return result;
    };

    if (!bb.expressions.isEmpty()) {
        for (int i = 0; i < bb.expressions.size(); ++i) {
            bb.expressions[i] = replaceVariablesInExpr(bb.expressions[i]);
        }
    } else if (!bb.expression.isEmpty()) {
        bb.expression = replaceVariablesInExpr(bb.expression);
    }
}


// Tokenizer Агент 4: Токенизация(tokenize)
std::vector<QString> Tokenizer::tokenize(const QString& expr) {
    std::vector<QString> tokens;
    int i = 0;

    while (i < expr.length()) {
        if (expr[i].isSpace()) {
            ++i;
            continue;
        }

        if (expr[i].isDigit() ||
            (expr[i] == '-' &&
             (i == 0 || expr[i - 1] == '(' || isOperator(tokens.empty() ? "" : tokens.back())) &&
             i + 1 < expr.length() && expr[i + 1].isDigit())) {

            QString num;
            if (expr[i] == '-') {
                num += expr[i++];
            }
            while (i < expr.length() && expr[i].isDigit()) {
                num += expr[i++];
            }
            tokens.push_back(num);
        }
        else if (expr[i].isLetter()) {
            QString word;
            while (i < expr.length() && (expr[i].isLetterOrNumber() || expr[i] == '_')) {
                word += expr[i++];
            }
            tokens.push_back(word);
        }
        else {
            QString op(expr[i]);
            if (i + 1 < expr.length() && expr[i + 1] == '=') {
                op += expr[++i];
            }
            tokens.push_back(op);
            ++i;
        }
    }
    return tokens;
}

// Tokenizer Агент 4: Токенизация (execute)
void Tokenizer::execute(Blackboard& bb) {
    bb.tokens.clear();
    bb.tokensList.clear();

    if (!bb.expressions.isEmpty()) {
        for (const QString& expr : bb.expressions) {
            std::vector<QString> tokensForExpr = tokenize(expr);
            bb.tokens.insert(bb.tokens.end(), tokensForExpr.begin(), tokensForExpr.end());
            bb.tokensList.push_back(tokensForExpr);
        }
    } else if (!bb.expression.isEmpty()) {
        std::vector<QString> tokensForExpr = tokenize(bb.expression);
        bb.tokens = tokensForExpr;
        bb.tokensList.push_back(tokensForExpr);
    }
}


// RPNConverter Агент 5: Преобразование в ОПН
void RPNConverter::execute(Blackboard& bb) {
    std::vector<QString> fullOutput;

    for (const auto& tokens : bb.tokensList) {
        std::vector<QString> output;
        std::stack<QString> ops;

        for (const auto& tok : tokens) {
            if (tok[0].isDigit() || (tok.length() > 1 && tok[0] == '-' && tok[1].isDigit())) {
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
                    throw std::runtime_error("Несоответствие скобок (Brackets)");
                }
            }
            else {
                throw std::runtime_error("Неизвестный токен: " + tok.toStdString());
            }
        }

        while (!ops.empty()) {
            if (isParen(ops.top())) {
                throw std::runtime_error("Несоответствие скобок (Brackets)");
            }
            output.push_back(ops.top());
            ops.pop();
        }

        fullOutput.insert(fullOutput.end(), output.begin(), output.end());
        fullOutput.push_back("__END__");
    }

    bb.rpn = fullOutput;
}


// RPNEvaluator Агент 6: Вычисление результата
bool RPNEvaluator::applyOp(int lhs, int rhs, const QString& op) {
    if (op == "AND" || op == "and") return lhs && rhs;
    if (op == "or" || op == "or") return lhs || rhs;
    if (op == "==") return lhs == rhs;
    if (op == "!=") return lhs != rhs;
    if (op == ">") return lhs > rhs;
    if (op == "<") return lhs < rhs;
    if (op == ">=") return lhs >= rhs;
    if (op == "<=") return lhs <= rhs;
    throw std::runtime_error("Неизвестная операция: " + op.toStdString());
}


void RPNEvaluator::execute(Blackboard& bb) {
    std::stack<int> st;
    bb.results.clear();

    for (const auto& tok : bb.rpn) {
        if (tok == "__END__") {
            if (st.size() != 1) {
                throw std::runtime_error("Неверное выражение (Incorrect expression)");
            }
            bb.results.append(st.top() != 0);
            while (!st.empty()) st.pop();
        }
        else if (tok[0].isDigit() || (tok.length() > 1 && tok[0] == '-' && tok[1].isDigit())) {
            bool ok = false;
            int val = tok.toInt(&ok);
            if (!ok) throw std::runtime_error("Ошибка конвертации числа: " + tok.toStdString());
            st.push(val);
        }
        else {
            if (st.size() < 2) {
                throw std::runtime_error("Недостаточно операндов (Not enough operands)");
            }
            int rhs = st.top(); st.pop();
            int lhs = st.top(); st.pop();
            st.push(applyOp(lhs, rhs, tok));
        }
    }

    if (!bb.rpn.empty() && bb.rpn.back() != "__END__") {
        if (st.size() != 1) {
            throw std::runtime_error("Неверное выражение (Incorrect expression)");
        }
        bb.results.append(st.top() != 0);
    }

    if (!bb.results.isEmpty()) {
        bb.result = bb.results.back();
        bb.resultComputed = true;
    }
}

void LogicalEngine::runFromInputLine(QLineEdit* expressionInput, Blackboard& bb, QTextEdit* exprList) {
    QString input = expressionInput->text().trimmed();
    if (input.isEmpty()) {
        exprList->setText("Ошибка: пустое выражение.");
        return;
    }

    const QStringList logicalOps = {"AND", "OR", ">", "<", "==", "!=", ">=", "<="};

    auto getLastWord = [](const QString& s) -> QString {
        QStringList words = s.trimmed().split(QRegularExpression("\\s+"));
        return words.isEmpty() ? "" : words.last();
    };

    auto getFirstWord = [](const QString& s) -> QString {
        QStringList words = s.trimmed().split(QRegularExpression("\\s+"));
        return words.isEmpty() ? "" : words.first();
    };

    auto isLogicalOp = [&](const QString& word) -> bool {
        QString upperWord = word.toUpper();
        return logicalOps.contains(upperWord);
    };

    QRegularExpression paramRegex(R"(^\[\s*[A-Za-z_][A-Za-z0-9_]*\s*\]\s*=\s*.+)");

    if (input.front() == QChar('[')) {
        if (!paramRegex.match(input).hasMatch()) {
            exprList->setText("Ошибка: некорректная строка параметра: " + input);
            return;
        }
        exprList->append("Параметр принят: " + input);
        return;
    }

    QString firstWord = getFirstWord(input);
    QString lastWord = getLastWord(input);

    if (isLogicalOp(firstWord)) {
        exprList->setText("Ошибка: выражение не может начинаться с оператора: " + firstWord);
        return;
    }
    if (isLogicalOp(lastWord)) {
        exprList->setText("Ошибка: выражение не может заканчиваться на оператор: " + lastWord);
        return;
    }

    bb.expressions.clear();
    bb.expressions.append(input);
    exprList->append("Обработка выражения: " + input);
}

void LogicalEngine::runFromFile(QString& filename, Blackboard& bb, QTextEdit* exprList) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: не удалось открыть файл:" << filename;
        return;
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    file.close();

    QMap<QString, QString> paramValues;
    QString currentExpr;
    int continuationLine = -1;
    bool expectingContinuation = false;

    auto isLogicalOp = [](const QString& word) {
        return word.compare("AND", Qt::CaseInsensitive) == 0 ||
               word.compare("OR", Qt::CaseInsensitive) == 0;
    };

    auto hasValidComparisons = [](const QString& expr) -> bool {
        QStringList tokens = expr.trimmed().split(QRegularExpression("\\s+"));
        static const QSet<QString> compOps = {"==", "!=", "<", ">", "<=", ">="};
        for (int i = 0; i < tokens.size(); ++i) {
            if (compOps.contains(tokens[i])) {
                if (i == 0 || i == tokens.size() - 1)
                    return false;
                if (compOps.contains(tokens[i - 1]) || compOps.contains(tokens[i + 1]))
                    return false;
            }
        }
        return true;
    };

    auto trim = [](const QString& s) -> QString {
        return s.trimmed();
    };

    auto getLastWord = [](const QString& s) -> QString {
        QStringList words = s.trimmed().split(QRegularExpression("\\s+"));
        return words.isEmpty() ? "" : words.last();
    };

    auto getFirstWord = [](const QString& s) -> QString {
        QStringList words = s.trimmed().split(QRegularExpression("\\s+"));
        return words.isEmpty() ? "" : words.first();
    };

    QRegularExpression assignmentRegex(R"(^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.+?)\s*$)");
    QRegularExpression paramUsageRegex(R"(\[\s*([A-Za-z_][A-Za-z0-9_]*)\s*\])");

    for (int i = 0; i < lines.size(); ++i) {
        QString trimmed = trim(lines[i]);
        if (trimmed.isEmpty()) continue;

        QRegularExpressionMatch match = assignmentRegex.match(trimmed);
        if (match.hasMatch()) {
            QString paramName = match.captured(1);
            QString value = match.captured(2);
            paramValues[paramName] = value;
            continue;
        }

        QString firstWord = getFirstWord(trimmed);
        QString lastWord = getLastWord(trimmed);

        if (expectingContinuation) {
            if (isLogicalOp(firstWord)) {
                QString msg = QString("Ошибка в строке %1: выражение не может начинаться с логического оператора '%2' после '%3'")
                                  .arg(i + 1)
                                  .arg(firstWord)
                                  .arg(getLastWord(lines[continuationLine]));
                if (exprList) exprList->append(msg);
                expectingContinuation = false;
                continue;
            }

            currentExpr += " " + trimmed;
            expectingContinuation = false;
        }
        else if (isLogicalOp(lastWord)) {
            currentExpr += (currentExpr.isEmpty() ? "" : " ") + trimmed;
            expectingContinuation = true;
            continuationLine = i;
        }
        else if (isLogicalOp(firstWord)) {
            if (currentExpr.isEmpty()) {
                QString msg = QString("Ошибка в строке %1: выражение начинается с логического оператора '%2' без предыдущей части.")
                                  .arg(i + 1)
                                  .arg(firstWord);
                if (exprList) exprList->append(msg);
                continue;
            }
            currentExpr += " " + trimmed;
        }
        else {
            if (!currentExpr.isEmpty()) {
                QString exprWithValues = currentExpr;

                QRegularExpressionMatchIterator it = paramUsageRegex.globalMatch(exprWithValues);
                while (it.hasNext()) {
                    QRegularExpressionMatch m = it.next();
                    QString param = m.captured(1);
                    if (paramValues.contains(param)) {
                        exprWithValues.replace(m.captured(0), paramValues[param]);
                    }
                    else {
                        QString msg = QString("Ошибка: параметр [%1] не задан. Строка %2.").arg(param).arg(i + 1);
                        if (exprList) exprList->append(msg);
                        exprWithValues.clear();
                        break;
                    }
                }

                if (!exprWithValues.isEmpty() && hasValidComparisons(exprWithValues)) {
                    bb.expressions.append(exprWithValues);
                }
                else if (!exprWithValues.isEmpty()) {
                    QString msg = QString("Ошибка в строке %1: выражение содержит некорректные операторы сравнения.")
                                      .arg(i + 1);
                    if (exprList) exprList->append(msg);
                }

                currentExpr.clear();
            }

            currentExpr = trimmed;
        }
    }

    if (!currentExpr.isEmpty()) {
        QString exprWithValues = currentExpr;
        QRegularExpressionMatchIterator it = paramUsageRegex.globalMatch(exprWithValues);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            QString param = m.captured(1);
            if (paramValues.contains(param)) {
                exprWithValues.replace(m.captured(0), paramValues[param]);
            }
            else {
                QString msg = QString("Ошибка: параметр [%1] не задан в последнем выражении.").arg(param);
                if (exprList) exprList->append(msg);
                exprWithValues.clear();
                break;
            }
        }

        if (!exprWithValues.isEmpty() && hasValidComparisons(exprWithValues)) {
            bb.expressions.append(exprWithValues);
        }
    }

    for (const QString& expr : bb.expressions) {
        if (exprList) {
            exprList->append(expr);
        }
    }
}









