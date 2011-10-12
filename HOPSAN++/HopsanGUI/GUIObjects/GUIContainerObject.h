/*-----------------------------------------------------------------------------
 This source file is part of Hopsan NG

 Copyright (c) 2011 
    Mikael Axin, Robert Braun, Alessandro Dell'Amico, Björn Eriksson,
    Peter Nordin, Karl Pettersson, Petter Krus, Ingo Staack

 This file is provided "as is", with no guarantee or warranty for the
 functionality or reliability of the contents. All contents in this file is
 the original work of the copyright holders at the Division of Fluid and
 Mechatronic Systems (Flumes) at Linköping University. Modifying, using or
 redistributing any part of this file is prohibited without explicit
 permission from the copyright holders.
-----------------------------------------------------------------------------*/

//!
//! @file   GUIContainerObject.h
//! @author Flumes <flumes@lists.iei.liu.se>
//! @date   2010-01-01
//!
//! @brief Contains the GUI Container class (base class for Systems and Groups)
//!
//$Id$

#ifndef GUICONTAINEROBJECT_H
#define GUICONTAINEROBJECT_H

#include "GUIModelObject.h"
#include "GUIComponent.h"
#include "GUIWidgets.h"
#include "../CopyStack.h"
#include "../CoreAccess.h"

//Forward Declarations
class ProjectTab;
class UndoStack;
class MainWindow;
class QGraphicsScene;
class GUIPort;

class GUIContainerObject : public GUIModelObject
{
    friend class UndoStack;     //! @todo Not sure about this, but the alternative would be to have lots and lots of access functions only used by undo stack...
    Q_OBJECT
public:
    enum ContainerEdgeT {RIGHTEDGE, BOTTOMEDGE, LEFTEDGE, TOPEDGE};
    GUIContainerObject(QPointF position, qreal rotation, const GUIModelObjectAppearance* pAppearanceData, selectionStatus startSelected = DESELECTED, graphicsType gfxType = USERGRAPHICS, GUIContainerObject *pParentContainer=0, QGraphicsItem *pParent=0);
    virtual ~GUIContainerObject();

    ProjectTab *mpParentProjectTab;  //!< @todo not public

    //Signal/slot connection methods
    void connectMainWindowActions();
    void disconnectMainWindowActions();

    //Scene and core access methods
    QGraphicsScene *getContainedScenePtr();
    virtual CoreSystemAccess *getCoreSystemAccessPtr();

    //GUIModelObjects and GUIWidgets methods
    void takeOwnershipOf(QList<GUIModelObject*> &rModeObjectlist, QList<GUIWidget*> &rWidgetList);

    //GUIModelObject methods
    GUIModelObject *addGUIModelObject(QString typeName, QPointF position, qreal rotation=0, selectionStatus startSelected = DESELECTED, nameVisibility nameStatus = USEDEFAULT, undoStatus undoSettings = UNDO);
    GUIModelObject *addGUIModelObject(GUIModelObjectAppearance* pAppearanceData, QPointF position, qreal rotation=0, selectionStatus startSelected = DESELECTED, nameVisibility nameStatus = USEDEFAULT, undoStatus undoSettings = UNDO);
    GUIModelObject *getGUIModelObject(const QString modelObjectName);
    GUIPort *getGUIModelObjectPort(const QString modelObjectName, const QString portName);
    void deleteGUIModelObject(QString componentName, undoStatus undoSettings=UNDO);
    void renameGUIModelObject(QString oldName, QString newName, undoStatus undoSettings=UNDO);
    bool hasGUIModelObject(QString name);
    void rememverSelectedGUIModelObject(GUIModelObject *object);
    void forgetSelectedGUIModelObject(GUIModelObject *object);
    QList<GUIModelObject *> getSelectedGUIModelObjectPtrs();
    bool isSubObjectSelected();

    //GUIWidgets methods
    GUITextBoxWidget *addTextBoxWidget(QPointF position, undoStatus undoSettings=UNDO);
    //GUITextWidget *addTextWidget(QPointF position, undoStatus undoSettings=UNDO);
    //GUIBoxWidget *addBoxWidget(QPointF position, undoStatus undoSettings=UNDO);
    void removeWidget(GUIWidget *pWidget, undoStatus undoSettings=UNDO);
    void rememberSelectedWidget(GUIWidget *widget);
    void forgetSelectedWidget(GUIWidget *widget);
    QList<GUIWidget *> getSelectedGUIWidgetPtrs();

    //Handle connector methods
    bool hasConnector(QString startComp, QString startPort, QString endComp, QString endPort);
    GUIConnector *findConnector(QString startComp, QString startPort, QString endComp, QString endPort);
    void rememberSubConnector(GUIConnector *pConnector);
    void removeSubConnector(GUIConnector* pConnector, undoStatus undoSettings=UNDO);
    bool isConnectorSelected();
    void rememberSelectedSubConnector(GUIConnector *pConnector);
    void forgetSelectedSubConnector(GUIConnector *pConnector);
    bool isCreatingConnector();
    void setIsCreatingConnector(bool isCreatingConnector);
    void cancelCreatingConnector();
    void addOneConnectorLine(QPointF pos);
    void removeOneConnectorLine(QPointF pos);
    void makeConnectorDiagonal(bool diagonal);
    void updateTempConnector(QPointF pos);

    //Handle container appearance
    QString getIconPath(const graphicsType gfxType, const AbsoluteRelativeT absrelType);
    void setIconPath(const QString path, const graphicsType gfxType, const AbsoluteRelativeT absrelType);
    ContainerEdgeT findPortEdge(QPointF center, QPointF pt); //!< @todo maybe not public
    virtual void refreshAppearance();
    void refreshExternalPortsAppearanceAndPosition();
    void calcSubsystemPortPosition(const double w, const double h, const double angle, double &x, double &y); //!< @todo maybe not public

    //Plot and simulation results methods
    void incrementOpenPlotCurves();
    void decrementOpenPlotCurves();
    bool hasOpenPlotCurves();
    QVector<double> getTimeVector(int generation, QString componentName, QString portName);
    QVector<double> getPlotData(int generation, QString componentName, QString portName, QString dataName);
    bool componentHasPlotGeneration(int generation, QString componentName);
    QList< QMap< QString, QMap< QString, QMap<QString, QPair<QVector<double>, QVector<double> > > > > > getAllPlotData();
    int getNumberOfPlotGenerations();
    void definePlotAlias(QString componentName, QString portName, QString dataName);
    bool definePlotAlias(QString alias, QString componentName, QString portName, QString dataName);
    void undefinePlotAlias(QString alias);
    QStringList getPlotVariableFromAlias(QString alias);
    QString getPlotAlias(QString componentName, QString portName, QString dataName);

    //Undo/redo methods
    UndoStack *getUndoStackPtr();

    //Copy/paste methods
    CopyStack *getDragCopyStackPtr();

    //These (overloaded versions) are used in containerPropertiesDialog by systems
    virtual size_t getNumberOfLogSamples(){assert(false);}
    virtual void setNumberOfLogSamples(size_t /*nSamples*/){assert(false);}

    //Model and script file methods
    void setModelFile(QString path);
    QFileInfo getModelFileInfo();
    void setScriptFile(QString path);
    QString getScriptFile();

    QStringList getGUIModelObjectNames();
    QList<GUIWidget *> getGUIWidgets();

    //Numbered sections methods
    void selectSection(int no, bool append=false);
    void assignSection(int no);

    //Favorite variables methods
    QList<QStringList> getFavoriteVariables();
    void setFavoriteVariable(QString componentName, QString portName, QString dataName, QString dataUnit);
    void removeFavoriteVariableByComponentName(QString componentName);

    //Losses methods
    bool areLossesVisible();

public slots:

    //Selection slots
    void selectAll();
    void deselectAll();
    void deselectSelectedNameText();
    QPointF getCenterPointFromSelection();

    //Show/hide slots
    void hideNames();
    void showNames();
    void toggleNames(bool value);
    void hidePorts(bool doIt);

    //Connector slots
    void createConnector(GUIPort *pPort, undoStatus undoSettings=UNDO);

    //Section slots
    void groupSelected(QPointF pt);

    //Copy/paste slots
    void cutSelected(CopyStack *xmlStack = 0);
    void copySelected(CopyStack *xmlStack = 0);
    void paste(CopyStack *xmlStack = 0);

    //Alignment slots
    void alignX();
    void alignY();

    //Undo/redo slots
    void undo();
    void redo();
    void clearUndo();
    void setUndoDisabled(bool disabled, bool dontAskJustDoIt=false);
    void setUndoEnabled(bool enabled, bool dontAskJustDoIt=false);
    bool isUndoEnabled();
    void updateMainWindowButtons();

    //Appearance slots
    void setGfxType(graphicsType gfxType);
    graphicsType getGfxType();
    bool arePortsHidden();
    bool areNamesHidden();

    //Properties slots
    void openPropertiesDialogSlot();

    //Enter and exit a container object
    void enterContainer();
    void exitContainer();

    //Rotate and flip slots
    void rotateSubObjects90cw();
    void rotateSubObjects90ccw();
    void flipSubObjectsHorizontal();
    void flipSubObjectsVertical();

    //Plot data slots
    void collectPlotData();

    //Losses
    void showLosses(bool show);
    void showLossesFromDialog();
    void hideLosses();

    //External/internal subsystems
    bool isAncestorOfExternalSubsystem();
    bool isExternal();

signals:

    //Selection signals
    void deselectAllNameText();
    void deselectAllGUIObjects();
    void selectAllGUIObjects();
    void deselectAllGUIConnectors();
    void selectAllGUIConnectors();

    //Hide/show signals
    void hideAllNameText();
    void showAllNameText();

    //Other signals
    void checkMessages();
    void deleteSelected();
    void setAllGfxType(graphicsType);
    void componentChanged();
    void connectorRemoved();
    void rotateSelectedObjectsRight();
    void rotateSelectedObjectsLeft();
    void flipSelectedObjectsHorizontal();
    void flipSelectedObjectsVertical();


protected:

    //Protected methods
    virtual void createPorts();
    virtual void createExternalPort(QString portName);
    virtual void removeExternalPort(QString portName);
    virtual void renameExternalPort(QString oldName, QString newName);
    virtual void openPropertiesDialog();
    void clearContents();
    void forgetSubConnector(GUIConnector *pConnector);
    void refreshInternalContainerPortGraphics();

    //Protected overloaded Qt methods
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    //Scene pointer member
    QGraphicsScene *mpScene;

    //Model and script file members
    QFileInfo mModelFileInfo;
    QString mScriptFilePath;

    //Model object members
    typedef QHash<QString, GUIModelObject*> GUIModelObjectMapT;
    GUIModelObjectMapT mGUIModelObjectMap;
    GUIModelObject *mpTempGUIModelObject;
    QList<GUIModelObject *> mSelectedGUIModelObjectsList;

    //Connector members
    QList<GUIConnector *> mSelectedSubConnectorsList;
    QList<GUIConnector *> mSubConnectorList;
    GUIConnector *mpTempConnector;
    bool mIsCreatingConnector;

    //Widget members
    QMap<size_t, GUIWidget *> mWidgetMap;
    QList<GUIWidget *> mSelectedGUIWidgetsList;
    size_t mHighestWidgetIndex;

    //Contained object appearance members
    bool mPortsHidden;
    bool mNamesHidden;
    graphicsType mGfxType;

    //Plot members
    QList< QMap< QString, QMap< QString, QMap<QString, QPair<QVector<double>, QVector<double> > > > > > mPlotData;
    QList< QVector<double> > mTimeVectors;
    QMap<QString, QStringList> mPlotAliasMap;
    int nPlotCurves;
    QList<QStringList> mFavoriteVariables;

    //Undo-redo members
    UndoStack *mpUndoStack;
    bool mUndoDisabled;

    //Copy-paste members
    CopyStack *mpDragCopyStack;
    double mPasteOffset;

    //Numbered sections members
    QList< QList<GUIModelObject *> > mSection;

    //Losses members
    bool mLossesVisible;
    QDialog *mpLossesDialog;
    QSlider *mpMinLossesSlider;
};

#endif // GUICONTAINEROBJECT_H
