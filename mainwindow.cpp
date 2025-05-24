#include "mainwindow.h"


LogicalEngine engine;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , expression(new QLineEdit)
    , calcButton(new QPushButton("Вычислить выражение", this))
    , calcFileButton(new QPushButton("Вычислить выражение", this))
    , exprList(new QTextEdit())
    , loadFromButton(new QPushButton("Загрузить из файла", this))
    , inputRadioButton(new QRadioButton("Ввести выражение вручную", this))
    , loadFromRadioButton(new QRadioButton("Выражение из файла", this))
    , exprOutput(new QTextEdit())
    , resultOutput(new QTextEdit())
    , startInputVarBtn(new QPushButton("Ввести значения переменных", this))
    , startInputFileVarBtn(new QPushButton("Ввести значения переменных(file)", this))
    , varInput(new QLineEdit)
    , varLabel(new QLabel("Переменная  =", this))
    , saveVarButton(new QPushButton("Сохранить", this))

{

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QObject::connect(loadFromButton, SIGNAL(clicked()), this, SLOT(loadFromFile()));
    QObject::connect(calcButton, SIGNAL(clicked()), this, SLOT(calcBtnClicked()));
    QObject::connect(calcFileButton, SIGNAL(clicked()), this, SLOT(calcFromFileClicked()));
    QObject::connect(saveVarButton, SIGNAL(clicked()), this, SLOT(saveVarClicked()));
    QObject::connect(inputRadioButton, SIGNAL(toggled(bool)), this, SLOT(inputRadio(bool)));
    QObject::connect(loadFromRadioButton, SIGNAL(toggled(bool)), this, SLOT(loadFromRadio(bool)));
    QObject::connect(startInputVarBtn, SIGNAL(clicked()), this, SLOT(startInput()));
    QObject::connect(startInputFileVarBtn, SIGNAL(clicked()), this, SLOT(startInputFromFile()));


    setFixedSize(1200, 700);

    expression->setPlaceholderText("Введите логическое выражение...");

    expression->setStyleSheet(
        "QLineEdit {"
        "background-color: #f0f0f0;"
        "border: 2px solid #0078d4;"
        "border-radius: 10px;"
        "padding: 5px 10px;"
        "font-size: 14px;"
        "color: black;"
        "}"

        "QLineEdit:focus {"
        "border: 2px solid #005a8f;"
        "background-color: white;"
        "}"
        );


    loadFromButton->setStyleSheet(
        "QPushButton {"
        "background-color: rgb(149, 165, 245);"
        "color: #333;"
        "border-radius: 10px;"
        "border: none;"
        "padding: 10px 20px;"
        "font-weight: bold;"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-size: 13px;"
        "}"

        "QPushButton:hover {"
        "background-color: rgb(125, 142, 227);"
        "}"

        "QPushButton:pressed {"
        "background-color: rgb(106, 120, 189);"
        "}"
        );


    calcButton->setShortcut(QKeySequence(Qt::Key_Return));
    calcButton->setStyleSheet(
        "QPushButton {"
        "background-color: rgb(149, 165, 245);"
        "color: #333;"
        "border-radius: 10px;"
        "border: none;"
        "padding: 10px 20px;"
        "font-weight: bold;"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-size: 13px;"
        "}"

        "QPushButton:hover {"
        "background-color: rgb(125, 142, 227);"
        "}"

        "QPushButton:pressed {"
        "background-color: rgb(106, 120, 189);"
        "}"
        );

    calcFileButton->setShortcut(QKeySequence(Qt::Key_Return));
    calcFileButton->setStyleSheet(
        "QPushButton {"
        "background-color: rgb(149, 165, 245);"
        "color: #333;"
        "border-radius: 10px;"
        "border: none;"
        "padding: 10px 20px;"
        "font-weight: bold;"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-size: 13px;"
        "}"

        "QPushButton:hover {"
        "background-color: rgb(125, 142, 227);"
        "}"

        "QPushButton:pressed {"
        "background-color: rgb(106, 120, 189);"
        "}"
        );

    exprOutput->setStyleSheet(
        "QTextEdit {"
        "background-color: #f0f0f0;"
        "border: 2px solid rgb(72, 199, 93);"
        "border-radius: 10px;"
        "padding: 5px 10px;"
        "font-size: 14px;"
        "color: black;"
        "}"

        "QTextEdit:focus {"
        "border: 2px solid rgb(72, 199, 93);"
        "background-color: white;"
        "}"
        );

    resultOutput->setStyleSheet(
        "QTextEdit {"
        "background-color: #f0f0f0;"
        "border: 2px solid rgb(72, 199, 93);"
        "border-radius: 10px;"
        "padding: 5px 10px;"
        "font-size: 14px;"
        "color: black;"
        "}"

        "QTextEdit:focus {"
        "border: 2px solid rgb(72, 199, 93);"
        "background-color: white;"
        "}"
        );

    exprList->setStyleSheet(
        "QTextEdit {"
        "background-color: #f0f0f0;"
        "border: 2px solid rgb(72, 199, 93);"
        "border-radius: 10px;"
        "padding: 5px 10px;"
        "font-size: 14px;"
        "color: black;"
        "}"

        "QTextEdit:focus {"
        "border: 2px solid rgb(72, 199, 93);"
        "background-color: white;"
        "}"
        );

    varInput->setStyleSheet(
        "QLineEdit {"
        "background-color: #f0f0f0;"
        "border: 2px solid #0078d4;"
        "border-radius: 10px;"
        "padding: 5px 10px;"
        "font-size: 14px;"
        "color: black;"
        "}"

        "QLineEdit:focus {"
        "border: 2px solid #005a8f;"
        "background-color: white;"
        "}"
        );

    startInputVarBtn->setStyleSheet(
        "QPushButton {"
        "background-color: rgb(149, 165, 245);"
        "color: #333;"
        "border-radius: 10px;"
        "border: none;"
        "padding: 10px 20px;"
        "font-weight: bold;"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-size: 13px;"
        "}"

        "QPushButton:hover {"
        "background-color: rgb(125, 142, 227);"
        "}"

        "QPushButton:pressed {"
        "background-color: rgb(106, 120, 189);"
        "}"
        );

    startInputFileVarBtn->setStyleSheet(
        "QPushButton {"
        "background-color: rgb(149, 165, 245);"
        "color: #333;"
        "border-radius: 10px;"
        "border: none;"
        "padding: 10px 20px;"
        "font-weight: bold;"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-size: 13px;"
        "}"

        "QPushButton:hover {"
        "background-color: rgb(125, 142, 227);"
        "}"

        "QPushButton:pressed {"
        "background-color: rgb(106, 120, 189);"
        "}"
        );

    saveVarButton->setStyleSheet(
        "QPushButton {"
        "background-color: rgb(149, 165, 245);"
        "color: #333;"
        "border-radius: 10px;"
        "border: none;"
        "padding: 10px 20px;"
        "font-weight: bold;"
        "font-family: 'Montserrat';"
        "font-style: normal;"
        "font-size: 13px;"
        "}"

        "QPushButton:hover {"
        "background-color: rgb(125, 142, 227);"
        "}"

        "QPushButton:pressed {"
        "background-color: rgb(106, 120, 189);"
        "}"
        );

    varLabel->setAlignment(Qt::AlignCenter);
    varLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *up = new QHBoxLayout;
    QVBoxLayout *radioBtnLayout = new QVBoxLayout;

    radioBtnLayout->addWidget(inputRadioButton);
    radioBtnLayout->addWidget(inputArgsButton);
    radioBtnLayout->addWidget(loadFromRadioButton);
    up->addLayout(radioBtnLayout);

    QHBoxLayout *fileBtnLayout = new QHBoxLayout;
    fileBtnLayout->addWidget(loadFromButton);
    up->addLayout(fileBtnLayout);

    expression->setVisible(false);
    loadFromButton->setVisible(false);
    calcButton->setVisible(false);
    calcFileButton->setVisible(false);
    exprList->setVisible(false);

    up->addWidget(calcButton);
    up->addWidget(calcFileButton);
    up->addWidget(expression);
    up->addWidget(exprList);

    QHBoxLayout *mid1 = new QHBoxLayout;
    QHBoxLayout *mid2 = new QHBoxLayout;
    QHBoxLayout *down = new QHBoxLayout;

    QLabel *exprLabel = new QLabel("Выражение");
    exprOutput->setReadOnly(true);

    QLabel *resultLabel = new QLabel("Результат   ");
    resultOutput->setReadOnly(true);

    varLabel->setVisible(false);
    varInput->setVisible(false);
    saveVarButton->setVisible(false);
    startInputVarBtn->setVisible(false);

    startInputFileVarBtn->setVisible(false);

    mid1->addWidget(exprLabel);
    mid1->addWidget(exprOutput);
    mid2->addWidget(resultLabel);
    mid2->addWidget(resultOutput);

    down->addWidget(startInputVarBtn);
    down->addWidget(startInputFileVarBtn);
    down->addWidget(varLabel);
    down->addWidget(varInput);
    down->addWidget(saveVarButton);

    mainLayout->addLayout(up);
    mainLayout->addLayout(mid1);
    mainLayout->addLayout(mid2);
    mainLayout->addLayout(down);
    setLayout(mainLayout);
}


void MainWindow::calcBtnClicked() {
    varLabel->setVisible(false);
    varInput->setVisible(false);
    saveVarButton->setVisible(false);
    exprList->setVisible(true);
    exprOutput->clear();
    resultOutput->clear();
    exprList->clear();

    QString expr = expression->text();
    bb.expression = expr;
    engine.runFromInputLine(expression, bb, exprList);

    if (exprList) {
        QString firstLine = exprList->toPlainText().section('\n', 0, 0).trimmed();
        if (firstLine.startsWith("Ошибка")) {
            startInputVarBtn->setVisible(false);
        }
        else {
            startInputVarBtn->setVisible(true);
        }
    }

    engine.controller.extractor.execute(bb);
}

void MainWindow::startInput() {
    startInputVarBtn->setVisible(false);
    if (bb.variables.empty()) {
        varLabel->setText("Нет переменных для ввода");
        varLabel->setVisible(true);

        engine.controller.replacer.execute(bb);
        engine.controller.tokenizer.execute(bb);

        exprOutput->setText(bb.expression);

        engine.controller.converter.execute(bb);
        engine.controller.evaluator.execute(bb);

        resultOutput->setText(bb.result ? "True" : "False");
    }
    else {
        currentVar = bb.variables.begin();
        varLabel->setText(QString("Переменная %1 =").arg(*currentVar));
        varLabel->setVisible(true);
        varInput->setVisible(true);
        saveVarButton->setVisible(true);
    }

}

void MainWindow::startInputFromFile() {
    startInputFileVarBtn->setVisible(false);

    for (const QString& expr : bb.expressions) {
        if (bb.variables.empty()) {
            varLabel->setText("Нет переменных для ввода");
            varLabel->setVisible(true);

            engine.controller.replacer.execute(bb);
            engine.controller.tokenizer.execute(bb);

            exprOutput->setText(expr);

            engine.controller.converter.execute(bb);
            engine.controller.evaluator.execute(bb);

            resultOutput->setText(bb.result ? "True" : "False");
        }
        else {
            currentVar = bb.variables.begin();
            varLabel->setText(QString("Переменная %1 =").arg(*currentVar));
            varLabel->setVisible(true);
            varInput->setVisible(true);
            saveVarButton->setVisible(true);
        }
    }

}

void MainWindow::saveVarClicked() {

    if (currentVar != bb.variables.end()) {
        QString varName = *currentVar;
        QString input = varInput->text().trimmed();
        bb.values[varName] = input;
        ++currentVar;

        if (currentVar != bb.variables.end()) {
            varLabel->setText(QString("Переменная %1 =").arg(*currentVar));
            varInput->clear();
        }
        else {
            varLabel->setText("Все значения сохранены.");
            varInput->setVisible(false);
            saveVarButton->setVisible(false);

            engine.controller.replacer.execute(bb);
            engine.controller.tokenizer.execute(bb);

            if (!bb.expressions.isEmpty()) {
                for (const QString& expr : bb.expressions) {
                    exprOutput->append(expr);
                }
            }
            else {
                exprOutput->setText(bb.expression);
            }

            engine.controller.converter.execute(bb);
            engine.controller.evaluator.execute(bb);

            resultOutput->clear();
            if (!bb.results.isEmpty()) {
                for(bool res : bb.results) {
                    resultOutput->append(res ? "True" : "False");
                }
                bb.expressions.clear();
            }
            else {
                resultOutput->setText(bb.result ? "True" : "False");
            }
            bb.expression.clear();
        }
    }
}


void MainWindow::calcFromFileClicked() {
    exprOutput->clear();
    resultOutput->clear();
    varLabel->setVisible(false);
    engine.controller.extractor.execute(bb);
    startInputFileVarBtn->setVisible(true);
}

void MainWindow::loadFromFile() {

    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Text Files (*.txt);;All Files (*)");
    if (filename.isEmpty())
        return;

    exprList->clear();
    bb.expressions.clear();
    calcFileButton->setVisible(true);
    engine.runFromFile(filename, bb, exprList);

    if (exprList) {
        QString firstLine = exprList->toPlainText().section('\n', 0, 0).trimmed();
        if (firstLine.startsWith("Ошибка")) {
            calcFileButton->setVisible(false);
        }
    }

    expression->setText(bb.expression);
    loadFromButton->setVisible(false);
    exprList->setVisible(true);
}


void MainWindow::inputRadio(bool checked) {
    expression->setVisible(checked);
    calcButton->setVisible(checked);
    calcFileButton->setVisible(false);
    exprList->setVisible(false);
    startInputVarBtn->setVisible(false);
    startInputFileVarBtn->setVisible(false);
    varLabel->setVisible(false);
    exprList->clear();

}


void MainWindow::loadFromRadio(bool checked) {
    loadFromButton->setVisible(checked);
    startInputVarBtn->setVisible(false);
    startInputFileVarBtn->setVisible(false);
    varLabel->setVisible(false);
}





