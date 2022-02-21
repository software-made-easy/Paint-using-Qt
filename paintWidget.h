#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QtPrintSupport/QPrinter>


class paintWidget : public QWidget
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    paintWidget(QWidget *parent = nullptr);

    enum drawMode
    {
        Rubber,     ///< Erase drawed
        Normal,     ///< Normal drawing
        Auto,       ///< Automatic choose
    };

    // Handles all events
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setBackgroundColor(const QColor &color);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setPenMode(drawMode);

    // Has the image been modified since last save
    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }
    QColor backgroundColor = Qt::white;
    drawMode mode = Normal;

public slots:

    // Events to handle
    void clearImage();
    void print();
    void preview();

signals:
    void updateStatusBar();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // Updates the scribble area where we are painting
    void paintEvent(QPaintEvent *event) override;

    // Makes sure the area we are drawing on remains
    // as large as the widget
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);

    // Will be marked true or false depending on if
    // we have saved after a change
    bool modified;

    // Marked true or false depending on if the user
    // is drawing
    bool scribbling;

    // Holds the current pen width & color
    int myPenWidth;
    QColor myPenColor;
    QColor tempColor = myPenColor;

    // Stores the image being drawn
    QImage image;

    // Stores the location at the current mouse event
    QPoint lastPoint;

private slots:
    void _print(QPrinter* printer);
    void printPreview(QPrinter *printer);
};

#endif // PAINTWIDGET_H
