#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QLineEdit>
#include <QWidget>

class textEdit : public QLineEdit
{
    Q_OBJECT
public:
    textEdit(QWidget *parent = nullptr);
    textEdit(QString text, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;

signals:
    void clicked();
    void doubleClick();

private:
    void customStyleSheet();
};

#endif // TEXTEDIT_H
