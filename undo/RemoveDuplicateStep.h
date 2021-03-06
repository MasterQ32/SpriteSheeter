#ifndef REMOVEDUPLICATESTEP_H
#define REMOVEDUPLICATESTEP_H
#include "UndoStep.h"
#include <QVector>
#include <QImage>

class Animation;

class RemoveDuplicateStep : public UndoStep
{
    struct Loc {
        int pos;
        bool selected;
        QImage img;
    };

    int prevW;  //Previous sheet width
    int idx;
    QVector<Loc> removedImages;
public:
    RemoveDuplicateStep(MainWindow* w, int animIndex);

    void undo();
    void redo();
    bool isDifferent() {return true;}   //Could calculate, but not really worth
};

#endif // REMOVEDUPLICATESTEP_H
