#include "textedit.h"
#include <QSizePolicy>


textEdit::textEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

textEdit::textEdit(QString text, QWidget *parent)
    : QLineEdit(text, parent)
{

}

void textEdit::customStyleSheet() {
    setStyleSheet("background-color: rgba(0, 0, 0, 0); boarder: none");
}

void textEdit::mousePressEvent(QMouseEvent *event) {
    emit clicked();
    QLineEdit::mousePressEvent(event);
}

void textEdit::mouseDoubleClickEvent(QMouseEvent *event) {
    emit doubleClick();
    QLineEdit::mouseDoubleClickEvent(event);
}
