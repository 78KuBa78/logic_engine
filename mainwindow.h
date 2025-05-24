#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QTextEdit>
#include "logic.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Blackboard bb;
    std::set<QString>::iterator currentVar;

    LogicalEngine engine;
    QLineEdit*     expression;
    QPushButton* calcButton;
    QPushButton* calcFileButton;
    QPushButton* loadFromButton;
    QRadioButton* inputRadioButton;
    QRadioButton* inputArgsButton;
    QRadioButton* loadFromRadioButton;
    QPushButton* startInputVarBtn;
    QPushButton* startInputFileVarBtn;
    QLabel* varLabel;
    QLineEdit* varInput;
    QPushButton* saveVarButton;
    QTextEdit* exprList;
    QTextEdit* exprOutput;
    QTextEdit* resultOutput;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

public slots:
    void loadFromFile();
    void calcBtnClicked();
    void calcFromFileClicked();
    void saveVarClicked();
    void inputRadio(bool checked);
    void loadFromRadio(bool checked);
    void startInput();
    void startInputFromFile();
};
#endif // MAINWINDOW_H
