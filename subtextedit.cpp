#include "subtextedit.h"
#include "QKeyEvent"
#include <iostream>
#include <QTextBlock>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>
//#include <QTextBlockFormat>
SubTextEdit::SubTextEdit() : state("action"), fontSize(14)
{

    //Set up a QTextDocument for our SubTextEdit class
    QTextDocument *doc = new QTextDocument;
    doc->setDefaultFont(QFont("Courier New", fontSize));
    doc->setDocumentMargin(72);
    this->setDocument(doc);

    //depreciated
    //connects signal cursorPositionChanged() to our custom slot function onCursorChanged() (can find in header file as well)
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorChanged()));

    QTextCursor cursor = this->textCursor();
    cursor.setBlockFormat(generateFormat(fontSize, SubTextEdit::Action, 0, 0));
}
QString SubTextEdit::getCurrentFile(){
    return currentFile;
}
void SubTextEdit::setCurrentFile(QString fname){
    currentFile = fname;
}

//overrides keypressevent of parent class. Gets called after every input.
void SubTextEdit::keyPressEvent(QKeyEvent *event){


    if (event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QTextCursor cursor = this->textCursor();
        QTextBlock block = (document()->findBlockByLineNumber(cursor.blockNumber()));
        QString text = block.text();
        int leftmargin = block.blockFormat().leftMargin();

        //if margin is 0, something is not right. so it fixes itself. checker.
        if (leftmargin == 0){
            cursor.setBlockFormat(generateFormat(fontSize, SubTextEdit::Action, 0, 0));
        }


        //If you press tab, insert a new text block.
        //In some cases, if text is empty, then change configs of current block instead of creating new one.
        if (keyEvent->key() == Qt::Key_Tab){

            //Check the block's type by comparing it's left margin size.
            switch (leftmargin){
            case (SubTextEdit::Action): //if currently action block, create a new character block.
                if (text.isEmpty()){
                    qDebug() << "hllo dutch";
                    cursor.setBlockFormat(generateFormat(0, SubTextEdit::Character, 0, 0));
                    break;
                }

                createCharacterBlock(cursor);
                break;

                //if chara block, create transition or dialog block depending on text
            case (SubTextEdit::Character):
                if (text.isEmpty()){
                    createTransitionBlock(cursor);
                }
                else{
                    createDialogBlock(cursor);
                }
                break;

            case (SubTextEdit::Dialog): //if dialog block, create parantheses block.
                //shared function
                createParenthesesBlock(cursor);
                break;
            case (SubTextEdit::Parenth):
                //go to end of block, then
                moveCursor(QTextCursor::EndOfBlock);
                cursor = this->textCursor();

                //create block.
                createDialogBlock(cursor);
                break;
            case (SubTextEdit::Transition):
                if (text.isEmpty()){
                    cursor.setBlockFormat(generateFormat(fontSize, SubTextEdit::Action, 0, 0));
                }
                else{
                    createActionBlock(cursor);
                }

                break;
            }
        }
        else if (keyEvent->key() == Qt::Key_Return){ //if press enter key.
            switch (leftmargin){
            case (SubTextEdit::Action): //if currently on action block, create another action block.
                createActionBlock(cursor);
                break;
            case (SubTextEdit::Character): //if character, create an action or dialog block. depends on text.
                if (text.isEmpty()){
                    createActionBlock(cursor);
                }
                else{
                    createDialogBlock(cursor);
                }
                break;
            case (SubTextEdit::Dialog): //if dialog, create an action block.
                createActionBlock(cursor);

                break;
            case (SubTextEdit::Parenth): //if parentheses, create dialog block

                //move cursor past the parentheses AKA end of block.
                moveCursor(QTextCursor::EndOfBlock);
                cursor = this->textCursor();
                //create block.
                createDialogBlock(cursor);
                break;
            case (SubTextEdit::Transition):
                //insert action.
                qDebug() << "enter to action";
                createActionBlock(cursor);
                break;
            }
        }
        else if (keyEvent->key() == Qt::Key_Backspace){ //if backspace
            //if text is empty and you hit backspace, go return to action block format.
            if (text.isEmpty()){
                cursor.setBlockFormat(generateFormat(fontSize, SubTextEdit::Action, 0, 0));
            }
            QTextEdit::keyPressEvent(event);
        }
        else{ //emulate regular keypresses. uses parent class's keypressevent(). parent: QTextEdit widget
            QTextEdit::keyPressEvent(event);

            //if block is in character mode, capitalize everything.
            if (leftmargin == SubTextEdit::Character && !block.text().isUpper()){
                text = block.text().toUpper();
                cursor.select(QTextCursor::LineUnderCursor);
                cursor.removeSelectedText();
                insertPlainText(text);
            }
        }
    }
}

int SubTextEdit::getFontSize(){
    return fontSize;
}
//depreciated. but this slot is still connected react to any cursor changes, but its not supposed to do anything.
void SubTextEdit::onCursorChanged(){
}

void SubTextEdit::createActionBlock(QTextCursor cursor){
    QTextBlockFormat bform = generateFormat(fontSize, SubTextEdit::Action, 0, 0);
    cursor.insertBlock(bform);
}
void SubTextEdit::createCharacterBlock(QTextCursor cursor){
    QTextBlockFormat bform = generateFormat(0, SubTextEdit::Character, 0, 0);
    cursor.insertBlock(bform);
    }
void SubTextEdit::createDialogBlock(QTextCursor cursor){
    QTextBlockFormat bform = generateFormat(fontSize, SubTextEdit::Dialog, SubTextEdit::Dialog, 0);
    cursor.insertBlock(bform);
}
void  SubTextEdit::createParenthesesBlock(QTextCursor cursor){
    QString blockText = document()->findBlockByLineNumber(cursor.blockNumber()).text();
    QTextBlockFormat bForm = generateFormat(0, SubTextEdit::Parenth, SubTextEdit::Parenth, 0);

    if (blockText.length() > 0){
            //ADJUST spacing between dialog and parantheses blocks.
            //tempformat is to recorrect the bottom margin of the current block before inserting our parentheses block.
            //this is so the spacing is not weird.
            QTextBlockFormat tempformat = generateFormat(0,SubTextEdit::Dialog, SubTextEdit::Dialog, 0);
            cursor.setBlockFormat(tempformat);


            cursor.insertBlock(bForm);
    }
    else{
        cursor.setBlockFormat(bForm);
    }
    insertPlainText("()");
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
    setTextCursor(cursor);

}
void  SubTextEdit::createTransitionBlock(QTextCursor cursor){
    QTextBlockFormat bForm = generateFormat(0, SubTextEdit::Transition, SubTextEdit::Action, 0);
    bForm.setAlignment(Qt::AlignRight);
    cursor.insertBlock(bForm);
}

QTextBlockFormat SubTextEdit::generateFormat(int b, int l, int r, int t){
    QTextBlockFormat format;
    format.setBottomMargin(b);
    format.setLeftMargin(l);
    format.setRightMargin(r);
    format.setTopMargin(t);
    return format;
}

/*
void onTabPressed(QTextCursor cursor, QString text, int leftmargin){

}
*/


//clears our text. used in textfilemanager class's opening a file function.
//Probably not the best practice lol
void SubTextEdit::clear(){
    setPlainText("");
}
