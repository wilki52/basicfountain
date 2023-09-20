#ifndef TEXTFILEMANAGER_H
#define TEXTFILEMANAGER_H

#include "subtextedit.h"

class TextFileManager
{
public:
    TextFileManager();
    ~TextFileManager();
    //for opening
    //void fountainToBlock();

    void exportAsPDF(SubTextEdit*);
    QString blockToFountain(QTextBlock, int leftmargin);

    void openFile(QString, SubTextEdit*);
    void saveFile(QString, SubTextEdit*);
    void setTextEditor();



private:
    //SubTextEdit textedit;

    QString identifyFountainLine(QString line); //returns fountain line.
    void generateBlock(SubTextEdit*, QString line, int indx, QString type);
    void generateDialog(SubTextEdit*, QString line, int indx);

};

#endif // TEXTFILEMANAGER_H
