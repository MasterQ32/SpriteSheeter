#include "DragDropStep.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "QDebug"

DragDropStep::DragDropStep(MainWindow* w, int x, int y) : UndoStep(w)
{
    
    animOverIdx = w->getSheet()->getOver(x, y);
    origDropLocation = w->getSheet()->getAnimation(animOverIdx)->getDropPos(x, y);
    animAddedTo = animCreated = -1;
}

DragDropStep::~DragDropStep()
{
    clear();
}

void DragDropStep::undo()
{
    Sheet* sheet = mainWindow->getSheet();
    //See if we created a new anim
    if(animCreated >= 0)
        sheet->removeAnimation(animCreated);
    else
    {
        Animation* anim = sheet->getAnimation(animAddedTo);
        //Remove added frames
        for(int i = 0; i < movedFrames.size(); i++)
            anim->removeFrame(newDropLocation);
    }

    //Loop through reverse, adding back in deleted animations
    for(int i = deletedAnimations.size() - 1; i >= 0; i--)
    {
        int deleted = deletedAnimations.at(i);
        Animation* anim = new Animation(sheet->getTransparentBg(), sheet->getScene());
        sheet->addAnimation(anim, deleted);
    }

    //Loop through reverse, adding back in deleted frames
    for(int j = movedFrames.size() - 1; j >= 0; j--)
    {
        qDebug() << j << movedFrames.size() << endl;
        FrameLoc fl = movedFrames.at(j);
        Animation* a = sheet->getAnimation(fl.anim);
        a->insertImage(new QImage(fl.img->copy()), fl.frame);
        if(fl.selected)
            a->getFrame(fl.frame)->selectToggle();
    }
    //Recalculate sheet positions
    sheet->refresh();
    mainWindow->updateSelectedAnim();
}

void DragDropStep::redo()
{
    clear();    //Wipe clean if we've done this before
    newDropLocation = origDropLocation;
    Sheet* sheet = mainWindow->getSheet();
    QVector<Animation*>* animations = sheet->getAnimationPtr();
    //Pull frames from animations
    Animation* over = animations->at(animOverIdx);
    int curAnim = 0;
    foreach(Animation* anim, *animations)
    {
        QVector<FrameLoc> frames = pullSelected(anim, (anim == over)?(&newDropLocation):(NULL));
        foreach(FrameLoc f, frames)
        {
            f.anim = curAnim;
            movedFrames.append(f);
        }
        curAnim++;
    }

    //Figure out what to do with them
    if(newDropLocation >= 0)                   //Add to this anim
    {
        QVector<QImage*> pulledImages = getPulledImages();
        over->insertImages(pulledImages, newDropLocation);
        selectFrames(over, newDropLocation, pulledImages.size());
        animAddedTo = animOverIdx;
        animCreated = -1;
    }
    else if(newDropLocation == ANIM_BEFORE)    //Add before this anim
    {
        Animation* anim = new Animation(sheet->getTransparentBg(), sheet->getScene());
        QVector<QImage*> pulledImages = getPulledImages();
        anim->insertImages(pulledImages, 0);
        selectFrames(anim, 0, pulledImages.size());
        sheet->addAnimation(anim, animOverIdx);
        animAddedTo = animCreated = animOverIdx;
    }
    else if(newDropLocation == ANIM_AFTER)     //Add after this anim
    {
        Animation* anim = new Animation(sheet->getTransparentBg(), sheet->getScene());
        QVector<QImage*> pulledImages = getPulledImages();
        anim->insertImages(pulledImages, 0);
        selectFrames(anim, 0, pulledImages.size());
        sheet->addAnimation(anim, animOverIdx + 1);
        animAddedTo = animCreated = animOverIdx+1;
    }

    //Delete animations that are empty as a result of this
    deletedAnimations = sheet->deleteEmpty();

    //Recalculate sheet positions
    sheet->refresh();
    mainWindow->updateSelectedAnim();
}

QVector<DragDropStep::FrameLoc> DragDropStep::pullSelected(Animation* anim, int* pullLoc)
{
    QVector<FrameLoc> imgList;

    QVector<Frame*>* frames = anim->getFramePtr();
    for(int i = 0; i < frames->size(); i++)
    {
        Frame* f = frames->at(i);
        if(f->isSelected())
        {
            if(pullLoc != NULL && *pullLoc > i)
                (*pullLoc)--;
            FrameLoc fl;
            fl.frame = i;
            fl.img = new QImage(f->getImage()->copy());
            fl.selected = f->isSelected();
            imgList.append(fl);
            frames->remove(i);
            i--;
            delete f;
        }
    }

    return imgList;
}

QVector<QImage*> DragDropStep::getPulledImages()
{
    QVector<QImage*> imgs;
    foreach(FrameLoc f, movedFrames)
        imgs.append(new QImage(f.img->copy()));
    return imgs;
}

void DragDropStep::selectFrames(Animation* anim, int loc, int count)
{
    for(int i = 0; i < count; i++)
    {
        int animLoc = i + loc;
        if(movedFrames.at(i).selected)
            anim->getFrame(animLoc)->selectToggle();
    }
}

void DragDropStep::clear()
{
    foreach(FrameLoc f, movedFrames)
        delete f.img;
    movedFrames.clear();
    deletedAnimations.clear();
}
