#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QColorDialog>
#include "ImportDialog.h"
#include "BalanceSheetDialog.h"
#include "IconExportDialog.h"
#include "SheetEditorView.h"
#include "RecentDocuments.h"
#include "Animation.h"
#include "Sheet.h"
#include "BalancePos.h"
#include "undo/UndoStep.h"
#include <QStack>
#include <QTextStream>
#include <QProgressDialog>

#define DRAG_HANDLE_SIZE 10

#define MAJOR_VERSION 1
#define MINOR_VERSION 2
#define REV_VERSION   0

#define CURSOR_SZ 64

#define UNTITLED_IMAGE_STR "Untitled"

enum WrapType {
    WRAP,
    PINGPONG,
    PINGPONG_BACK,
    STOP
};

class ZoomableGraphicsView;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent* e);
    Ui::MainWindow* getUI() {return ui;}
    Sheet* getSheet() {return sheet;}
    void drawAnimation();
    void updateSelectedAnim(bool updateName = true);
    void minimizeSheetWidth();  //Minimize the sheet's width against the anim lines currently on it
    int mAnimFrame;
    bool userEditingWidth;
    void checkMinWidth();   //Check the sheet's current width against the min possible width & update as needed

signals:
    bool setImportImg(QImage image);
    void setBalanceDefWH(int w, int h);
    void setIconImage(QImage img);

public slots:
    void importNext(QImage img, int numx, int numy, bool bVert, bool bSplit);
    void importAll(QImage img, int numx, int numy, bool bVert, bool bSplit);
    void animUpdate();
    void mouseCursorPos(int x, int y);
    void mouseDown(int x, int y);
    void mouseUp(int x, int y);
    void newFile();
    void saveFileAs();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void saveSheet(QString filename = QString());
    void loadSheet(QString openFilename = QString());
    void addImages(QStringList l);
    void addFolders(QStringList l);
    void balance(int w, int h, BalancePos::Pos vert, BalancePos::Pos horiz);

    void startedBatchRender(QString sheetName);
    void finishedBatchRender();
    void threadRenderingCanceled();

private slots:
    void on_openImagesButton_clicked();
    void on_xSpacingBox_valueChanged(int arg1);
    void on_ySpacingBox_valueChanged(int arg1);
    void on_removeAnimButton_clicked();
    void on_animationNameEditor_textChanged(const QString &arg1);
    void on_animPlayButton_clicked();
    void on_animStopButton_clicked();
    void on_animPrevFrameButton_clicked();
    void on_animNextFrameButton_clicked();
    void on_animationSpeedSpinbox_valueChanged(int arg1);
    void on_openStripButton_clicked();
    void on_sheetWidthBox_valueChanged(int arg1);
    void on_saveFrameButton_clicked();
    void on_frameBgColSelect_clicked();
    void on_sheetBgColSelect_clicked();
    void on_frameBgTransparent_toggled(bool checked);
    void on_sheetBgTransparent_toggled(bool checked);
    void on_balanceAnimButton_clicked();
    void on_fontButton_clicked();
    void on_xSpacingBox_editingFinished();
    void on_ySpacingBox_editingFinished();
    void on_sheetWidthBox_editingFinished();
    void on_animationNameEditor_editingFinished();
    void on_animNameEnabled_toggled(bool checked);
    void on_exportAnimButton_clicked();
    void on_reverseAnimButton_clicked();
    void on_removeDuplicateFramesButton_clicked();
    void on_actionAbout_triggered();
    void on_actionBatch_Processing_triggered();
    void on_fontColSelect_clicked();
    void on_minWidthCheckbox_toggled(bool checked);
    void on_newButton_clicked();
    void on_openButton_clicked();
    void on_saveButton_clicked();
    void on_cutButton_clicked();
    void on_copyButton_clicked();
    void on_pasteButton_clicked();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_exportButton_clicked();
    void on_radioButton_wrap_toggled(bool checked);
    void on_radioButton_pingPong_toggled(bool checked);
    void on_radioButton_stop_toggled(bool checked);

private:
    Ui::MainWindow*         ui;
    ZoomableGraphicsView*   mSheetZoom;
    ZoomableGraphicsView*   mAnimationZoom;
    ImportDialog*           mImportWindow;
    BalanceSheetDialog*     mBalanceWindow;
    IconExportDialog*       mIconExportWindow;
    RecentDocuments*        mRecentDocuments;
    QStringList             mOpenFiles;
    QString                 curImportImage;
    QString                 lastSaveStr;
    QString                 lastImportExportStr;

    //Animation variables
    Sheet* sheet;
    WrapType animationWrap;

    QGraphicsItem* clicked; //Last scene item the user clicked on
    QGraphicsItem* selected; //Last scene item the user clicked on that was currently selected
    QGraphicsItem* lastSelected;    //Last item that was clicked (persists after clicking stops)
    QGraphicsRectItem* curSelectedRect; //Rect for showing hovered anim frame
    QGraphicsLineItem* curDragLine;
    QImage mCurFrame;
    QGraphicsRectItem* curSelectedAnimRect;

    //Variables for drawing the current sheet/animation
    bool m_bDraggingSelected;
    bool m_bSetDraggingCursor;
    QRect m_rLastDragHighlight;
    bool m_bLastDragInAnim;
    QImage transparentBg;
    QColor animHighlightCol;

    //Undo/redo variables
    QStack<UndoStep*> undoStack;
    QStack<UndoStep*> redoStack;
    bool bStackBottomSaved;
    int lastXSpacing, lastYSpacing, lastSheetW;
    int wEditing;
    QString lastAnimName;

    QString lastIconStr;
    QString lastOpenDir;
    QString lastGIFStr;

    QColorDialog colorSelect;

    int curMouseX;
    int curMouseY;

    QString sCurFilename;

    //Variables for dealing with the Qt draw engine
    QGraphicsScene* animScene;
    QGraphicsPixmapItem* animItem;
    QGraphicsScene* msheetScene;

    //Animation update timer
    QTimer* animUpdateTimer;

    //Having to do with clicking & dragging the right side of a sheet
    int mStartSheetW;
    int xStartDragSheetW;
    bool bDraggingSheetW;
    bool bUIMutex;

    QProgressDialog* progressBar;

    void openImportDiag();
    void importImageAsSheet(QString s, int numxframes, int numyframes, bool bVert, bool bSplit);
    void importImageAsSheet(QImage img, QString sImgFilename, int numxframes, int numyframes, bool bVert, bool bSplit);
    void importImageList(QStringList& fileList, QString prepend = QString(""), QString animName = QString(""));
    void centerParent(QWidget* parent, QWidget* child);

    void closeEvent(QCloseEvent *event);
    void loadSettings();
    void cleanMemory();

    void insertAnimHelper(QVector<QImage> imgList, QString name);  //TODO Remove

    void updateWindowTitle();
    void addUndoStep(UndoStep* step);
    void clearUndo();
    void clearRedo();
    void updateUndoRedoMenu();  //Update the menu icons to active/inactive as needed

    void genericSave(QString saveFilename);
    void saveToStream(QDataStream& s);
    void loadFromStream(QDataStream& s);

    bool loadAnimatedGIF(QString sFilename);    //Returns false on non-multi-page gif or failure, true on success

    bool isMouseOverDragArea(int x, int y);
    QGraphicsItem* isItemUnderCursor(int x, int y);
    void setColorButtonIcons();   //Set the colors of the color selection buttons
    QStringList supportedFileFormats();
    QString getSaveFilename(const char* title);
    void saveSettings();
    void saveFile();
    void deleteSelected();
    void setSaved();
    QImage loadImageFI(QString filename);  //Load a QImage using FreeImage, which generally loads images better
    void updatePlayIcon();  //Update the icon for the anim play/pause button

    bool isModified();  //If the current sheet is modified
};

#endif // MAINWINDOW_H
