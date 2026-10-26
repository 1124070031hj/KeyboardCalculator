#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QJSEngine>
#include <QRegularExpression>
#include <cmath>
#include <QKeyEvent>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 让主窗口能够接收键盘事件
    setFocusPolicy(Qt::StrongFocus);

    // 显示框只负责显示，不直接编辑文本
    ui->displayEdit->setReadOnly(true);
    ui->displayEdit->setFocusPolicy(Qt::NoFocus);

    // 按钮不抢夺键盘焦点
    QPushButton *allButtons[] = {
        ui->btn0, ui->btn1, ui->btn2, ui->btn3, ui->btn4,
        ui->btn5, ui->btn6, ui->btn7, ui->btn8, ui->btn9,
        ui->btnDot, ui->btnClear, ui->btnBackspace,
        ui->btnAdd, ui->btnSubtract,
        ui->btnMultiply, ui->btnDivide, ui->btnEqual
    };

    for (QPushButton *button : allButtons) {
        button->setFocusPolicy(Qt::NoFocus);
    }

    setFocus();
    connect(ui->displayEdit, &QLineEdit::textChanged,
            this, &MainWindow::updatePreview);
    connect(ui->btnEqual, &QPushButton::clicked,
            this, &MainWindow::calculateResult);
    // 加法
    connect(ui->btnAdd, &QPushButton::clicked,
            this, [this]() {
                inputOperator("+");
            });

    // 减法
    connect(ui->btnSubtract, &QPushButton::clicked,
            this, [this]() {
                inputOperator("-");
            });

    // 乘法
    connect(ui->btnMultiply, &QPushButton::clicked,
            this, [this]() {
                inputOperator("×");
            });

    // 除法
    connect(ui->btnDivide, &QPushButton::clicked,
            this, [this]() {
                inputOperator("÷");
            });
    // 将10个数字按钮放进数组
    QPushButton *digitButtons[10] = {
        ui->btn0,
        ui->btn1,
        ui->btn2,
        ui->btn3,
        ui->btn4,
        ui->btn5,
        ui->btn6,
        ui->btn7,
        ui->btn8,
        ui->btn9
    };

    // 用循环连接10个按钮
    for (int i = 0; i < 10; i++) {

        connect(digitButtons[i], &QPushButton::clicked,
                this, [this, i]() {

                    inputDigit(i);

                });
    }
    connect(ui->btnDot, &QPushButton::clicked,
            this, &MainWindow::inputDot);
    connect(ui->btnClear, &QPushButton::clicked,
            this, [this]() {
                ui->displayEdit->setText("0");
            });
    connect(ui->btnBackspace, &QPushButton::clicked,
            this, [this]() {

                QString current = ui->displayEdit->text();

                if (current.length() <= 1) {
                    ui->displayEdit->setText("0");
                }
                else {
                    current.chop(1);
                    ui->displayEdit->setText(current);
                }

            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QString text = event->text();

    // 1. 数字键：调用对应的数字按钮
    if (text.size() == 1 && text.at(0).isDigit()) {
        bool ok = false;
        int digit = text.toInt(&ok);

        if (ok && digit >= 0 && digit <= 9) {
            QPushButton *digitButtons[10] = {
                ui->btn0, ui->btn1, ui->btn2, ui->btn3,
                ui->btn4, ui->btn5, ui->btn6, ui->btn7,
                ui->btn8, ui->btn9
            };

            digitButtons[digit]->click();
            event->accept();
            return;
        }
    }

    // 2. 运算符：复用原来的按钮逻辑
    if (text == "+") {
        ui->btnAdd->click();
    }
    else if (text == "-") {
        ui->btnSubtract->click();
    }
    else if (text == "*") {
        ui->btnMultiply->click();
    }
    else if (text == "/") {
        ui->btnDivide->click();
    }
    else if (text == ".") {
        ui->btnDot->click();
    }
    // 3. 回车：计算结果
    else if (event->key() == Qt::Key_Return ||
             event->key() == Qt::Key_Enter) {
        ui->btnEqual->click();
    }
    // 4. 退格
    else if (event->key() == Qt::Key_Backspace) {
        ui->btnBackspace->click();
    }
    // 5. Esc：清除
    else if (event->key() == Qt::Key_Escape) {
        ui->btnClear->click();
    }
    else {
        QMainWindow::keyPressEvent(event);
        return;
    }

    event->accept();
}

void MainWindow::inputDigit(int digit)
{
    QString current = ui->displayEdit->text();

    QString number = QString::number(digit);

   if (current == "0" || current == "错误") {
        ui->displayEdit->setText(number);
    }
    else {
        ui->displayEdit->setText(current + number);
    }
}
void MainWindow::inputOperator(const QString &op)
{
    QString current = ui->displayEdit->text();

    // 1. 允许以负号开头
    if (current.isEmpty() || current == "0" ||
        current == "错误") {

        if (op == "-") {
            ui->displayEdit->setText("-");
        }
        else if (current == "0") {
            ui->displayEdit->setText(current + op);
        }

        return;
    }

    // 2. 获取最后一个字符
    QString operators = "+-×÷";
    QString last = current.right(1);

    // 3. 最后不是运算符，可以正常追加
    if (!operators.contains(last)) {
        ui->displayEdit->setText(current + op);
        return;
    }

    // 4. 如果输入的是负号
    if (op == "-") {

        // 前面是 +、×、÷，可以追加负号
        if (last == "+" || last == "×" || last == "÷") {
            ui->displayEdit->setText(current + "-");
            return;
        }

        // 前面是减号，判断它是否是二元减法
        if (last == "-" && current.length() >= 2) {
            QChar before = current.at(current.length() - 2);

            if (before.isDigit() || before == '.') {
                ui->displayEdit->setText(current + "-");
            }
        }
    }
}
void MainWindow::inputDot()
{
    QString current = ui->displayEdit->text();

    // 获取四则运算符的位置
    QString operators = "+-×÷";
    int lastOperatorIndex = -1;

    for (int i = 0; i < current.length(); i++) {
        if (operators.contains(current.at(i))) {
            lastOperatorIndex = i;
        }
    }

    // 提取最后一个运算符后面的数字
    QString currentNumber = current.mid(lastOperatorIndex + 1);

    // 如果当前数字已有小数点，直接返回
    if (currentNumber.contains(".")) {
        return;
    }

    // 如果刚输入完运算符，先补0
    if (current == "错误" || current.isEmpty()) {
        ui->displayEdit->setText("0.");
        return;
    }
    else {
        ui->displayEdit->setText(current + ".");
    }

}
void MainWindow::calculateResult()
{
    QString expression = ui->displayEdit->text();

    double number = 0;

    if (!evaluateExpression(expression, number)) {
        ui->displayEdit->setText("错误");
        ui->previewLabel->clear();
        return;
    }

    ui->displayEdit->setText(
        QString::number(number, 'g', 15)
        );

    ui->previewLabel->clear();
}
void MainWindow::updatePreview()
{
    QString expression = ui->displayEdit->text();

    double number = 0;

    if (!evaluateExpression(expression, number)) {
        ui->previewLabel->clear();
        return;
    }

    ui->previewLabel->setText(
        QString::number(number, 'g', 15)
        );
}
bool MainWindow::evaluateExpression(
    const QString &expression, double &number)
{
    // 允许负数参与运算
    QRegularExpression pattern(
        R"(^-?\d+(?:\.\d*)?(?:[+\-×÷]-?\d+(?:\.\d*)?)*$)"
        );

    if (!pattern.match(expression).hasMatch()) {
        return false;
    }

    // 转成 JavaScript 能识别的运算符
    QString jsExpression = expression;

    jsExpression.replace("×", "*");
    jsExpression.replace("÷", "/");

    // 例如 5--3 转成 5- -3
    // 防止 JavaScript 将 -- 识别为自减运算符
    jsExpression.replace("--", "- -");

    QJSEngine engine;
    QJSValue result = engine.evaluate(jsExpression);

    if (result.isError() ||
        !result.isNumber() ||
        !std::isfinite(result.toNumber())) {
        return false;
    }

    number = result.toNumber();

    return true;
}