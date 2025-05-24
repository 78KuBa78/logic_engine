#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <QtWidgets>

struct Blackboard {
    QString expression;
    std::vector<QString> tokens;
    std::set<QString> variables;
    std::unordered_map<QString, QString> values;
    std::unordered_map<std::string, std::string> params;
    std::vector<QString> rpn;
    bool resultComputed = false;
    bool result = false;
    QList<QString> expressions;
    QList<std::vector<QString>> tokensList;
    QList<bool> results;
};

struct VariableExtractor {
    void execute(Blackboard& bb);
};

struct ValuePrompter {
    void execute(Blackboard& bb);
};

struct VariableReplacer {
    void execute(Blackboard& bb);
};

struct Tokenizer {
    std::vector<QString> tokenize(const QString &expr);
    void execute(Blackboard& bb);
};

struct RPNConverter {
    void execute(Blackboard& bb);
};

struct RPNEvaluator {
    bool applyOp(int lhs, int rhs, const QString& op);
    void execute(Blackboard& bb);
};

struct BlackboardController {
    VariableExtractor extractor;
    ValuePrompter prompter;
    VariableReplacer replacer;
    Tokenizer tokenizer;
    RPNConverter converter;
    RPNEvaluator evaluator;

    void run(Blackboard& bb);
};

struct LogicalEngine {
    BlackboardController controller;

    void runFromInputLine(QLineEdit* expressionInput, Blackboard& bb, QTextEdit* exprList);
    void runFromFile(QString& filename, Blackboard& bb, QTextEdit* exprList);
};

#endif // LOGIC_H
