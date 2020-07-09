#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "object_code_generate.h"
#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_openFile_clicked();

    void on_quaternion_clicked();

    void on_opt_quaternion_clicked();

    void on_code_clicked();

    void on_saveFile_clicked();

private:
    Ui::MainWindow* ui;
    QString fileName;
    bool quaternion;
    bool opt_quaternion;
};
#endif // MAINWINDOW_H
