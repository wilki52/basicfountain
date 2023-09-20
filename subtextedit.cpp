#include "subtextedit.h"
#include "QKeyEvent"
#include <iostream>
#include <QTextBlock>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>
//#include <QTextBlockFormat>
SubTextEdit::SubTextEdit()
{

    fontSize = 14;
    QTextDocument *doc = new QTextDocument;
    doc->setDefaultFont(QFont("Courier New", fontSize));
    doc->setDocumentMargin(72);
    this->setDocument(doc);
    this->setTabStopDistance(72);
    state = "action";
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorChanged()));

    QTextCursor cursor = this->textCursor();
    QTextBlockFormat bform;

    bform.setLeftMargin(SubTextEdit::Action);
    bform.setBottomMargin(fontSize);
    cursor.setBlockFormat(bform);

    onCursorChanged();

}
QString SubTextEdit::getCurrentFile(){
    return currentFile;
}
void SubTextEdit::setCurrentFile(QString fname){
    currentFile = fname;

}


int SubTextEdit::getFontSize(){
    return fontSize;
}

void SubTextEdit::setShortcuts(){
    QTextCursor cursor = this->textCursor();
    QTextDocument *doc = this->document();
    QTextBlock block = (doc->findBlockByLineNumber(cursor.blockNumber()));
    QString text = block.text();
    int leftmargin = block.blockFormat().leftMargin();
    tabSetMargins[1] = 0;
    tabSetMargins[2] = fontSize;

    enterSetMargins[1] = 0;
    enterSetMargins[2] = fontSize;


    switch (leftmargin){
        case SubTextEdit::Action:
            state = "action";
            tabSetMargins[0] = SubTextEdit::Character;
            tabSetMargins[2] = 0;

            enterSetMargins[0] = SubTextEdit::Action;
            break;

        case SubTextEdit::Dialog:
            state = "dialog";
            tabSetMargins[0] = SubTextEdit::Parenth;
            tabSetMargins[1] = SubTextEdit::Parenth;
            tabSetMargins[2] = 0;

            enterSetMargins[0] = SubTextEdit::Action;
            break;

        case SubTextEdit::Character:
            if (!block.text().isUpper()){
                text= block.text().toUpper();
                cursor.select(QTextCursor::LineUnderCursor);
                cursor.removeSelectedText();
                insertPlainText(text);
            }
            state = "character";
            tabSetMargins[0] = SubTextEdit::Dialog;
            tabSetMargins[1] = SubTextEdit::Dialog;
            tabSetMargins[2] = 0;

            if (block.text().length()==0){
                enterSetMargins[0]= SubTextEdit::Action;
            }
            else{
                enterSetMargins[0] = SubTextEdit::Dialog;
                enterSetMargins[1] = SubTextEdit::Dialog;
            }
            break;

        case SubTextEdit::Parenth:
            state = "parenth";
            tabSetMargins[0] = SubTextEdit::Dialog;
            tabSetMargins[1] = SubTextEdit::Dialog;

            enterSetMargins[0] = SubTextEdit::Dialog;
            enterSetMargins[1] = SubTextEdit::Dialog;
            break;

        case SubTextEdit::Transition:
            state = "transition";
            tabSetMargins[0] = SubTextEdit::Action;
            enterSetMargins[0] = SubTextEdit::Action;
            break;

        }

}
void SubTextEdit::onCursorChanged(){
    setShortcuts();

}

void SubTextEdit::generateBlockParantheses(QTextCursor cursor, int bot,int left, int right, int top){
    QString blockText = document()->findBlockByLineNumber(cursor.blockNumber()).text();

    QTextBlockFormat bform = generateFormat(bot, left, right, top);

    if (blockText.length() >0){
            //creates block format which sets current block's bottom margin to 0.
            QTextBlockFormat tempformat = generateFormat(0,SubTextEdit::Dialog, SubTextEdit::Dialog, 0);
            cursor.setBlockFormat(tempformat);


            cursor.insertBlock(bform);
    }
    else{
            //it is empty, so just set the margins here.
            cursor.setBlockFormat(bform);
    }
    insertPlainText("()");
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
    setTextCursor(cursor);
}
QTextBlockFormat SubTextEdit::generateFormat(int b, int l, int r, int t){
    QTextBlockFormat format;
    format.setBottomMargin(b);
    format.setLeftMargin(l);
    format.setRightMargin(r);
    format.setTopMargin(t);
    return format;
}

//overrides keypressevent of parent class. Gets called after every input.
void SubTextEdit::keyPressEvent(QKeyEvent *event){


    if (event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QTextCursor cursor = this->textCursor();
        QTextBlock block = (document()->findBlockByLineNumber(cursor.blockNumber()));
        QTextBlockFormat bform;

        if (keyEvent->key() == Qt::Key_Tab){
            //Set margins for the new block.
       \
            bform = generateFormat(tabSetMargins[2], tabSetMargins[0], tabSetMargins[1], 0);
            //RANDOM EXCEPTIONS HERE.
            if (state == "dialog"){
                //TURN TO PARANTHESES
                generateBlockParantheses(cursor, tabSetMargins[2], tabSetMargins[0], tabSetMargins[1], 0);
                return;
            }
            else if (state == "parenth"){

                //TURN TO DIALOG; //MOVE CURSOR TO END OF THE BLOCK. This is so when a block is inserted, we won't clip parentheses.
                moveCursor(QTextCursor::EndOfBlock);
                cursor = this->textCursor();
            }
            else if (state == "character"){
                if (block.text() ==""){

                    bform = generateFormat(fontSize, SubTextEdit::Transition, 36, 0);
                    bform.setAlignment(Qt::AlignRight);
                }
                else{
                    bform = generateFormat(fontSize, SubTextEdit::Dialog, SubTextEdit::Dialog, 0);
                }
            }

            //BLOCK CREATION.
            //if its empty, no point in creating a new block.
            if (block.text()==""){
                cursor.setBlockFormat(bform);
            }
            else{
                cursor.insertBlock(bform);
            }

        }

            //IF RETURN KEY IS PRESSED.
        else if (keyEvent->key() == Qt::Key_Return){
            if (state=="parenth"){
                //TRANSITIONS TO DIALOG if press return.
                    //Just skips to the end of th eblock.
                moveCursor(QTextCursor::EndOfBlock);
                cursor = this->textCursor();
            }
            //generate.
            cursor.insertBlock(generateFormat(enterSetMargins[2],enterSetMargins[0],enterSetMargins[1], 0));

        }

        else if (keyEvent->key() == Qt::Key_Backspace){
            if (block.text().isEmpty()){
                cursor.setBlockFormat(generateFormat(fontSize, SubTextEdit::Action, 0, 0));
            }
            QTextEdit::keyPressEvent(event);
        }
        else{
            QTextEdit::keyPressEvent(event);
        }
    }
    //CALL IT AGAIN AT THE END TO REFRESH.
    setShortcuts();
}

void SubTextEdit::clear(){
    setPlainText("");

}
