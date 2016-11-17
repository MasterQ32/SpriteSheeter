#ifndef ANIMATION_H
#define ANIMATION_H
#include <QObject>
#include <QVector>
#include <QImage>
#include <QGraphicsScene>
#include <QMap>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QColor>
#include "BalancePos.h"

class Animation : public QObject
{
    Q_OBJECT

    Animation(){}

    QGraphicsScene* scene;
    QVector<QGraphicsPixmapItem*> images;               //Actual images for this animation
    QVector<QGraphicsRectItem*> frameBackgrounds;       //Background items for each frame
    QVector<QGraphicsRectItem*> selectedForegrounds;    //Foreground items for each selected frame
    QMap<QGraphicsPixmapItem*, QImage*> imageMap;
    QVector<bool> selected;
    int offsetX, offsetY;   //Position on the screen
    int spacingX, spacingY;
    int width;
    int curHeight;  //Last-calculated height for the animation
    QColor frameBgCol;
    bool frameBgTransparent;
    QImage* transparentBg;
    unsigned int minWidth;  //Minimum width for this animation at the current width

    unsigned int heightRecalc();    //Recalculate where each image is on in the sheet
    unsigned int widthOfImages();
public:
    explicit Animation(QImage* bg, QGraphicsScene* s, QObject *parent = 0);
    ~Animation();

    //Insert an image at the end of the animation and hand over control of the memory
    void insertImage(QImage* img);

    //Insert an image at the specified index and hand over control of the memory
    void insertImage(QImage* img, unsigned int index);

    //Insert a list of images at the end of the animation and hand over control of the memory
    void insertImages(QVector<QImage*> imgs);

    //Insert a list of images at the specified index and hand over control of the memory
    void insertImages(QVector<QImage*> imgs, unsigned int index);

    //Remove the given images from the given animation and add them to this one
    //Note the Qt syntax: otherIndices << 33 << 12 << 68 << 6 << 12;
    void pullImages(Animation* other, QList<unsigned int> otherIndices, unsigned int insertLocation);

    //Set the width of the animation. Return the new height
    unsigned int setWidth(unsigned int width);

    //Set the spacing between animations and frames
    void setSpacing(unsigned int x, unsigned int y);
    void setXSpacing(unsigned int x);
    void setYSpacing(unsigned int y);

    //Set the offset to draw this animation at
    void setOffset(unsigned int x, unsigned int y);

    //Set the background color for each frame
    void setFrameBgCol(QColor c);

    //Set the background transparency for each frame
    void setFrameBgTransparent(bool b);

    //Set the visibility of the frame bg (if both sheet and frame bgs are invisible, don't draw the latter)
    void setFrameBgVisible(bool b);

    //Get the last-calculated height for the animation
    unsigned int getCurHeight() {return curHeight;}

    //Reverse the animation
    void reverse();

    //Remove duplicate frames from the current animation (return true if duplicates found/removed)
    bool removeDuplicateFrames();

    //Get the largest width/height out of all animation frames
    QPoint getMaxFrameSize();

    //Balance sheet to the given size
    //TODO don't include BalanceSheetDialog just for this
    void balance(QPoint sz, BalancePos::Pos vert, BalancePos::Pos horiz);

    //Test if a point is inside the animation
    bool isInside(int x, int y);

    unsigned int getMinWidth() {return minWidth;}   //Get the minimum width for the current width
    unsigned int getSmallestImageWidth();           //Get the smallest possible width for this animation

    void toggleSelect(QGraphicsItem* it); //Select the given item as a frame

    bool deleteSelected();

signals:

public slots:

private:
};

#endif // ANIMATION_H
