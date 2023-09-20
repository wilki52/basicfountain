#ifndef SUBTEXTEDIT_H
#define SUBTEXTEDIT_H
#include "QTextEdit"


class SubTextEdit: public QTextEdit
{

    Q_OBJECT
public:


    SubTextEdit();
    void keyPressEvent(QKeyEvent *) override;

    QString getCurrentFile();
    void setCurrentFile(QString fname);
    int getFontSize();
    void clear();

    enum TypeMargins {
        Action = 36,
        Character = 194,
        Dialog = 108,
        Parenth = 115,
        Transition = 109

    };
public slots:
           void onCursorChanged();
private:

    void setShortcuts();
    void generateBlockParantheses(QTextCursor cursor, int bot,int left, int right, int top);
    QTextBlockFormat generateFormat(int b, int l, int r, int t);

    int tabSetMargins[3];
    int enterSetMargins[3];
    int fontSize;
    QString state = "action";
    QString currentFile;

};

#endif // SUBTEXTEDIT_H
