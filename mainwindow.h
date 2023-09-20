#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "subtextedit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *) override;
    Ui::MainWindow getUi();

    void onRemoveWidget();
    void addWidget(QWidget*);

private slots:


    void on_actionSave_as_triggered();

    void on_actionExport_as_PDF_triggered();

    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;
    SubTextEdit *txtedit;

};
#endif // MAINWINDOW_H
