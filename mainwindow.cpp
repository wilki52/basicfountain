#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QKeyEvent"
#include "subtextedit.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
//#include <QPoint>
#include <QTextBlock>
#include "textfilemanager.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->verticalLayout->addStretch();
    SubTextEdit *text = new SubTextEdit;

    text->isEnabled();


    text->setFixedSize(QSize(600,600));

    qDebug() <<"height: "<< widthMM() << " " << heightMM();

    addWidget(text);
    txtedit = text;


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    }


}

Ui::MainWindow MainWindow::getUi(){
    return *ui;
}

void MainWindow::onRemoveWidget(){
}
void MainWindow::addWidget(QWidget* widget){
   QVBoxLayout* vlayout = qobject_cast<QVBoxLayout*>(ui->verticalLayout->layout());

    vlayout->addWidget(widget);
}



void MainWindow::on_actionSave_as_triggered()
{
    QFileDialog dia;
    QString fileName = dia.getSaveFileName(this, "Save as...");
    fileName = fileName+".fountain";

    TextFileManager* fileManager = new TextFileManager;

    fileManager->saveFile(fileName, txtedit);

}


void MainWindow::on_actionExport_as_PDF_triggered()
{
    TextFileManager *fmanager= new TextFileManager;
    fmanager->exportAsPDF(txtedit);
}


void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open .fountain file");

    TextFileManager *fmanager= new TextFileManager;
    fmanager->openFile(fileName, txtedit);
    setWindowTitle(fileName);
}

