#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
#include <QJSEngine>
#include <QRegularExpression>
#include <cmath>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
void MainWindow::inputDigit(int digit)
{
    QString current = ui->displayEdit->text();

    QString number = QString::number(digit);

    if (current == "0") {
        ui->displayEdit->setText(number);
    }
    else {
        ui->displayEdit->setText(current + number);
    }
}
void MainWindow::inputOperator(const QString &op)
{
    QString current = ui->displayEdit->text();

    // 1. 如果输入框为空，不允许直接输入运算符
    if (current.isEmpty()) {
        return;
    }

    // 2. 定义允许的四种运算符
    QString operators = "+-×÷";

    // 3. 检查最后一个字符是不是运算符
    QString last = current.right(1);

    if (operators.contains(last)) {
        return;
    }

    // 4. 拼接运算符
    ui->displayEdit->setText(current + op);
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
    if (currentNumber.isEmpty()) {
        ui->displayEdit->setText(current + "0.");
    }
    else {
        ui->displayEdit->setText(current + ".");
    }

}void MainWindow::calculateResult()
{
    // 1. 获取显示框里的表达式
    QString expression = ui->displayEdit->text();

    // 2. 检查表达式是否合法
    QRegularExpression pattern(
        R"(^\d+(?:\.\d*)?(?:[+\-×÷]\d+(?:\.\d*)?)*$)"
        );

    if (!pattern.match(expression).hasMatch()) {
        ui->displayEdit->setText("错误");
        return;
    }

    // 3. 转换运算符
    expression.replace("×", "*");
    expression.replace("÷", "/");

    // 4. 创建 JavaScript 引擎
    QJSEngine engine;

    // 5. 计算表达式
    QJSValue result = engine.evaluate(expression);

    // 6. 检查是否发生计算错误
    if (result.isError() ||
        !result.isNumber() ||
        !std::isfinite(result.toNumber())) {

        ui->displayEdit->setText("错误");
        return;
    }

    // 7. 显示计算结果
    double number = result.toNumber();

    ui->displayEdit->setText(
        QString::number(number, 'g', 15)
        );
}
