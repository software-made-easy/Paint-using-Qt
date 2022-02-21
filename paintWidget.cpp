#include <QtWidgets>
#include <QStandardPaths>

#include <QtPrintSupport/qtprintsupportglobal.h>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>

#include "paintWidget.h"


paintWidget::paintWidget(QWidget *parent)
    : QWidget(parent)
{
    // Roots the widget to the top left even if resized
    setAttribute(Qt::WA_StaticContents);

    // accept drag and drop
    setAcceptDrops(true);

    // Set defaults for the monitored variables
    modified = false;
    scribbling = false;
    myPenWidth = 1;
    myPenColor = Qt::blue;
}

// Used to load the image and place it in the widget
bool paintWidget::openImage(const QString &fileName)
{
    // Holds the image
    QImage loadedImage;

    // If the image wasn't loaded leave this function
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}

// Save the current image
bool paintWidget::saveImage(const QString &fileName, const char *fileFormat)
{
    // Created to hold the image
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}

// used to change background color
void paintWidget::setBackgroundColor(const QColor &color) {
    // FIXME: Not workking
    /*QImage newImage(image.size(), QImage::Format_RGB32);
    newImage.fill(color);
    QPainter p(&newImage);
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    p.drawImage(0, 0, image);
    QSize newSize = newImage.size().expandedTo(size());
    resizeImage(&newImage, newSize);
    image = newImage;*/
    setStyleSheet(QString("background-color: %1").arg(color.name()));
    modified = false;
    update();
    backgroundColor = color;
}

// Used to change the pen color
void paintWidget::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}

// Used to change the pen width
void paintWidget::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}

// pen mode
void paintWidget::setPenMode(drawMode newMode) {
    if (newMode == Auto) {
        if (mode == Normal) {
            setPenMode(Rubber);
        }
        else {
            setPenMode(Normal);
        }
    }

    if (newMode == Rubber && mode != Rubber) {
        tempColor = myPenColor;
        myPenColor = backgroundColor;
        mode = Rubber;
    }
    else if (newMode == Normal && mode != Normal) {
        myPenColor = tempColor;
        mode = Normal;
    }
}

// Color the image area with white
void paintWidget::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}

// If a mouse button is pressed check if it was the
// left button and if so store the current position
// Set that we are currently drawing
void paintWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true;
    }
}

// When the mouse moves if the left button is clicked
// we call the drawline function which draws a line
// from the last position to the current
void paintWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && scribbling)
        drawLineTo(event->pos());
}

// If the button is released we set variables to stop drawing
void paintWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
        scribbling = false;
    }
}

// QPainter provides functions to draw on the widget
// The QPaintEvent is sent to widgets that need to
// update themselves
void paintWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Returns the rectangle that needs to be updated
    QRect dirtyRect = event->rect();

    // Draws the rectangle where the image needs to
    // be updated
    painter.drawImage(dirtyRect, image, dirtyRect);
}

// Resize the image to slightly larger then the main window
// to cut down on the need to resize the image
void paintWidget::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
    emit updateStatusBar();
}

void paintWidget::dragEnterEvent(QDragEnterEvent *event) {
    QWidget::dragEnterEvent(event);
}

void paintWidget::dragMoveEvent(QDragMoveEvent *event) {
    QWidget::dragMoveEvent(event);
}

void paintWidget::dropEvent(QDropEvent *event) {
    QWidget::dropEvent(event);
}

void paintWidget::drawLineTo(const QPoint &endPoint)
{
    // Used to draw on the widget
    QPainter painter(&image);

    // Set the current settings for the pen
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    // Draw a line from the last registered point to the current
    painter.drawLine(lastPoint, endPoint);

    // Set that the image hasn't been saved
    modified = true;

    int rad = (myPenWidth / 2) + 2;

    // Call to update the rectangular space where we drew
    update(QRect(lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));

    // Update the last position where we left off drawing
    lastPoint = endPoint;
}

// When the app is resized create a new image using
// the changes made to the image
void paintWidget::resizeImage(QImage *image, const QSize &newSize)
{
    // Check if we need to redraw the image
    if (image->size() == newSize)
        return;

    // Create a new image to display and fill it with white
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));

    // Draw the image
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

// print preview
void paintWidget::preview() {
    QPrinter printer(QPrinter::HighResolution);
    // Open preview dialog and print if asked
    QPrintPreviewDialog dialog(&printer, this);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &paintWidget::printPreview);
    dialog.exec();
}

// Print the image
void paintWidget::_print(QPrinter *printer)
{
    // Open printer dialog and print if asked
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QPainter painter(printer);
        QRect rect = painter.viewport();
        QSize size = image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(image.rect());
        painter.drawImage(0, 0, image);
    }
}

// create printer
void paintWidget::print() {
    QPrinter printer(QPrinter::HighResolution);
    _print(&printer);
}

void paintWidget::printPreview(QPrinter *printer)
{
    QPainter painter(printer);
    QRect rect = painter.viewport();
    QSize size = this->size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow(this->rect());
    painter.drawImage(0, 0, image);
    render(&painter);
}
