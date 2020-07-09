#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QMessageBox"
#include "QStandardPaths"
#include "object_code_generate.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileName = "";
    quaternion = false;
    opt_quaternion = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openFile_clicked()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    fileName = QFileDialog::getOpenFileName(nullptr, "选择源码", dir, "text files(*.txt)");
    if (!fileName.isEmpty()) {
        ui->filePath->setText(fileName);
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "错误", "无法打开文件");
        } else {
            QString content;
            auto tmpContent = file.readAll();
            content.append(tmpContent);
            ui->fileContent->setText(content);
        }
        file.close();
        ui->openFile->setDisabled(true);
    }
}

void MainWindow::on_quaternion_clicked()
{
    if (fileName.isEmpty()) {
        QMessageBox::critical(this, "错误", "请先打开一个文件");
    } else {
        grammar gram(fileName.toStdString());
        gram.begin();
        string errorInfo = gram.errorManage();
        if (errorInfo != "") {
            QMessageBox::critical(this, "错误", QString::fromStdString(errorInfo));
            exit(1);
        }
        auto result = QString::fromStdString(gram.print_quaternion_list());
        ui->fileContent->setText(result);
        quaternion = true;
        ui->quaternion->setDisabled(true);
    }
}

void MainWindow::on_opt_quaternion_clicked()
{
    if (!quaternion) {
        QMessageBox::critical(this, "错误", "请先生成四元式");
    } else {
        divide();
        ui->fileContent->setText(QString::fromStdString(display()));
        opt_quaternion = true;
        ui->opt_quaternion->setDisabled(true);
    }
}

void MainWindow::on_code_clicked()
{
    if (!opt_quaternion) {
        QMessageBox::critical(this, "错误", "请先生成优化后的四元式");
    } else {
        fcode* fcode_test = new fcode();
        fcode_test->to_code();
        ui->fileContent->setText(QString::fromStdString(fcode_test->print_fcode_array()));
        delete fcode_test;
        ui->code->setDisabled(true);
    }
}

void MainWindow::on_saveFile_clicked()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString temp = ui->fileContent->toPlainText();
    QString filePath = QFileDialog::getSaveFileName(this, "save as", dir, "text files(*.txt)");
    if (!filePath.isEmpty()){
        QFile file(filePath);
        if (file.exists()) {
            QMessageBox::critical(this, "错误", "文件已存在");
        } else {
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            file.write(temp.toLatin1());
            QMessageBox::information(this, "成功", "保存文件成功");
        }
        file.close();
    }
}
