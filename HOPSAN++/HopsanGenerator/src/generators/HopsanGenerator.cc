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
//! @file   ComponentGeneratorUtilities.cc
//! @author Robert Braun <robert.braun@liu.se
//! @date   2012-01-08
//!
//! @brief Contains component generation utiluties
//!
//$Id$

#include <QStringList>
#include <QProcess>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QApplication>
#include <QPushButton>
#include <QProgressDialog>
#include <QDomElement>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>

#include <cassert>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "generators/HopsanGenerator.h"
#include "symhop/SymHop.h"

#include "ComponentSystem.h"
#include "Port.h"
#include "version.h"


using namespace std;
using namespace SymHop;
using namespace hopsan;


HopsanGenerator::HopsanGenerator(QString coreIncludePath, QString binPath, bool showDialog)
{
#ifdef WIN32
    mOutputPath = "C:/HopsanGeneratorTempFiles/output/";
    mTempPath = "C:/HopsanGeneratorTempFiles/temp/";
#else
    mOutputPath = QDir::currentPath()+"/output/";
    mTempPath = QDir::currentPath()+"/temp/";
#endif
    mCoreIncludePath = coreIncludePath;
    mBinPath = binPath;

    mExecPath = qApp->applicationDirPath().append('/');

    mShowDialog = showDialog;

    if(mShowDialog)
    {
        mpTextEdit = new QTextEdit();
        mpTextEdit->setReadOnly(true);
        QFont monoFont = QFont("Monospace", 10, 50);
        monoFont.setStyleHint(QFont::TypeWriter);
        mpTextEdit->setFont(monoFont);

        mpDoneButton = new QPushButton("Close");
        mpDoneButton->setFixedWidth(200);

        mpLayout = new QVBoxLayout();
        mpLayout->addWidget(mpTextEdit);
        mpLayout->addWidget(mpDoneButton);
        mpLayout->setAlignment(mpDoneButton, Qt::AlignCenter);

        mpDialog = new QWidget(0);
        mpDialog->setWindowModality(Qt::ApplicationModal);
        mpDialog->setLayout(mpLayout);
        mpDialog->setMinimumSize(640, 480);
        mpDialog->setWindowTitle("HopsanGenerator");

        mpDoneButton->connect(mpDoneButton, SIGNAL(clicked()), mpDialog, SLOT(hide()));

        mpDialog->show();
        QApplication::processEvents();

        printMessage("##########################\n# Loaded HopsanGenerator #\n##########################\n");
    }
}


void HopsanGenerator::printMessage(QString msg)
{
    if(mShowDialog)
    {
        mpTextEdit->setTextColor(QColor("Black"));
        mpTextEdit->append(msg);
        QApplication::processEvents();
#ifdef WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
    }
}


void HopsanGenerator::printErrorMessage(QString msg)
{
    if(mShowDialog)
    {
        mpTextEdit->setTextColor(QColor("Red"));
        mpTextEdit->append(msg);
        QApplication::processEvents();
#ifdef WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
    }
}















QString HopsanGenerator::generateSourceCodefromComponentObject(ComponentSpecification comp, bool overwriteStartValues)
{
    if(comp.cqsType == "S") { comp.cqsType = "Signal"; }


    QString code;
    QTextStream codeStream(&code);

    codeStream << "#ifndef " << comp.typeName.toUpper() << "_HPP_INCLUDED\n";
    codeStream << "#define " << comp.typeName.toUpper() << "_HPP_INCLUDED\n\n";
    codeStream << "#include <math.h>\n";
    codeStream << "#include \"ComponentEssentials.h\"\n";
    codeStream << "#include \"ComponentUtilities.h\"\n";
    codeStream << "#include <sstream>\n\n";
    codeStream << "using namespace std;\n\n";
    codeStream << "namespace hopsan {\n\n";
    codeStream << "    class " << comp.typeName << " : public Component" << comp.cqsType << "\n";
    codeStream << "    {\n";
    codeStream << "    private:\n";                         // Private section
    codeStream << "        double ";
    int portId=1;

    for(int i=0; i<comp.portNames.size(); ++i)              //Declare variables
    {
        QStringList varNames;
        if(comp.portNodeTypes[i] == "NodeSignal")
        {
            varNames << comp.portNames[i];
        }
        else
        {
            varNames << getQVariables(comp.portNodeTypes[i]) << getCVariables(comp.portNodeTypes[i]);
        }

        for(int v=0; v<varNames.size()-1; ++v)
        {
            QString varName;
            if(comp.portNodeTypes[i] == "NodeSignal")
                varName = varNames[v];
            else
                varName = varNames[v] + QString::number(portId);
            codeStream << varName << ", ";
        }
        QString varName;
        if(comp.portNodeTypes[i] == "NodeSignal")
            varName = varNames.last();
        else
            varName = varNames.last() + QString::number(portId);
        codeStream << varName;
        ++portId;
        if(i < comp.portNames.size()-1)
        {
            codeStream << ", ";
        }
    }

    codeStream << ";\n";
    for(int i=0; i<comp.parNames.size(); ++i)                   //Declare parameters
    {
        codeStream << "        double " << comp.parNames[i] << ";\n";
    }
    for(int i=0; i<comp.varNames.size(); ++i)
    {
        codeStream << "        " << comp.varTypes[i] << " " << comp.varNames[i] << ";\n";
    }
    for(int i=0; i<comp.utilities.size(); ++i)
    {
        codeStream << "        " << comp.utilities[i] << " " << comp.utilityNames[i] << ";\n";
    }
    codeStream << "        double ";
    portId=1;
    QStringList allVarNames;                                    //Declare node data pointers
    for(int i=0; i<comp.portNames.size(); ++i)
    {
        QString id = QString::number(portId);
        if(comp.portNodeTypes[i] == "NodeSignal")
        {
            allVarNames << comp.portNames[i];
        }
        else
        {
            QStringList vars;
            vars << getQVariables(comp.portNodeTypes[i]) << getCVariables(comp.portNodeTypes[i]);

            for(int v=0; v<vars.size(); ++v)
            {
                allVarNames << vars[v]+id;
            }
        }
        ++portId;
    }

    if(!allVarNames.isEmpty())
    {
        codeStream << "*mpND_" << allVarNames[0];
        for(int i=1; i<allVarNames.size(); ++i)
        {
            codeStream << ", *mpND_" << allVarNames[i];
        }
    }

    codeStream << ";\n";
    codeStream << "        Port ";                              //Declare ports
    for(int i=0; i<comp.portNames.size(); ++i)
    {
        codeStream << "*mp" << comp.portNames[i];
        if(i<comp.portNames.size()-1)
        {
            codeStream << ", ";
        }
    }

    codeStream << ";\n\n";
    codeStream << "    public:\n";                              //Public section
    codeStream << "        static Component *Creator()\n";
    codeStream << "        {\n";
    codeStream << "            return new " << comp.typeName << "();\n";
    codeStream << "        }\n\n";
    codeStream << "        " << comp.typeName << "() : Component" << comp.cqsType << "()\n";
    codeStream << "        {\n";
    for(int i=0; i<comp.parNames.size(); ++i)
    {
        codeStream << "            " << comp.parNames[i] << " = " << comp.parInits[i] << ";\n";
    }
    codeStream << "\n";

    for(int i=0; i<comp.parNames.size(); ++i)
    {
        codeStream << "            registerParameter(\"" << comp.parDisplayNames[i] << "\", \""
                   << comp.parDescriptions[i] << "\", \"" << comp.parUnits[i] << "\", " << comp.parNames[i] << ");\n";
    }
    codeStream << "\n";
    for(int i=0; i<comp.portNames.size(); ++i)
    {

        codeStream << "            mp" << comp.portNames[i] << " = add" << comp.portTypes[i]
                   << "(\"" << comp.portNames[i] << "\", \"" << comp.portNodeTypes[i] << "\"";
        if(comp.portNotReq[i])
        {
            codeStream << ", Port::NOTREQUIRED);\n";
        }
        else
        {
            codeStream << ");\n";
        }
    }

    codeStream << "        }\n\n";
    codeStream << "        void initialize()\n";
    codeStream << "        {\n";
    portId=1;
    for(int i=0; i<comp.portNames.size(); ++i)
    {
        QStringList varNames;
        QStringList varLabels;
        if(comp.portNodeTypes[i] == "NodeSignal")
        {
            varNames << comp.portNames[i];
            varLabels << "VALUE";
        }
        else
        {
            varNames << getQVariables(comp.portNodeTypes[i]) << getCVariables(comp.portNodeTypes[i]);
            varLabels << getVariableLabels(comp.portNodeTypes[i]);
        }

        for(int v=0; v<varNames.size(); ++v)
        {
            QString varName;
            if(comp.portNodeTypes[i] == "NodeSignal")
                varName = varNames[v];
            else
                varName = varNames[v]+QString::number(portId);
            codeStream << "            mpND_" << varName << " = getSafeNodeDataPtr(mp" << comp.portNames[i] << ", " << comp.portNodeTypes[i] << "::" << varLabels[v];
            if(comp.portNotReq[i])
            {
                codeStream << ", " << comp.portDefaults[i];
            }
            codeStream << ");\n";
        }
        ++portId;
    }

    codeStream << "\n";
    if(!comp.initEquations.isEmpty())
    {
        portId=1;
        for(int i=0; i<comp.portNames.size(); ++i)
        {
            QStringList varNames;
            if(comp.portNodeTypes[i] == "NodeSignal")
            {
                varNames << comp.portNames[i];
            }
            else
            {
                varNames << getQVariables(comp.portNodeTypes[i]) << getCVariables(comp.portNodeTypes[i]);
            }

            for(int v=0; v<varNames.size(); ++v)
            {
                QString varName;
                if(comp.portNodeTypes[i] == "NodeSignal")
                    varName = varNames[v];
                else
                    varName = varNames[v] + QString::number(portId);
                codeStream << "            " << varName << " = (*mpND_" << varName << ");\n";
            }
            ++portId;
        }
        codeStream << "\n";
        for(int i=0; i<comp.initEquations.size(); ++i)
        {
            codeStream << "            " << comp.initEquations[i] << "\n";
        }
        if(overwriteStartValues)
        {
            codeStream << "\n";
            portId=1;
            for(int i=0; i<comp.portNames.size(); ++i)
            {
                QStringList varNames;
                if(comp.portNodeTypes[i] == "NodeSignal" && comp.portTypes[i] == "WritePort")
                {
                    varNames << comp.portNames[i];
                }
                if(comp.portNodeTypes[i] != "NodeSignal" && (comp.cqsType == "Q" || comp.cqsType == "S"))
                {
                    varNames << getQVariables(comp.portNodeTypes[i]);
                }
                if(comp.portNodeTypes[i] != "NodeSignal" && (comp.cqsType == "C" || comp.cqsType == "S"))
                {
                    varNames << getCVariables(comp.portNodeTypes[i]);
                }
                for(int v=0; v<varNames.size(); ++v)
                {
                    QString varName;
                    if(comp.portNodeTypes[i] == "NodeSignal")
                        varName = varNames[v];
                    else
                        varName = varNames[v] + QString::number(portId);
                    codeStream << "            (*mpND_" << varName << ") = " << varName << ";\n";
                }
            }
            ++portId;
        }
    }
    codeStream << "        }\n\n";

    //Simulate one time step
    codeStream << "        void simulateOneTimestep()\n";
    codeStream << "        {\n";
    portId=1;
    for(int i=0; i<comp.portNames.size(); ++i)
    {
        QStringList varNames;
        if(comp.portNodeTypes[i] == "NodeSignal" && comp.portTypes[i] == "ReadPort")
        {
            varNames << comp.portNames[i];
        }
//        if(comp.portNodeTypes[i] != "NodeSignal" && (comp.cqsType == "C" || comp.cqsType == "S"))
//        {
//            varNames << getQVariables(comp.portNodeTypes[i]);
//        }
//        if(comp.portNodeTypes[i] != "NodeSignal" && (comp.cqsType == "Q" || comp.cqsType == "S"))
//        {
//            varNames << getCVariables(comp.portNodeTypes[i]);
//        }
        else
        {
            varNames << getQVariables(comp.portNodeTypes[i]);       //Always create both C- and Q-type variables, regaradless of component type (they may be needed)
            varNames << getCVariables(comp.portNodeTypes[i]);
        }

        for(int v=0; v<varNames.size(); ++v)
        {
            QString varName;
            if(comp.portNodeTypes[i] == "NodeSignal")
                varName = varNames[v];
            else
                varName = varNames[v] + QString::number(portId);
            codeStream << "            " << varName << " = (*mpND_" << varName << ");\n";
        }
        ++portId;
    }

    codeStream << "\n";
    for(int i=0; i<comp.simEquations.size(); ++i)
    {
        codeStream << "            " << comp.simEquations[i] << "\n";
    }
    codeStream << "\n";
    portId=1;
    for(int i=0; i<comp.portNames.size(); ++i)
    {
        QStringList varNames;
        if(comp.portNodeTypes[i] == "NodeSignal" && comp.portTypes[i] == "WritePort")
        {
            varNames << comp.portNames[i];
        }
        if(comp.portNodeTypes[i] != "NodeSignal" && (comp.cqsType == "Q" || comp.cqsType == "S"))
        {
            varNames << getQVariables(comp.portNodeTypes[i]);
        }
        if(comp.portNodeTypes[i] != "NodeSignal" && (comp.cqsType == "C" || comp.cqsType == "S"))
        {
            varNames << getCVariables(comp.portNodeTypes[i]);
        }

        for(int v=0; v<varNames.size(); ++v)
        {
            QString varName;
            if(comp.portNodeTypes[i] == "NodeSignal")
                varName = varNames[v];
            else
                varName = varNames[v] + QString::number(portId);
            codeStream << "            (*mpND_" << varName << ") = " << varName << ";\n";
        }
        ++portId;
    }

    codeStream << "        }\n\n";
    codeStream << "        void finalize()\n";
    codeStream << "        {\n";
    for(int i=0; i<comp.finalEquations.size(); ++i)
    {
        codeStream << "            " << comp.finalEquations[i] << "\n";
    }
    codeStream << "        }\n\n";
    codeStream << "    };\n";
    codeStream << "}\n\n";

    codeStream << "#endif // " << comp.typeName.toUpper() << "_HPP_INCLUDED\n";

    return code;
}







//! @brief Generates and compiles component source code from a ComponentSpecification object
//! @param outputFile Name of output file
//! @param comp Component specification object
//! @param overwriteStartValues Tells whether or not this components overrides the built-in start values or not
void HopsanGenerator::compileFromComponentObject(QString outputFile, ComponentSpecification comp, /*ModelObjectAppearance appearance,*/ bool overwriteStartValues)
{
    QString code;

    if(comp.plainCode.isEmpty())
    {
        code = generateSourceCodefromComponentObject(comp, overwriteStartValues);
    }
    else
    {
        code = comp.plainCode;
    }

    qDebug() << "Code: " << code;

    if(!QDir(mTempPath).exists())
    {
        QDir().mkpath(mTempPath);
    }

    if(!QDir(mOutputPath).exists())
    {
        QDir().mkpath(mOutputPath);
    }

    printMessage("Writing "+outputFile+"...");

    //Initialize the file stream
    QFileInfo fileInfo;
    QFile file;
    fileInfo.setFile(QString(mTempPath)+outputFile);
    file.setFileName(fileInfo.filePath());   //Create a QFile object
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        printErrorMessage("Failed to open file for writing: " + outputFile);
        return;
    }
    QTextStream fileStream(&file);  //Create a QTextStream object to stream the content of file

    fileStream << code;

    file.close();

    printMessage("Writing tempLib.cc...");

    QFile ccLibFile;
    ccLibFile.setFileName(QString(mTempPath)+"tempLib.cc");
    if(!ccLibFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        printErrorMessage("Failed to open tempLib.cc for writing.");
        return;
    }
    QTextStream ccLibStream(&ccLibFile);
    ccLibStream << "#include \"" << outputFile << "\"\n";
    ccLibStream << "#include \"ComponentEssentials.h\"\n\n";
    ccLibStream << "using namespace hopsan;\n\n";
    ccLibStream << "extern \"C\" DLLEXPORT void register_contents(ComponentFactory* cfact_ptr, NodeFactory* /*nfact_ptr*/)\n";
    ccLibStream << "{\n";
    ccLibStream << "    cfact_ptr->registerCreatorFunction(\"" << comp.typeName<< "\", " << comp.typeName << "::Creator);\n";
    ccLibStream << "}\n\n";
    ccLibStream << "extern \"C\" DLLEXPORT void get_hopsan_info(HopsanExternalLibInfoT *pHopsanExternalLibInfo)\n";
    ccLibStream << "{\n";
    ccLibStream << "    pHopsanExternalLibInfo->libName = (char*)\"HopsanGeneratedComponent_"+comp.typeName+"\";\n";
    ccLibStream << "    pHopsanExternalLibInfo->hopsanCoreVersion = (char*)HOPSANCOREVERSION;\n";
    ccLibStream << "    pHopsanExternalLibInfo->libCompiledDebugRelease = (char*)DEBUGRELEASECOMPILED;\n";
    ccLibStream << "}\n";
    ccLibFile.close();

    printMessage("Writing " + comp.typeName + ".xml...");

    //! @todo Fix appearance file generation without ModelObjectAppearance
    QFile xmlFile;
    xmlFile.setFileName(QString(mOutputPath)+comp.typeName+".xml");
    if(!xmlFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        printErrorMessage("Failed to open " + comp.typeName + ".xml  for writing.");
        return;
    }
    QTextStream xmlStream(&xmlFile);
    xmlStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlStream << "<hopsanobjectappearance version=\"0.3\">\n";
    xmlStream << "  <modelobject typename=\"" << comp.typeName << "\" displayname=\"" << comp.displayName << "\">\n";
    xmlStream << "    <icons/>\n";
    xmlStream << "    <ports>\n";
    double xDelay = 1.0/(comp.portNames.size()+1.0);
    double xPos = xDelay;
    double yPos = 0;
    for(int i=0; i<comp.portNames.size(); ++i)
    {
        xmlStream << "      <port name=\"" << comp.portNames[i] << "\" x=\"" << xPos << "\" y=\"" << yPos << "\" a=\"" << 270 << "\"/>\n";
        xPos += xDelay;
    }
    xmlStream << "    </ports>\n";
    xmlStream << "  </modelobject>\n";
    xmlStream << "</hopsanobjectappearance>\n";
    xmlFile.close();

    QString libFileName = comp.typeName;
#ifdef WIN32
    libFileName.append(".dll");
#else
    libFileName.append(".so");
#endif

    compileComponentLibrary(mTempPath, comp.typeName, this);

    printMessage("Moving files to output directory...");

    QFile soFile(mTempPath+libFileName);
    QFile::remove(mOutputPath + libFileName);
    soFile.copy(mOutputPath + libFileName);
}


QString HopsanGenerator::getCoreIncludePath()
{
    return mCoreIncludePath;
}


QString HopsanGenerator::getBinPath()
{
    return mBinPath;
}


bool HopsanGenerator::assertFilesExist(QString path, QStringList files)
{
    Q_FOREACH(const QString file, files)
    {
        if(!QFile::exists(path+"/"+file))
        {
            printErrorMessage("File not found: "+file+".");
            return false;
        }
    }
    printMessage("All required files found.");
    return true;
}


void HopsanGenerator::callProcess(QString name, QStringList args, QString workingDirectory)
{
    QProcess p;
    if(!workingDirectory.isEmpty())
    {
        p.setWorkingDirectory(workingDirectory);
    }
    p.start(name, args);
    p.waitForFinished(60000);
    printMessage(p.readAll());
}


bool HopsanGenerator::runUnixCommand(QString cmd)
{
    char line[130];
    cmd +=" 2>&1";
    FILE *fp = popen(  (const char *) cmd.toStdString().c_str(), "r");
    if ( !fp )
    {
        printErrorMessage("Could not execute '" + cmd + "'! err=%d");
        return false;
    }
    else
    {
        while ( fgets( line, sizeof line, fp))
        {
           printMessage((const QString &)line);
        }
    }
    return true;
}


//! @warning May delete contents in file if it fails to open in write mode
bool HopsanGenerator::replaceInFile(const QString &fileName, const QStringList &before, const QStringList &after)
{
    QFile file(fileName);
    printMessage("Replacing string in file: "+file.fileName());

    Q_ASSERT(before.size() == after.size());

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        printErrorMessage("Unable to open file: "+fileName+" for reading.");
        return false;
    }
    QString contents = file.readAll();
    file.close();

    bool didSomething = false;
    for(int i=0; i<before.size(); ++i)
    {
        if(contents.contains(before[i]))
        {
            didSomething = true;
            contents.replace(before[i], after[i]);
        }

    }

    if(!didSomething)
    {
        return true;
    }

    if(!file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text))
    {
        printErrorMessage("Unable to open file "+fileName+" for writing.");
    }
    file.write(contents.toAscii());
    file.close();

    return true;
}
