#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QPushButton>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    // 1. 获取当前显示的字符串
    QString current = ui->displayEdit->text();

    // 2. 把数字转换为字符串
    QString number = QString::number(digit);

    // 3. 判断是否为初始状态
    if (current == "0") {
        ui->displayEdit->setText(number);
    }
    else {
        ui->displayEdit->setText(current + number);
    }
}
void MainWindow::inputDot()
{
    QString current = ui->displayEdit->text();

    // 如果已经存在小数点，不再添加
    if (current.contains(".")) {
        return;
    }

    // 添加小数点
    ui->displayEdit->setText(current + ".");
}
