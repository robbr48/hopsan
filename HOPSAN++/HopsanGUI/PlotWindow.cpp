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
//! @file   PlotWindow.cpp
//! @author Flumes <flumes@lists.iei.liu.se>
//! @date   2010-01-01
//!
//! @brief Contains the PlotWindow class
//!
//$Id$

#include <QDebug>
#include <QSpinBox>
#include <QColorDialog>
#include <QLabel>
#include <QCursor>
#include <QAction>
#include <QDateTime>
#include <QStandardItemModel>
#include <QtGlobal>
#include <QtGui>


#include "GUIObjects/GUIContainerObject.h"
#include "Widgets/PlotWidget.h"
#include "Widgets/MessageWidget.h"
#include "Widgets/HcomWidget.h"
#include "Widgets/ModelWidget.h"
#include "Utilities/GUIUtilities.h"
#include "Dialogs/OptionsDialog.h"
#include "GUIObjects/GUISystem.h"
#include "MainWindow.h"
#include "PlotWindow.h"
#include "GUIPort.h"
#include "GraphicsView.h"
#include "Configuration.h"
#include "loadFunctions.h"
#include "version_gui.h"
#include "ModelHandler.h"

#include "qwt_scale_engine.h"
#include "qwt_symbol.h"
#include "qwt_text_label.h"
#include "qwt_plot_renderer.h"
#include "qwt_scale_map.h"
#include "qwt_plot.h"
//#include "qwt_legend_item.h"
#include "qwt_legend.h"
//#include "q_layout.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_widget.h"
#include <qwt_dyngrid_layout.h>
#include <qwt_plot_legenditem.h>


#include "PlotHandler.h"


#include "Dependencies/BarChartPlotter/barchartplotter.h"
#include "Dependencies/BarChartPlotter/axisbase.h"


//=============================================================
#include "PlotTab.h"
#include "PlotCurve.h"


//! @brief Closes the plot tab with specified index
//! @param index Index of tab to close
void PlotTabWidget::closePlotTab(int index)
{
    PlotTab *theTab = getTab(index);
    theTab->close();
    delete(theTab);
}


//! @brief Returns a pointer to current plot tab
PlotTab *PlotTabWidget::getCurrentTab()
{
    return qobject_cast<PlotTab*>(currentWidget());
}


//! @brief Returns a pointer to plot tab with index i
//! @param i Index of plot tab
PlotTab *PlotTabWidget::getTab(const int i)
{
    return qobject_cast<PlotTab*>(widget(i));
}

void PlotTabWidget::closeAllTabs()
{
    while (count()>0)
    {
        closePlotTab(0);
    }
}


//! @brief Constructor for the plot tab widget
//! @param parent Pointer to the plot window the plot tab widget belongs to
PlotTabWidget::PlotTabWidget(PlotWindow *pParentPlotWindow)
    : QTabWidget(pParentPlotWindow)
{
    setTabsClosable(true);
    setMouseTracking(true);

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closePlotTab(int)));
}


//! @brief Constructor for the plot window, where plots are displayed.
//! @param plotVariableTree is a pointer to the variable tree from where the plot window was created
//! @param parent is a pointer to the main window
PlotWindow::PlotWindow(const QString name, QWidget *parent)
    : QMainWindow(parent)
{
    // Set name of Window
    mName = name;
    refreshWindowTitle();

    // Set Window attributes
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_MouseTracking, true);

    //setAcceptDrops(false);
    //setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setPalette(gConfig.getPalette());

    // Set size of Window
    int sh = qApp->desktop()->screenGeometry().height();
    int sw = qApp->desktop()->screenGeometry().width();
    resize(sw*0.7, sh*0.7);   //Resize plot window to 70% of screen height and width
    int w = this->size().width();
    int h = this->size().height();
    int x = (sw - w)/2;
    int y = (sh - h)/2;
    move(x, y);       //Move plot window to center of screen

    //Create the toolbar and its buttons
    mpToolBar = new QToolBar(this);

    mpNewPlotButton = new QAction(this);
    mpNewPlotButton->setToolTip("Create New Plot");
    mpNewPlotButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-NewPlot.png"));
    connect(mpNewPlotButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpArrowButton = new QAction(this);
    mpArrowButton->setToolTip("Arrow (P)");
    mpArrowButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Pointer.png"));
    mpArrowButton->setCheckable((true));
    mpArrowButton->setShortcut(QKeySequence("p"));
    //! @todo Check short cut if to confirm if there is a conflict or not
    connect(mpArrowButton, SIGNAL(hovered()),this, SLOT(showToolBarHelpPopup()));

    mpLegendButton = new QAction(this);
    mpLegendButton->setToolTip("Legend (L)");
    mpLegendButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ShowPlotLegends.png"));
    //mpLegendButton->setCheckable((true));
    mpLegendButton->setShortcut(QKeySequence("p"));
    //! @todo Check short cut if to confirm if there is a conflict or not
    connect(mpLegendButton, SIGNAL(hovered()),this, SLOT(showToolBarHelpPopup()));

    mpZoomButton = new QAction(this);
    mpZoomButton->setToolTip("Zoom (Z)");
    mpZoomButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Zoom.png"));
    mpZoomButton->setCheckable(true);
    mpZoomButton->setShortcut(QKeySequence("z"));
    connect(mpZoomButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpOriginalZoomButton = new QAction(this);
    mpOriginalZoomButton->setToolTip("Reset original Zoom");
    mpOriginalZoomButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Zoom100.png"));


    mpPanButton = new QAction(this);
    mpPanButton->setToolTip("Pan (X)");
    mpPanButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Pan.png"));
    mpPanButton->setCheckable(true);
    mpPanButton->setShortcut(QKeySequence("x"));
    connect(mpPanButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpSaveButton = new QAction(this);
    mpSaveButton->setToolTip("Save Plot Window Description File (.xml)");
    mpSaveButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Save.png"));
    mpSaveButton->setShortcut(QKeySequence("Ctrl+s"));
    connect(mpSaveButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpImportPloAction = new QAction(this);
    mpImportPloAction->setText("Import from Old Hopsan File (.plo)");
    mpImportPloAction->setToolTip("Import from Old Hopsan File (.plo)");

    mpImportCsvAction = new QAction(this);
    mpImportCsvAction->setText("Import from Comma-Separated Values File (.csv)");
    mpImportCsvAction->setToolTip("Import from Comma-Separated Values File (.csv)");

    mpImportMenu = new QMenu(mpToolBar);
    mpImportMenu->addAction(mpImportPloAction);
    mpImportMenu->addAction(mpImportCsvAction);

    mpImportButton = new QToolButton(mpToolBar);
    mpImportButton->setToolTip("Import Plot Data");
    mpImportButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ImportPlot.png"));
    mpImportButton->setMenu(mpImportMenu);
    mpImportButton->setPopupMode(QToolButton::InstantPopup);
    mpImportButton->setMouseTracking(true);
    connect(mpImportPloAction, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpExportToXmlAction = new QAction("Export to Extensible Markup Language File (.xml)", mpToolBar);
    mpExportToCsvAction = new QAction("Export to Comma-Separeted Values File (.csv)", mpToolBar);
    mpExportToHvcAction = new QAction("Export to Hopsan Validation Files (.hvc and .csv)", mpToolBar);
    mpExportToMatlabAction = new QAction("Export to Matlab Script File (.m)", mpToolBar);
    mpExportToGnuplotAction = new QAction("Export to gnuplot data file(.dat)", mpToolBar);
    mpExportToOldHopAction = new QAction("Export to Hopsan Classic file(.plo)", mpToolBar);

    mpExportMenu = new QMenu(mpToolBar);
    mpExportMenu->addAction(mpExportToXmlAction);
    mpExportMenu->addAction(mpExportToCsvAction);
    mpExportMenu->addAction(mpExportToHvcAction);
    mpExportMenu->addAction(mpExportToMatlabAction);
    mpExportMenu->addAction(mpExportToGnuplotAction);
    mpExportMenu->addAction(mpExportToOldHopAction);

    mpExportButton = new QToolButton(mpToolBar);
    mpExportButton->setToolTip("Export Plot Tab");
    mpExportButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ExportPlot.png"));
    mpExportButton->setMenu(mpExportMenu);
    mpExportButton->setPopupMode(QToolButton::InstantPopup);
    mpExportButton->setMouseTracking(true);

//    mpExportPdfAction = new QAction("Export to PDF", mpToolBar);
//    mpExportPngAction = new QAction("Export to PNG", mpToolBar);

    mpExportToGraphicsAction = new QAction("Export as Graphics", mpToolBar);
    mpExportToGraphicsAction->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ExportGfx.png"));
    mpExportToGraphicsAction->setToolTip("Export to Graphics File");


//    mpExportGfxMenu = new QMenu(mpToolBar);
//    mpExportGfxMenu->addAction(mpExportPdfAction);
//    mpExportGfxMenu->addAction(mpExportPngAction);
//    mpExportGfxMenu->addAction(mpExportToGraphicsAction);

//    mpExportGfxButton = new QToolButton(mpToolBar);
//    mpExportGfxButton->setToolTip("Export to Graphics File");
//    mpExportGfxButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ExportGfx.png"));
//    mpExportGfxButton->setMenu(mpExportGfxMenu);
//    mpExportGfxButton->setPopupMode(QToolButton::InstantPopup);
//    mpExportGfxButton->setMouseTracking(true);

    mpLoadFromXmlButton = new QAction(this);
    mpLoadFromXmlButton->setToolTip("Import Plot");
    mpLoadFromXmlButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Open.png"));
    connect(mpLoadFromXmlButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpGridButton = new QAction(this);
    mpGridButton->setToolTip("Show Grid (G)");
    mpGridButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Grid.png"));
    mpGridButton->setCheckable(true);
    mpGridButton->setChecked(true);
    mpGridButton->setShortcut(QKeySequence("g"));
    connect(mpGridButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpBackgroundColorButton = new QAction(this);
    mpBackgroundColorButton->setToolTip("Select Canvas Color (C)");
    mpBackgroundColorButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-BackgroundColor.png"));
    mpBackgroundColorButton->setShortcut(QKeySequence("c"));
    connect(mpBackgroundColorButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpLocktheAxis = new QAction(this);
    mpLocktheAxis->setToolTip("Lock Axis");
    mpLocktheAxis->setIcon(QIcon(QString(ICONPATH) + "Hopsan-PlotCurveScale.png"));
    connect(mpLocktheAxis, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpToggleAxisLockButton = new QAction(this);
    mpToggleAxisLockButton->setToolTip("Lock Axis To Current Limits");
    mpToggleAxisLockButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-Lock.png"));
    connect(mpToggleAxisLockButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpOpentimeScaleDialog = new QAction(this);
    mpOpentimeScaleDialog->setToolTip("Open Time-Scale Dialog");
    mpOpentimeScaleDialog->setIcon(QIcon(QString(ICONPATH) + "Hopsan-AxisTimeScale.png"));


    mpNewWindowFromTabButton = new QAction(this);
    mpNewWindowFromTabButton->setToolTip("Create Plot Window From Tab");
    mpNewWindowFromTabButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-OpenTabInNewPlotWindow.png"));
    connect(mpNewWindowFromTabButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpResetXVectorButton = new QAction(this);
    mpResetXVectorButton->setToolTip("Reset Time Vector");
    mpResetXVectorButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ResetTimeVector.png"));
    mpResetXVectorButton->setEnabled(false);
    connect(mpResetXVectorButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpBodePlotButton = new QAction(this);
    mpBodePlotButton->setToolTip("Transfer Function Analysis");
    mpBodePlotButton->setIcon(QIcon(QString(ICONPATH) + "Hopsan-TransferFunctionAnalysis.png"));
    connect(mpBodePlotButton, SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    mpAllGenerationsDown = new QAction(this);
    mpAllGenerationsDown->setIcon(QIcon(QString(ICONPATH) + "Hopsan-StepLeft.png"));
    mpAllGenerationsDown->setToolTip("Shift all curve generations down");

    mpAllGenerationsUp = new QAction(this);
    mpAllGenerationsUp->setIcon(QIcon(QString(ICONPATH) + "Hopsan-StepRight.png"));
    mpAllGenerationsUp->setToolTip("Shift all curve generations up");

    //Initialize the help message popup
    mpHelpPopup = new QWidget(this);
    mpHelpPopup->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    mpHelpPopupIcon = new QLabel();
    mpHelpPopupIcon->setMouseTracking(true);
    mpHelpPopupIcon->setPixmap(QPixmap(QString(ICONPATH) + "Hopsan-Info.png"));
    mpHelpPopupLabel = new QLabel();
    mpHelpPopupLabel->setMouseTracking(true);
    mpHelpPopupGroupBoxLayout = new QHBoxLayout(mpHelpPopup);
    mpHelpPopupGroupBoxLayout->addWidget(mpHelpPopupIcon);
    mpHelpPopupGroupBoxLayout->addWidget(mpHelpPopupLabel);
    mpHelpPopupGroupBoxLayout->setContentsMargins(3,3,3,3);
    mpHelpPopupGroupBox = new QGroupBox(mpHelpPopup);
    mpHelpPopupGroupBox->setLayout(mpHelpPopupGroupBoxLayout);
    mpHelpPopupGroupBox->setMouseTracking(true);
    mpHelpPopupLayout = new QHBoxLayout(mpHelpPopup);
    mpHelpPopupLayout->addWidget(mpHelpPopupGroupBox);
    mpHelpPopup->setLayout(mpHelpPopupLayout);
    mpHelpPopup->setMouseTracking(true);
    mpHelpPopup->setBaseSize(50,30);
    mpHelpPopup->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    mpHelpPopup->setStyleSheet("QGroupBox { background-color : rgba(255,255,224,255); } QLabel { margin : 0px; } ");
    mpHelpPopup->hide();
    mpHelpPopupTimer = new QTimer(this);
    connect(mpHelpPopupTimer, SIGNAL(timeout()), mpHelpPopup, SLOT(hide()));

    // Setup PlotVariable List stuff
    PlotTreeWidget *pLocalPlotWidget = new PlotTreeWidget(this);
    QDockWidget *pLocalPlotWidgetDock = new QDockWidget(tr("Plot Variables"), this);
    pLocalPlotWidgetDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, pLocalPlotWidgetDock);
    pLocalPlotWidgetDock->setWidget(pLocalPlotWidget);
    if(gpModelHandler->count() != 0)
    {
        pLocalPlotWidget->mpPlotVariableTree->setLogDataHandler(gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler()); //!< @todo not necessarily the same as where the plot data will come from if plot by script
    }

    pLocalPlotWidgetDock->toggleViewAction()->setToolTip("Toggle Variable List");
    pLocalPlotWidgetDock->toggleViewAction()->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ShowPlotWindowVariableList.png"));
    connect(pLocalPlotWidgetDock->toggleViewAction(), SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    // Setup CurveInfoBox stuff
    mpCurveInfoStack = new QStackedWidget(this);

    mpCurveInfoDock = new QDockWidget(tr("PlotCurve Settings"), this);
    mpCurveInfoDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, mpCurveInfoDock);
    mpCurveInfoDock->setWidget(mpCurveInfoStack);
    mpCurveInfoDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    mpCurveInfoDock->setPalette(gConfig.getPalette());
    mpCurveInfoDock->show();

    mpCurveInfoDock->toggleViewAction()->setToolTip("Toggle Curve Controls");
    mpCurveInfoDock->toggleViewAction()->setIcon(QIcon(QString(ICONPATH) + "Hopsan-ShowPlotWindowCurveSettings.png"));
    connect(mpCurveInfoDock->toggleViewAction(), SIGNAL(hovered()), this, SLOT(showToolBarHelpPopup()));

    // Populate toolbar with actions
    mpToolBar->addAction(mpNewPlotButton);
    mpToolBar->addAction(mpLoadFromXmlButton);
    mpToolBar->addAction(mpSaveButton);
    mpToolBar->addWidget(mpImportButton);
    mpToolBar->addWidget(mpExportButton);
    //mpToolBar->addWidget(mpExportGfxButton);
    mpToolBar->addAction(mpExportToGraphicsAction);
    mpToolBar->addSeparator();
    mpToolBar->addAction(mpArrowButton);
    mpToolBar->addAction(mpPanButton);
    mpToolBar->addAction(mpZoomButton);
    mpToolBar->addAction(mpOriginalZoomButton);
    mpToolBar->addSeparator();
    mpToolBar->addAction(mpResetXVectorButton);
    mpToolBar->addAction(mpBodePlotButton);
    mpToolBar->addAction(mpAllGenerationsDown);
    mpToolBar->addAction(mpAllGenerationsUp);
    mpToolBar->addAction(mpOpentimeScaleDialog);
    mpToolBar->addSeparator();
    mpToolBar->addAction(mpGridButton);
    mpToolBar->addAction(mpBackgroundColorButton);
    mpToolBar->addAction(mpLegendButton);
    mpToolBar->addAction(mpLocktheAxis);
    mpToolBar->addAction(mpToggleAxisLockButton);
    mpToolBar->addAction(mpCurveInfoDock->toggleViewAction());
    mpToolBar->addAction(pLocalPlotWidgetDock->toggleViewAction());
    mpToolBar->addSeparator();
    mpToolBar->addAction(mpNewWindowFromTabButton);
    mpToolBar->setMouseTracking(true);
    addToolBar(mpToolBar);

    this->setDockOptions(QMainWindow::AllowNestedDocks);

    mpPlotTabWidget = new PlotTabWidget(this);
    this->addPlotTab();
    this->changedTab();

    mpLayout = new QGridLayout(this);
    mpLayout->addWidget(mpPlotTabWidget,0,0,2,4);
    mpLayout->addWidget(mpHelpPopup, 0,0,1,4);

    //Set the correct position of the help popup message in the central widget
    mpLayout->setColumnMinimumWidth(0,2);
    mpLayout->setColumnStretch(0,0);
    mpLayout->setColumnStretch(1,1);
    mpLayout->setRowMinimumHeight(0,2);
    mpLayout->setRowStretch(0,0);
    mpLayout->setRowStretch(1,1);
    mpLayout->setRowStretch(2,0);

    QWidget *pCentralWidget = new QWidget(this);
    pCentralWidget->setLayout(mpLayout);
    this->setCentralWidget(pCentralWidget);

    // Establish toolbar button signal and slots connections
    connect(mpNewPlotButton,                    SIGNAL(triggered()),            this,               SLOT(addPlotTab()));
    connect(mpLoadFromXmlButton,                SIGNAL(triggered()),            this,               SLOT(loadFromXml()));
    connect(mpImportPloAction,                  SIGNAL(triggered()),            this,               SLOT(importPlo()));
    connect(mpImportCsvAction,                  SIGNAL(triggered()),            this,               SLOT(importCsv()));
    connect(mpSaveButton,                       SIGNAL(triggered()),            this,               SLOT(saveToXml()));
    connect(mpBodePlotButton,                   SIGNAL(triggered()),            this,               SLOT(createBodePlot()));
    connect(mpNewWindowFromTabButton,           SIGNAL(triggered()),            this,               SLOT(createPlotWindowFromTab()));
    connect(gpMainWindow->getOptionsDialog(),   SIGNAL(paletteChanged()),       this,               SLOT(updatePalette()));
    connect(mpPlotTabWidget,                    SIGNAL(currentChanged(int)),    this,               SLOT(changedTab()));

    // Hide curve settings area by default if screen size is to small
    if(sh*sw < 800*1280)
    {
        mpCurveInfoDock->toggleViewAction()->toggle();
    }

    this->setMouseTracking(true);
}

PlotWindow::~PlotWindow()
{
    // Need to close tabs before window is closed and destructors are run
    mpPlotTabWidget->closeAllTabs();
}


void PlotWindow::setCustomXVector(QVector<double> xarray, const VariableCommonDescription &rVarDesc)
{
    getCurrentPlotTab()->setCustomXVectorForAll(xarray, rVarDesc, FirstPlot);
}

void PlotWindow::setCustomXVector(SharedLogVariableDataPtrT pData)
{
    getCurrentPlotTab()->setCustomXVectorForAll(pData);
}


void PlotWindow::addPlotTab(QString requestedName)
{
    PlotTab *mpNewTab = new PlotTab(mpPlotTabWidget, this);

    QString tabName;
    QString numString;
    if(requestedName.isEmpty())
    {
        for(int tabID=0; true; ++tabID)
        {
            numString.setNum(tabID);
            bool found = false;
            for(int i=0; i<mpPlotTabWidget->count(); ++i)
            {
                if(mpPlotTabWidget->tabText(i) == "Plot " + numString)
                {
                    found=true;
                    break;
                }
            }
            if(!found)
                break;
        }
        tabName = "Plot " + numString;
    }
    else
    {
        tabName = requestedName;
    }

    mpPlotTabWidget->addTab(mpNewTab, tabName);

    mpPlotTabWidget->setCurrentIndex(mpPlotTabWidget->count()-1);


}


PlotTabWidget *PlotWindow::getPlotTabWidget()
{
    return mpPlotTabWidget;
}


PlotTab *PlotWindow::getCurrentPlotTab()
{
    return mpPlotTabWidget->getCurrentTab();
}




//! @brief Shows the help popup message for 5 seconds with specified message.
//! Any message already being shown will be replaced. Messages can be hidden in advance by calling mpHelpPopup->hide().
//! @param message String with text so show in message
void PlotWindow::showHelpPopupMessage(QString message)
{
    if(gConfig.getShowPopupHelp())
    {
        mpHelpPopupLabel->setText(message);
        mpHelpPopup->show();
        mpHelpPopupTimer->stop();
        mpHelpPopupTimer->start(5000);
    }
}


//! @brief Hides the help popup message
void PlotWindow::hideHelpPopupMessage()
{
    mpHelpPopup->hide();
}

QString PlotWindow::getName() const
{
    return mName;
}


//! @brief Creates a new plot curve from a plot variable in current container object and adds it to the current plot tab
//! @param generation Generation of plot data
//! @param componentName Name of component where variable is located
//! @param portName Name of port where variable is located
//! @param dataName Name of variable
//! @param dataUnit Unit of variable
PlotCurve* PlotWindow::addPlotCurve(SharedLogVariableDataPtrT pData, int axisY, QColor desiredColor)
{
    if (pData)
    {
        //! @todo check if model path same as earlier to prvent mixing data
        // Remember which model it belongs to, and connect the closeWindow signal from the data handler
        // this makes it possible to autoclose all plotwindows with data from a particaulr model(logdatahandler)
        mModelName = pData->getModelPath();
        if (pData->getLogDataHandler())
        {
            connect(pData->getLogDataHandler(), SIGNAL(closePlotsWithOwnedData()), this, SLOT(close()), Qt::UniqueConnection);
        }

        // Make sure that we have a tab
        if (!getCurrentPlotTab())
        {
            addPlotTab();
            changedTab();
        }
        PlotCurve *pTempCurve = new PlotCurve(pData, axisY, getCurrentPlotTab());
        getCurrentPlotTab()->addCurve(pTempCurve, desiredColor);
        refreshWindowTitle();
        return pTempCurve;
    }
    return 0;
}

PlotCurve *PlotWindow::addPlotCurve(SharedLogVariableDataPtrT pXData, SharedLogVariableDataPtrT pYData, int axisY, QColor desiredColor)
{
    PlotCurve *pCurve = addPlotCurve(pYData, axisY, desiredColor);
    if (pCurve)
    {
        pCurve->setCustomXData(pXData);
    }
    return pCurve;
}


void PlotWindow::addBarChart(QStandardItemModel *pItemModel)
{
    getCurrentPlotTab()->addBarChart(pItemModel);
    changedTab(); //Refresh buttons on/off
}

//! @brief Imports .Plo files from Old Hopsan
//! Imports Plot Data Only

void PlotWindow::importPlo()
{
    gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler()->importFromPlo();
}


void PlotWindow::importCsv()
{
    gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler()->importFromCSV_AutoFormat();
}


//! @brief Saves the plot window to XML
//! All generations of all open curves will be saved, together with all cosmetic information about the plot window.
void PlotWindow::saveToXml()
{
    //Open file dialog and initialize the file stream
    QDir fileDialogSaveDir;
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Plot Window Description to XML"),
                                                    gConfig.getPlotWindowDir(),
                                                    tr("Plot Window Description File (*.xml)"));
    if(filePath.isEmpty())
    {
        return;    //Don't save anything if user presses cancel
    }
    else
    {
        QFileInfo fileInfo = QFileInfo(filePath);
        gConfig.setPlotWindowDir(fileInfo.absolutePath());
    }

    //Write to xml file
    QDomDocument domDocument;
    QDomElement xmlRootElement = domDocument.createElement("hopsanplot");
    domDocument.appendChild(xmlRootElement);

    QDomElement dateElement = appendDomElement(xmlRootElement,"date");      //Append date tag
    QDate date = QDate::currentDate();
    dateElement.setAttribute("year", date.year());
    dateElement.setAttribute("month", date.month());
    dateElement.setAttribute("day", date.day());

    QDomElement timeElement = appendDomElement(xmlRootElement,"time");      //Append time tag
    QTime time = QTime::currentTime();
    timeElement.setAttribute("hour", time.hour());
    timeElement.setAttribute("minute", time.minute());
    timeElement.setAttribute("second", time.second());

    //Add tab elements
    for(int i=0; i<mpPlotTabWidget->count(); ++i)
    {
        QDomElement tabElement = appendDomElement(xmlRootElement,"plottab");
        if(mpPlotTabWidget->getTab(i)->isGridVisible())
        {
            tabElement.setAttribute("grid", "true");
        }
        else
        {
            tabElement.setAttribute("grid", "false");
        }
        tabElement.setAttribute("color", makeRgbString(mpPlotTabWidget->getTab(i)->getPlot()->canvasBackground().color()));

        if(mpPlotTabWidget->getTab(i)->mHasCustomXData)
        {
            QDomElement specialXElement = appendDomElement(tabElement,"specialx");
            //! @todo FIXA /Peter
            //specialXElement.setAttribute("generation",  mpPlotTabs->getTab(i)->mVectorXGeneration);
//            specialXElement.setAttribute("component",   mpPlotTabWidget->getTab(i)->mSpecialXVectorDescription->mComponentName);
//            specialXElement.setAttribute("port",        mpPlotTabWidget->getTab(i)->mSpecialXVectorDescription->mPortName);
//            specialXElement.setAttribute("data",        mpPlotTabWidget->getTab(i)->mSpecialXVectorDescription->mDataName);
//            specialXElement.setAttribute("unit",        mpPlotTabWidget->getTab(i)->mSpecialXVectorDescription->mDataUnit);
//            specialXElement.setAttribute("model",       mpPlotTabWidget->getTab(i)->mSpecialXVectorDescription->mModelPath);
        }

        //Add curve elements
        for(int j=0; j<mpPlotTabWidget->getTab(i)->getCurves().size(); ++j)
        {
            QDomElement curveElement = appendDomElement(tabElement,"curve");
            //! @todo FIXA /Peter
            //curveElement.setAttribute("generation", mpPlotTabs->getTab(i)->getCurves().at(j)->getGeneration());
            curveElement.setAttribute("component",  mpPlotTabWidget->getTab(i)->getCurves().at(j)->getComponentName());
            curveElement.setAttribute("port",       mpPlotTabWidget->getTab(i)->getCurves().at(j)->getPortName());
            curveElement.setAttribute("data",       mpPlotTabWidget->getTab(i)->getCurves().at(j)->getDataName());
            //curveElement.setAttribute("unit",       mpPlotTabWidget->getTab(i)->getCurves().at(j)->getDataUnit());
            curveElement.setAttribute("axis",       mpPlotTabWidget->getTab(i)->getCurves().at(j)->getAxisY());
            curveElement.setAttribute("width",      mpPlotTabWidget->getTab(i)->getCurves().at(j)->pen().width());
            curveElement.setAttribute("color",      makeRgbString(mpPlotTabWidget->getTab(i)->getCurves().at(j)->pen().color()));
            //curveElement.setAttribute("model",      mpPlotTabWidget->getTab(i)->getCurves().at(j)->getContainerObjectPtr()->getModelFileInfo().filePath());
            //! @todo FIXA /Peter
        }
    }

    appendRootXMLProcessingInstruction(domDocument);

    //Save to file
    QFile xmlsettings(filePath);
    if (!xmlsettings.open(QIODevice::WriteOnly | QIODevice::Text))  //open file
    {
        qDebug() << "Failed to open file for writing: " << filePath;
        return;
    }
    QTextStream out(&xmlsettings);
    domDocument.save(out, XMLINDENTATION);
}


//! @brief Loads a plot window from XML
void PlotWindow::loadFromXml()
{
    gpPlotWidget->loadFromXml();
}


void PlotWindow::performFrequencyAnalysis(PlotCurve *curve)
{
    mpFrequencyAnalysisCurve = curve;

    QLabel *pInfoLabel = new QLabel(tr("This will generate a frequency spectrum. Using more log samples will increase accuracy of the results."));
    pInfoLabel->setWordWrap(true);
    pInfoLabel->setFixedWidth(300);

    mpFrequencyAnalysisDialog = new QDialog(this);
    mpFrequencyAnalysisDialog->setWindowTitle("Generate Frequency Spectrum");

    mpLogScaleCheckBox = new QCheckBox("Use log scale");
    mpLogScaleCheckBox->setChecked(true);

    mpPowerSpectrumCheckBox = new QCheckBox("Power spectrum");
    mpPowerSpectrumCheckBox->setChecked(false);

    QPushButton *pCancelButton = new QPushButton("Cancel");
    QPushButton *pNextButton = new QPushButton("Go!");

    //Toolbar
    QAction *pHelpAction = new QAction("Show Context Help", this);
    pHelpAction->setIcon(QIcon(QString(ICONPATH)+"Hopsan-Help.png"));
    QToolBar *pToolBar = new QToolBar(this);
    pToolBar->addAction(pHelpAction);

    QGridLayout *pFrequencyAnalysisDialogLayout = new QGridLayout(mpFrequencyAnalysisDialog);
    pFrequencyAnalysisDialogLayout->addWidget(pInfoLabel,               0, 0, 1, 4);
    pFrequencyAnalysisDialogLayout->addWidget(mpLogScaleCheckBox,       1, 0, 1, 4);
    pFrequencyAnalysisDialogLayout->addWidget(mpPowerSpectrumCheckBox,  2, 0, 1, 4);
    pFrequencyAnalysisDialogLayout->addWidget(pToolBar,                 3, 0, 1, 1);
    pFrequencyAnalysisDialogLayout->addWidget(new QWidget(this),        3, 1, 1, 1);
    pFrequencyAnalysisDialogLayout->addWidget(pCancelButton,            3, 2, 1, 1);
    pFrequencyAnalysisDialogLayout->addWidget(pNextButton,              3, 3, 1, 1);
    pFrequencyAnalysisDialogLayout->setColumnStretch(1, 1);

    mpFrequencyAnalysisDialog->setLayout(pFrequencyAnalysisDialogLayout);
    mpFrequencyAnalysisDialog->setPalette(gConfig.getPalette());
    mpFrequencyAnalysisDialog->show();

    connect(pCancelButton, SIGNAL(clicked()), mpFrequencyAnalysisDialog, SLOT(close()));
    connect(pNextButton, SIGNAL(clicked()), this, SLOT(performFrequencyAnalysisFromDialog()));
    connect(pHelpAction, SIGNAL(triggered()), this, SLOT(showFrequencyAnalysisHelp()));
}


void PlotWindow::performFrequencyAnalysisFromDialog()
{
    mpFrequencyAnalysisDialog->close();

    addPlotTab();
    getCurrentPlotTab()->getPlot()->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
    getCurrentPlotTab()->updateAxisLabels();
    LogDataHandler *pLogDataHandler = gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler();
    SharedLogVariableDataPtrT pVariable = mpFrequencyAnalysisCurve->getLogDataVariablePtr();
    bool power = mpPowerSpectrumCheckBox->isChecked();
    SharedLogVariableDataPtrT pNewVar = pLogDataHandler->fftVariable(pVariable, SharedLogVariableDataPtrT(), power);
    PlotCurve *pNewCurve = new PlotCurve(pNewVar, QwtPlot::yLeft, getCurrentPlotTab(), FirstPlot, FrequencyAnalysisType);
    getCurrentPlotTab()->addCurve(pNewCurve);

    if(mpLogScaleCheckBox->isChecked())
    {
        getCurrentPlotTab()->getPlot(FirstPlot)->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
        getCurrentPlotTab()->getPlot(FirstPlot)->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
    }
    getCurrentPlotTab()->rescaleAxesToCurves();
}


void PlotWindow::showFrequencyAnalysisHelp()
{
    gpMainWindow->openContextHelp("userFrequencyAnalysis.html");
}


void PlotWindow::createBodePlot()
{
    mpCreateBodeDialog = new QDialog(this);
    mpCreateBodeDialog->setWindowTitle("Transfer Function Analysis");

    QGroupBox *pInputGroupBox = new QGroupBox(tr("Input Variable"));
    QVBoxLayout *pInputGroupBoxLayout = new QVBoxLayout;
    pInputGroupBoxLayout->addStretch(1);
    for(int i=0; i<getCurrentPlotTab()->getNumberOfCurves(FirstPlot); ++i)
    {
        QRadioButton *radio = new QRadioButton(getCurrentPlotTab()->getCurves().at(i)->getComponentName() + ", " +
                                               getCurrentPlotTab()->getCurves().at(i)->getPortName() + ", " +
                                               getCurrentPlotTab()->getCurves().at(i)->getDataName());
        mBodeInputButtonToCurveMap.insert(radio, getCurrentPlotTab()->getCurves().at(i));
        pInputGroupBoxLayout->addWidget(radio);
    }
    pInputGroupBox->setLayout(pInputGroupBoxLayout);

    QGroupBox *pOutputGroupBox = new QGroupBox(tr("Output Variable"));
    QVBoxLayout *pOutputGroupBoxLayout = new QVBoxLayout;
    pOutputGroupBoxLayout->addStretch(1);
    for(int i=0; i<getCurrentPlotTab()->getNumberOfCurves(FirstPlot); ++i)
    {
        QRadioButton *radio = new QRadioButton(getCurrentPlotTab()->getCurves().at(i)->getComponentName() + ", " +
                                               getCurrentPlotTab()->getCurves().at(i)->getPortName() + ", " +
                                               getCurrentPlotTab()->getCurves().at(i)->getDataName());
        mBodeOutputButtonToCurveMap.insert(radio, getCurrentPlotTab()->getCurves().at(i));
        pOutputGroupBoxLayout->addWidget(radio);
    }
    pOutputGroupBox->setLayout(pOutputGroupBoxLayout);

    double dataSize = getCurrentPlotTab()->getCurves(FirstPlot).first()->getTimeVectorPtr()->getDataSize()+1;
    double stopTime = getCurrentPlotTab()->getCurves(FirstPlot).first()->getTimeVectorPtr()->last();
    double maxFreq = dataSize/stopTime/2;
    QLabel *pMaxFrequencyLabel = new QLabel("Maximum frequency in bode plot:");
    QLabel *pMaxFrequencyValue = new QLabel();
    QLabel *pMaxFrequencyUnit = new QLabel("Hz");
    pMaxFrequencyValue->setNum(maxFreq);
    mpMaxFrequencySlider = new QSlider(this);
    mpMaxFrequencySlider->setOrientation(Qt::Horizontal);
    mpMaxFrequencySlider->setMinimum(0);
    mpMaxFrequencySlider->setMaximum(maxFreq);
    mpMaxFrequencySlider->setValue(maxFreq);
    connect(mpMaxFrequencySlider, SIGNAL(valueChanged(int)), pMaxFrequencyValue, SLOT(setNum(int)));

    QHBoxLayout *pSliderLayout = new QHBoxLayout();
    pSliderLayout->addWidget(mpMaxFrequencySlider);
    pSliderLayout->addWidget(pMaxFrequencyValue);
    pSliderLayout->addWidget(pMaxFrequencyUnit);

    QPushButton *pCancelButton = new QPushButton("Cancel");
    QPushButton *pNextButton = new QPushButton("Go!");

    //Toolbar
    QAction *pHelpAction = new QAction("Show Context Help", this);
    pHelpAction->setIcon(QIcon(QString(ICONPATH)+"Hopsan-Help.png"));
    QToolBar *pToolBar = new QToolBar(this);
    pToolBar->addAction(pHelpAction);

    QGridLayout *pBodeDialogLayout = new QGridLayout;
    pBodeDialogLayout->addWidget(pInputGroupBox, 0, 0, 1, 3);
    pBodeDialogLayout->addWidget(pOutputGroupBox, 1, 0, 1, 3);
    pBodeDialogLayout->addWidget(pMaxFrequencyLabel, 2, 0, 1, 3);
    pBodeDialogLayout->addLayout(pSliderLayout, 3, 0, 1, 3);
    pBodeDialogLayout->addWidget(pToolBar, 4, 0, 1, 1);
    pBodeDialogLayout->addWidget(pCancelButton, 4, 1, 1, 1);
    pBodeDialogLayout->addWidget(pNextButton, 4, 2, 1, 1);

    mpCreateBodeDialog->setLayout(pBodeDialogLayout);
    mpCreateBodeDialog->setPalette(gConfig.getPalette());
    mpCreateBodeDialog->show();

    connect(pCancelButton, SIGNAL(clicked()), mpCreateBodeDialog, SLOT(close()));
    connect(pNextButton, SIGNAL(clicked()), this, SLOT(createBodePlotFromDialog()));
    connect(pHelpAction, SIGNAL(triggered()), this, SLOT(showFrequencyAnalysisHelp()));
    //connect(pNextButton, SIGNAL(clicked()), pCreateBodeDialog, SLOT(close()));
}


void PlotWindow::createBodePlotFromDialog()
{
    PlotCurve *inputCurve = 0;
    PlotCurve *outputCurve = 0;
    QMap<QRadioButton *, PlotCurve *>::iterator it;
    for(it=mBodeInputButtonToCurveMap.begin(); it!=mBodeInputButtonToCurveMap.end(); ++it)
    {
        if(it.key()->isChecked())
            inputCurve = it.value();
    }
    for(it=mBodeOutputButtonToCurveMap.begin(); it!=mBodeOutputButtonToCurveMap.end(); ++it)
    {
        if(it.key()->isChecked())
            outputCurve = it.value();
    }
    if(inputCurve == 0 || outputCurve == 0)
    {
        QMessageBox::warning(this, tr("Transfer Function Analysis Failed"), tr("Both input and output vectors must be selected."));
    }
    else if(inputCurve == outputCurve)
    {
        QMessageBox::warning(this, tr("Transfer Function Analysis Failed"), tr("Input and output vectors must be different."));
    }
    else
    {
        mpCreateBodeDialog->close();
        createBodePlot(inputCurve, outputCurve, mpMaxFrequencySlider->value());
    }
}


void PlotWindow::createBodePlot(SharedLogVariableDataPtrT var1, SharedLogVariableDataPtrT var2, int Fmax)
{
    PlotCurve *pCurve1 = new PlotCurve(var1,QwtPlot::yLeft, getCurrentPlotTab());
    PlotCurve *pCurve2 = new PlotCurve(var2,QwtPlot::yLeft, getCurrentPlotTab());
    createBodePlot(pCurve1, pCurve2, Fmax);
}


void PlotWindow::createBodePlot(PlotCurve *pInputCurve, PlotCurve *pOutputCurve, int Fmax)
{
    //Create temporary real vectors
    QVector<double> realYvector = pInputCurve->getDataVectorCopy();
    QVector<double> realXvector = pOutputCurve->getDataVectorCopy();

    //Abort and inform user if vectors are not of same size
    if(realXvector.size() != realYvector.size())
    {
        QMessageBox::warning(gpMainWindow, gpMainWindow->tr("Wrong Vector Size"),
                             "Input and output vector must be of same size.");
        return;
    }

    //Reduce vector size if they are not equal to an even potential of 2, and inform user
    int n = pow(2, int(log2(realXvector.size())));
    if(n != realXvector.size())     //Vector is not an exact potential, so reduce it
    {
        QString oldString, newString;
        oldString.setNum(realXvector.size());
        newString.setNum(n);
        QMessageBox::information(gpMainWindow, gpMainWindow->tr("Wrong Vector Size"),
                                 "Size of data vector must be an even power of 2. Number of log samples was reduced from " + oldString + " to " + newString + ".");
        reduceVectorSize(realXvector, n);
        reduceVectorSize(realYvector, n);
    }

    //Create complex vectors
    QVector< std::complex<double> > Y = realToComplex(realYvector);
    QVector< std::complex<double> > X = realToComplex(realXvector);

    //Apply the fourier transforms
    FFT(X);
    FFT(Y);

    //Divide the fourier transform elementwise and take their absolute value
    QVector< std::complex<double> > G;
    QVector<double> vRe;
    QVector<double> vIm;
    QVector<double> vImNeg;
    QVector<double> vBodeGain;
    QVector<double> vBodePhase;

    double phaseCorrection=0;
    QVector<double> vBodePhaseUncorrected;
    for(int i=0; i<Y.size()/2; ++i)
    {
        if(Y.at(i) == std::complex<double>(0,0))        //Check for division by zero
        {
            G.append(G[i-1]);    //! @todo This is not a good solution, and what if i=0?
        }
        else
        {
            G.append(X.at(i)/Y.at(i));                  //G(iw) = FFT(Y(iw))/FFT(X(iw))
        }
        if(i!=0)
        {
            vRe.append(G[i].real());
            vIm.append(G[i].imag());
            vImNeg.append(-G[i].imag());
            vBodeGain.append(20*log10(sqrt(G[i].real()*G[i].real() + G[i].imag()*G[i].imag())));  //Gain: abs(G) = sqrt(R^2 + X^2)
            vBodePhaseUncorrected.append(atan2(G[i].imag(), G[i].real())*180./3.14159265);          //Phase: arg(G) = arctan(X/R)

            // Correct the phase plot to make it continous (because atan2 is limited from -180 to +180)
            if(vBodePhaseUncorrected.size() > 1)
            {
                if(vBodePhaseUncorrected.last() > 170 && vBodePhaseUncorrected[vBodePhaseUncorrected.size()-2] < -170)
                    phaseCorrection -= 360;
                else if(vBodePhaseUncorrected.last() < -170 && vBodePhaseUncorrected[vBodePhaseUncorrected.size()-2] > 170)
                    phaseCorrection += 360;
            }
            vBodePhase.append(vBodePhaseUncorrected.last() + phaseCorrection);
        }
    }


    QVector<double> F;
    double stoptime = pInputCurve->getTimeVectorPtr()->last();
    for(int i=1; i<G.size(); ++i)
    {
        F.append((i+1)/stoptime);
        if(F.last() >= Fmax) break;
    }
    vBodeGain.resize(F.size());
    vBodePhase.resize(F.size());


    addPlotTab("Nyquist Plot");
    PlotCurve *pNyquistCurve1 = new PlotCurve(*pOutputCurve->getLogDataVariablePtr()->getVariableCommonDescription().data(),
                                              vRe, vIm, pOutputCurve->getAxisY(),
                                              getCurrentPlotTab(), FirstPlot, NyquistType);
    getCurrentPlotTab()->addCurve(pNyquistCurve1);

    PlotCurve *pNyquistCurve2 = new PlotCurve(*pOutputCurve->getLogDataVariablePtr()->getVariableCommonDescription().data(),
                                              vRe, vImNeg, pOutputCurve->getAxisY(),
                                              getCurrentPlotTab(), FirstPlot, NyquistType);
    getCurrentPlotTab()->addCurve(pNyquistCurve2);
    getCurrentPlotTab()->getPlot()->replot();
    getCurrentPlotTab()->rescaleAxesToCurves();
    getCurrentPlotTab()->updateGeometry();

    addPlotTab("Bode Diagram");
    PlotCurve *pGainCurve = new PlotCurve(*pOutputCurve->getLogDataVariablePtr()->getVariableCommonDescription().data(),
                                          F, vBodeGain, pOutputCurve->getAxisY(),
                                          getCurrentPlotTab(), FirstPlot, BodeGainType);
    getCurrentPlotTab()->addCurve(pGainCurve);

    PlotCurve *pPhaseCurve = new PlotCurve(*pOutputCurve->getLogDataVariablePtr()->getVariableCommonDescription().data(),
                                           F, vBodePhase, pOutputCurve->getAxisY(),
                                           getCurrentPlotTab(), SecondPlot, BodePhaseType);
    getCurrentPlotTab()->addCurve(pPhaseCurve, QColor(), SecondPlot);

    getCurrentPlotTab()->showPlot(SecondPlot, true);
    getCurrentPlotTab()->getPlot(FirstPlot)->replot();
    getCurrentPlotTab()->getPlot(SecondPlot)->replot();
    getCurrentPlotTab()->updateGeometry();

    getCurrentPlotTab()->setBottomAxisLogarithmic(true);

    getCurrentPlotTab()->rescaleAxesToCurves();


    //Add a curve marker at the amplitude margin
    for(int i=1; i<vBodePhase.size(); ++i)
    {
        if(vBodePhase.at(i) < -180 && vBodePhase.at(i-1) > -180)
        {
            QString valueString;
            valueString.setNum(-vBodeGain.at(i));
            getCurrentPlotTab()->insertMarker(pGainCurve, F.at(i), vBodeGain.at(i), "Gain Margin = " + valueString + " dB", false);
            break;
        }
    }

    //Add a curve marker at the phase margin
    for(int i=1; i<vBodeGain.size(); ++i)
    {
        if(vBodeGain.at(i) < -0 && vBodeGain.at(i-1) > -0)
        {
            QString valueString;
            valueString.setNum(180.0+vBodePhase.at(i));
            getCurrentPlotTab()->insertMarker(pPhaseCurve, F.at(i), vBodePhase.at(i), "Phase Margin = " + valueString + trUtf8("°"), false);
            break;
        }
    }

    SharedLogVariableDataPtrT gainVar = gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler()->defineNewVariable("bodegain");
    if(gainVar.isNull())
    {
        gainVar = gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler()->getLogVariableDataPtr("bodegain",-1);
    }
    gainVar.data()->assignFrom(F, vBodeGain);

    SharedLogVariableDataPtrT phaseVar = gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler()->defineNewVariable("bodephase");
    if(phaseVar.isNull())
    {
        phaseVar = gpModelHandler->getCurrentViewContainerObject()->getLogDataHandler()->getLogVariableDataPtr("bodegain",-1);
    }
    phaseVar.data()->assignFrom(F, vBodePhase);
}


//! @brief Shows the help popup with a message for the currently hovered toolbar item
void PlotWindow::showToolBarHelpPopup()
{
    QCursor cursor;
    QAction *pHoveredAction = mpToolBar->actionAt(mpToolBar->mapFromGlobal(cursor.pos()));
    if(pHoveredAction == mpNewPlotButton)
    {
        showHelpPopupMessage("Create a new empty plot tab.");
    }
    else if(pHoveredAction == mpBodePlotButton)
    {
        showHelpPopupMessage("Performs transfer function analysis to generate nyquist plot and bode diagram.");
    }
    else if(pHoveredAction == mpArrowButton)
    {
        showHelpPopupMessage("Enables to select/revert-back-to the Arrow Pointer.");
    }
    else if(pHoveredAction == mpZoomButton)
    {
        showHelpPopupMessage("Enable zooming tool.");
    }
    else if(pHoveredAction == mpPanButton)
    {
        showHelpPopupMessage("Enable panning tool.");
    }
    else if(pHoveredAction == mpSaveButton)
    {
        showHelpPopupMessage("Save plot window to XML.");
    }
    else if(pHoveredAction == mpLoadFromXmlButton)
    {
        showHelpPopupMessage("Load plot window from XML.");
    }
    else if(pHoveredAction == mpGridButton)
    {
        showHelpPopupMessage("Toggle background grid.");
    }
    else if(pHoveredAction == mpBackgroundColorButton)
    {
        showHelpPopupMessage("Change background color.");
    }
    else if(pHoveredAction == mpNewWindowFromTabButton)
    {
        showHelpPopupMessage("Create new plot window from current tab.");
    }
    else if(pHoveredAction == mpResetXVectorButton)
    {
        showHelpPopupMessage("Reset X-vector to simulation time.");
    }
//    else if(pHoveredAction == mpShowCurveInfoButton)
//    {
//        showHelpPopupMessage("Show/hide plot curve control panel.");
//    }
//    else if(pHoveredAction == mpShowPlotWidgetButton)
//    {
//        showHelpPopupMessage("Show/hide variable lists.");
//    }
//    else if(pHoveredAction == mpImportButton)
//    {
//        showHelpPopupMessage("Import Data from Old Hopsan.");
//    }
    else if(pHoveredAction == mpLegendButton)
    {
        showHelpPopupMessage("Open the legend settings dialog.");
    }
    else if(pHoveredAction == mpLocktheAxis)
    {
        showHelpPopupMessage("Open the lock axis dialog.");
    }
    else if(pHoveredAction == mpToggleAxisLockButton)
    {
        showHelpPopupMessage("Lock all axes to current limits.");
    }
}


//! @brief Slot that closes all tabs with no curves, and then closes the entire plot window if it has no curves.
void PlotWindow::closeIfEmpty()
{
    int curves=0;
    for(int i=0; i<mpPlotTabWidget->count(); ++i)
    {
        int nCurvesInTab = 0;
        for(int plotId=0; plotId<2; ++plotId)
        {
            nCurvesInTab += mpPlotTabWidget->getTab(i)->getNumberOfCurves(HopsanPlotIDEnumT(plotId));
        }

        if(nCurvesInTab == 0)
        {
            mpPlotTabWidget->closePlotTab(i);
            --i;
        }

        curves += nCurvesInTab;
    }

    if(curves == 0)
        close();
}

void PlotWindow::closeAllTabs()
{
    mpPlotTabWidget->closeAllTabs();
}


void PlotWindow::hidePlotCurveInfo()
{
    mpCurveInfoDock->toggleViewAction()->setChecked(false);
}


void PlotWindow::setLegendsVisible(bool value)
{
    for(int i=0; i<mpPlotTabWidget->count(); ++i)
    {
        mpPlotTabWidget->getTab(i)->setLegendsVisible(value);
    }
}


void PlotWindow::mouseMoveEvent(QMouseEvent *event)
{
    hideHelpPopupMessage();

    QMainWindow::mouseMoveEvent(event);
}


//! @brief Reimplementation of close function for plot window. Notifies plot widget that window no longer exists.
void PlotWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed(this);
    event->accept();
}

void PlotWindow::changedTab()
{
    // First disconnect all current connections (in case the tab is being changed)
    disconnect(mpZoomButton,                SIGNAL(toggled(bool)),  0,  0);
    disconnect(mpOriginalZoomButton,        SIGNAL(triggered()),    0,  0);
    disconnect(mpArrowButton,               SIGNAL(toggled(bool)),  0,  0);
    disconnect(mpPanButton,                 SIGNAL(toggled(bool)),  0,  0);
    disconnect(mpBackgroundColorButton,     SIGNAL(triggered()),    0,  0);
    disconnect(mpGridButton,                SIGNAL(toggled(bool)),  0,  0);
    disconnect(mpResetXVectorButton,        SIGNAL(triggered()),    0,  0);
    disconnect(mpExportToCsvAction,         SIGNAL(triggered()),    0,  0);
    disconnect(mpExportToHvcAction,         SIGNAL(triggered()),    0,  0);
    disconnect(mpExportToXmlAction,         SIGNAL(triggered()),    0,  0);
    disconnect(mpExportToOldHopAction,      SIGNAL(triggered()),    0,  0);
    disconnect(mpExportToMatlabAction,      SIGNAL(triggered()),    0,  0);
    disconnect(mpExportToGnuplotAction,     SIGNAL(triggered()),    0,  0);
    disconnect(mpExportToGraphicsAction,    SIGNAL(triggered()),    0,  0);
    disconnect(mpLegendButton,              SIGNAL(triggered()),    0,  0);
    disconnect(mpLocktheAxis,               SIGNAL(triggered()),    0,  0);
    disconnect(mpAllGenerationsDown,        SIGNAL(triggered()),    0,  0);
    disconnect(mpAllGenerationsUp,          SIGNAL(triggered()),    0,  0);
    disconnect(mpOpentimeScaleDialog,       SIGNAL(triggered()),    0,  0);
    disconnect(mpToggleAxisLockButton,      SIGNAL(triggered()),    0,  0);

    // If there are any tabs then show the widget and reestablish connections to the current tab
    if(mpPlotTabWidget->count() > 0)
    {
        mpPlotTabWidget->show();
        PlotTab* pCurrentTab = mpPlotTabWidget->getCurrentTab();

        if(pCurrentTab->isSpecialPlot())
        {
            mpArrowButton->setDisabled(true);
            mpZoomButton->setDisabled(true);
            mpOriginalZoomButton->setDisabled(true);
            mpPanButton->setDisabled(true);
            mpSaveButton->setDisabled(true);
            mpExportToCsvAction->setDisabled(true);
            mpExportToHvcAction->setDisabled(true);
            mpExportToGnuplotAction->setDisabled(true);
            mpExportToOldHopAction->setDisabled(true);
            mpExportToMatlabAction->setDisabled(true);
            mpLoadFromXmlButton->setDisabled(true);
            mpGridButton->setDisabled(true);
            mpBackgroundColorButton->setDisabled(true);
            mpNewWindowFromTabButton->setDisabled(true);
            mpResetXVectorButton->setDisabled(true);
            mpBodePlotButton->setDisabled(true);
            mpExportToGraphicsAction->setDisabled(true);
            mpAllGenerationsDown->setDisabled(true);
            mpAllGenerationsUp->setDisabled(true);
        }
        else
        {
            mpArrowButton->setDisabled(false);
            mpZoomButton->setDisabled(false);
            mpOriginalZoomButton->setDisabled(false);
            mpPanButton->setDisabled(false);
            mpImportPloAction->setDisabled(false);
            mpSaveButton->setDisabled(false);
            mpExportToCsvAction->setDisabled(false);
            mpExportToHvcAction->setDisabled(false);
            mpExportToGnuplotAction->setDisabled(false);
            mpExportToOldHopAction->setDisabled(false);
            mpExportToMatlabAction->setDisabled(false);
            mpLoadFromXmlButton->setDisabled(false);
            mpGridButton->setDisabled(false);
            mpBackgroundColorButton->setDisabled(false);
            mpNewWindowFromTabButton->setDisabled(false);
            mpResetXVectorButton->setDisabled(false);
            mpBodePlotButton->setDisabled(false);
            mpExportToGraphicsAction->setDisabled(false);
            mpAllGenerationsDown->setDisabled(false);
            mpAllGenerationsUp->setDisabled(false);
            mpZoomButton->setChecked(pCurrentTab->mpZoomerLeft[FirstPlot]->isEnabled());
            mpPanButton->setChecked(pCurrentTab->mpPanner[FirstPlot]->isEnabled());
            mpGridButton->setChecked(pCurrentTab->mpGrid[FirstPlot]->isVisible());
            mpResetXVectorButton->setEnabled(pCurrentTab->mHasCustomXData);
            mpBodePlotButton->setEnabled(pCurrentTab->getCurves(FirstPlot).size() > 1);
        }

        connect(mpZoomButton,               SIGNAL(toggled(bool)),  pCurrentTab,    SLOT(enableZoom(bool)));
        connect(mpOriginalZoomButton,       SIGNAL(triggered()),    pCurrentTab,    SLOT(resetZoom()));
        connect(mpArrowButton,              SIGNAL(toggled(bool)),  pCurrentTab,    SLOT(enableArrow(bool))); // Arrow
        connect(mpPanButton,                SIGNAL(toggled(bool)),  pCurrentTab,    SLOT(enablePan(bool)));
        connect(mpBackgroundColorButton,    SIGNAL(triggered()),    pCurrentTab,    SLOT(setBackgroundColor()));
        connect(mpGridButton,               SIGNAL(toggled(bool)),  pCurrentTab,    SLOT(enableGrid(bool)));
        connect(mpResetXVectorButton,       SIGNAL(triggered()),    pCurrentTab,    SLOT(resetXTimeVector()));
        connect(mpExportToXmlAction,        SIGNAL(triggered()),    pCurrentTab,    SLOT(exportToXml()));
        connect(mpExportToCsvAction,        SIGNAL(triggered()),    pCurrentTab,    SLOT(exportToCsv()));
        connect(mpExportToHvcAction,        SIGNAL(triggered()),    pCurrentTab,    SLOT(exportToHvc()));
        connect(mpExportToMatlabAction,     SIGNAL(triggered()),    pCurrentTab,    SLOT(exportToMatlab()));
        connect(mpExportToGnuplotAction,    SIGNAL(triggered()),    pCurrentTab,    SLOT(exportToGnuplot()));
        connect(mpExportToOldHopAction,     SIGNAL(triggered()),    pCurrentTab,    SLOT(exportToPLO()));
        connect(mpExportToGraphicsAction,   SIGNAL(triggered()),    pCurrentTab,    SLOT(exportToGraphics()));
        connect(mpLegendButton,             SIGNAL(triggered()),    pCurrentTab,    SLOT(openLegendSettingsDialog()));
        connect(mpLocktheAxis,              SIGNAL(triggered()),    pCurrentTab,    SLOT(openAxisSettingsDialog()));
        connect(mpAllGenerationsDown,       SIGNAL(triggered()),    pCurrentTab,    SLOT(shiftAllGenerationsDown()));
        connect(mpAllGenerationsUp,         SIGNAL(triggered()),    pCurrentTab,    SLOT(shiftAllGenerationsUp()));
        connect(mpOpentimeScaleDialog,      SIGNAL(triggered()),    pCurrentTab,    SLOT(openTimeScalingDialog()));
        connect(mpToggleAxisLockButton,     SIGNAL(triggered()),    pCurrentTab,    SLOT(toggleAxisLock()));

        // Set the plottab specific info layout
        mpCurveInfoStack->setCurrentWidget(pCurrentTab->mpCurveInfoScrollArea);
    }
    else
    {
        mpPlotTabWidget->hide();
    }
}

void PlotWindow::refreshWindowTitle()
{
    if (mModelName.isEmpty())
    {
        setWindowTitle(mName);
    }
    else
    {
        setWindowTitle(mName + " (" + mModelName + ")");
    }
}


//! @brief Slot that updates the palette to match the one used in main window
void PlotWindow::updatePalette()
{
    setPalette(gpMainWindow->palette());
}


//! @brief Creates a new plot window and adds the curves from current plot tab
void PlotWindow::createPlotWindowFromTab()
{
    //! @todo should be in tab instead and have signal slot
    PlotWindow *pPW = 0;
    for(int i=0; i<getCurrentPlotTab()->getCurves().size(); ++i)
    {
        //pPlotWindow->addPlotCurve(getCurrentPlotTab()->getCurves().at(i)->getGeneration(), getCurrentPlotTab()->getCurves().at(i)->getComponentName(), getCurrentPlotTab()->getCurves().at(i)->getName(), getCurrentPlotTab()->getCurves().at(i)->getDataName(), getCurrentPlotTab()->getCurves().at(i)->getDataUnit(), getCurrentPlotTab()->getCurves().at(i)->getAxisY());
        pPW = gpPlotHandler->plotDataToWindow(pPW,getCurrentPlotTab()->getCurves().at(i)->getLogDataVariablePtr(), getCurrentPlotTab()->getCurves().at(i)->getAxisY());
    }
}


