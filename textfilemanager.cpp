#include "textfilemanager.h"
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextBlock>
#include <QPrinter>
TextFileManager::TextFileManager()
{

}

TextFileManager::~TextFileManager(){


}

void TextFileManager::openFile(QString fileName, SubTextEdit *txtedit){

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        //QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    txtedit->clear();
    QTextStream in(&file);

    int counter = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        //identify fountain text's type.
        //create block according to fountain text
        QString lineType = identifyFountainLine(line);
        generateBlock(txtedit, line, counter, lineType);

        counter++;

        if (lineType=="character"){
            while (!(line == "" || line=="\n")){
                line = in.readLine();
                generateDialog(txtedit, line, counter);
                counter++;
            }
        }
    }

    file.close();
    QTextCursor cursor = txtedit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    txtedit->setTextCursor(cursor);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.removeSelectedText();

}

//identifies what type of line it is.
QString TextFileManager::identifyFountainLine(QString line){

    if (line==""){
        return "empty";
    }
    else if (line.at(0)== '>' || (line.length() >=3 && line.last(3)=="TO:")){
        return "transition";
    }
    else if (line.at(0)=='.'){
        return "header";
    }
    else if (line.isUpper()){
        if (line.length() >= 4 && (line.first(4)=="INT." || line.first(4)=="EXT.")){
            return "header";
        }
        else{
            return "character";
        }
    }
    else{
        return "action";
    }
    return "unidentifiable";

} //returns fountain line.

//generates a block based off of it's type.
void TextFileManager::generateBlock(SubTextEdit* textedit, QString line, int indx, QString type){
    QTextCursor cursor = textedit->textCursor();
    //QTextBlock block = (textedit->document()->findBlockByLineNumber(cursor.blockNumber()));

    QTextBlockFormat bform = textedit->document()->findBlockByLineNumber(indx).blockFormat();
    int fontSize = textedit->getFontSize();
    if (type=="empty"){
        return;
    }
    else if (type == "character"){
        bform.setLeftMargin(194);
    }
    else if (type == "header"){
        bform.setLeftMargin(36);
        bform.setBottomMargin(fontSize);
        cursor.insertBlock(bform);
        if (line.at(0)=='.'){
                line = line.mid(1);
            }
        textedit->insertPlainText(line.toUpper()); //return capitalized version
        return;
    }
    else if (type == "transition"){
        bform.setAlignment(Qt::AlignRight);
        bform.setLeftMargin(109);
        bform.setBottomMargin(fontSize);
        QString substring = line.mid(1);
        cursor.insertBlock(bform);
        textedit->insertPlainText(substring);
        return;

    }
    else if (type == "action"){
        bform.setLeftMargin(36);
    }

    bform.setBottomMargin(fontSize);
    cursor.insertBlock(bform);
    textedit->insertPlainText(line);
}
//genereate
//Generates either a dialog block or parentheses block.
void TextFileManager::generateDialog(SubTextEdit* textedit, QString line, int indx){
    QTextCursor cursor = textedit->textCursor();
    QTextBlockFormat bform = textedit->document()->findBlockByLineNumber(indx).blockFormat();
    if (line.length()==0){return;}
    if (line.at(0) == '('){
        bform.setLeftMargin(SubTextEdit::Parenth);
        bform.setRightMargin(SubTextEdit::Parenth);
    }
    else{
        bform.setLeftMargin(SubTextEdit::Dialog);
        bform.setRightMargin(SubTextEdit::Dialog);
    }
    bform.setBottomMargin(textedit->getFontSize());
    QTextBlock prevBlock = textedit->document()->findBlockByLineNumber(indx).previous();
    QTextBlockFormat prevb = prevBlock.blockFormat();
    prevb.setBottomMargin(0);

    cursor.setBlockFormat(prevb); //cursor is currently on previous block, so just update so bottom margin is 0.
    cursor.insertBlock(bform); //insert new block now.

    textedit->insertPlainText(line);
}

//savefile.
void TextFileManager::saveFile(QString fileName, SubTextEdit *txtedit){

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    QTextBlock end = txtedit->document()->end();

    //ITERATE THROUGH THE BLOCKS and convert them into plaintext and put them into the .fountain file.
    for (QTextBlock blox = txtedit->document()->begin(); blox != end; blox = blox.next()){
        int ltmargin = blox.blockFormat().leftMargin();
        out << blockToFountain(blox, ltmargin);
    }
    file.close();
}

//This function converts a QTextBlock into fountain-syntax plaintext. Used in Saving files.
QString TextFileManager::blockToFountain(QTextBlock block, int leftMargin){
    QString output = "";
    if (block.text().length()==0){
        return "";
    }

    if (!(leftMargin==(108)||leftMargin==(115))){ //lmargin==dialog or parentheses
       output +="\n\n";
    } //only print new lines if its not dialog or parentheses.
    else{
        output +="\n";
    }
    if (block.text().isUpper() && leftMargin==(109)){
        output+= ">";
    }

    if (block.text().isUpper() && leftMargin==(36) ){
        output+= ".";
    }

    output += block.text();
    return output;

}

void TextFileManager::exportAsPDF(SubTextEdit* txtedit){
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Export PDF", QString(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::Letter);
    //printer.setPageMargins(QMarginsF(1.5, 1, 1, 1), QPageLayout::Inch);
    printer.setOutputFileName(fileName);

    txtedit->toHtml();
    txtedit->document()->setPageSize(printer.pageLayout().paintRectPixels(printer.resolution()).size());
    txtedit->document()->print(&printer);
}
