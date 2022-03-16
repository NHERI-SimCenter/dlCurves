/* *****************************************************************************
Copyright (c) 2022-2023, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna



#include <FooterWidget.h>
#include <HeaderWidget.h>
#include "MainWindowSimCenterPlainApp.h"
#include <SimCenterWidget.h>
#include <dialogabout.h>

#include <QDebug>

//#include <InputWidgetEE_UQ.h>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QApplication>
#include <QDockWidget>
#include <QDesktopWidget>
#include <QDesktopServices>


MainWindowSimCenterPlainApp::MainWindowSimCenterPlainApp(QString appName,
                                                         SimCenterWidget *theWidget,
                                                         QWidget *parent)
    : QMainWindow(parent), inputWidget(theWidget)
{
    //
    // create a layout & widget for central area of this QMainWidget
    //  to this widget we will add a header, selection, button and footer widgets
    //

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    centralWidget->setLayout(layout);
    centralWidget->setContentsMargins(0,0,0,0);

    QRect rec = QGuiApplication::primaryScreen()->geometry();
    int height = this->height()<int(rec.height())?int(rec.height()):this->height();
    int width  = this->width()<int(rec.width())?int(rec.width()):this->width();
    this->resize(width, height);

    // add header
    HeaderWidget *header = new HeaderWidget();
    header->setHeadingText(appName);
    layout->addWidget(header);

    layout->addWidget(theWidget);
    
    // add footer
    layout->setSpacing(0);
    FooterWidget *footer = new FooterWidget();
    layout->addWidget(footer);

    
    this->setCentralWidget(centralWidget);

    this->createActions();

    //    inputWidget->setMainWindow(this);

    //
    // strings needed in about menu, use set methods to override
    //

    manualURL = QString("");
    feedbackURL = QString("https://docs.google.com/forms/d/e/1FAIpQLSfh20kBxDmvmHgz9uFwhkospGLCeazZzL770A2GuYZ2KgBZBA/viewform");
    //    featureRequestURL = QString("https://docs.google.com/forms/d/e/1FAIpQLScTLkSwDjPNzH8wx8KxkyhoIT7AI9KZ16Wg9TuW1GOhSYFOag/viewform");
    versionText = QString("");
    citeText = QString("");
    aboutText = QString(tr("This is a SimCenter Workflow Applicatios"));

    aboutTitle = "About this SimCenter Application"; // this is the title displayed in the on About dialog
    aboutSource = ":/Resources/docs/textAbout.html";  // this is an HTML file stored under resources

    copyrightText = QString("\
                            <p>\
                            The source code is licensed under a BSD 2-Clause License:<p>\
                            \"Copyright (c) 2017-2018, The Regents of the University of California (Regents).\"\
                            All rights reserved.<p>\
                            <p>\
                            Redistribution and use in source and binary forms, with or without \
                            modification, are permitted provided that the following conditions are met:\
                            <p>\
                            1. Redistributions of source code must retain the above copyright notice, this\
                            list of conditions and the following disclaimer.\
                            \
                            \
                            2. Redistributions in binary form must reproduce the above copyright notice,\
                            this list of conditions and the following disclaimer in the documentation\
                            and/or other materials provided with the distribution.\
                            <p>\
                            THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\
                            ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\
                            WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\
                            DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR\
                            ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\
                            (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\
                            LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\
            ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\
            (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\
            SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\
            <p>\
            The views and conclusions contained in the software and documentation are those\
            of the authors and should not be interpreted as representing official policies,\
            either expressed or implied, of the FreeBSD Project.\
            <p>\
            REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, \
            THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\
            THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS \
            PROVIDED \"AS IS\". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,\
            UPDATES, ENHANCEMENTS, OR MODIFICATIONS.\
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            The compiled binary form of this application is licensed under a GPL Version 3 license.\
            The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
            included in the packaging of this application. \
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            This software makes use of the QT packages (unmodified): core, gui, widgets and network\
                                                                     <p>\
                                                                     QT is copyright \"The Qt Company Ltd&quot; and licensed under the GNU Lesser General \
                                                                     Public License (version 3) which references the GNU General Public License (version 3)\
      <p>\
      The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
      included in the packaging of this application. \
      <p>\
      ");


}

      MainWindowSimCenterPlainApp::~MainWindowSimCenterPlainApp()
    {

    }


    bool MainWindowSimCenterPlainApp::save()
    {
        if (currentFile.isEmpty()) {
            return saveAs();
        } else {
            return saveFile(currentFile);
        }
    }


    bool MainWindowSimCenterPlainApp::saveAs()
    {
        //
        // get filename
        //

        QFileDialog dialog(this, "Save Simulation Model");
        //dialog.setWindowModality(Qt::WindowModal);
        dialog.setAcceptMode(QFileDialog::AcceptSave);

        QStringList filters;
        filters << "Json files (*.json)"
            << "All files (*)";

        dialog.setNameFilters(filters);

        if (dialog.exec() == QDialog::Rejected) {
            return false;
        }


        return saveFile(dialog.selectedFiles().first());

    }


    void MainWindowSimCenterPlainApp::open()
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Simulation Model", "",  "Json files (*.json);;All files (*)");
        if (!fileName.isEmpty())
        {
            emit sendStatusMessage("Loading file "+fileName);
            loadFile(fileName);
        }
    }


    void MainWindowSimCenterPlainApp::openFile(QString fileName)
    {
        if (!fileName.isEmpty())
            loadFile(fileName);
    }


    void MainWindowSimCenterPlainApp::newFile()
    {
        // set currentFile blank
        setCurrentFile(QString());
    }


    void MainWindowSimCenterPlainApp::setCurrentFile(const QString &fileName)
    {
        currentFile = fileName;
        //  setWindowModified(false);

        QString shownName = currentFile;
        if (currentFile.isEmpty())
            shownName = "untitled.json";

        setWindowFilePath(shownName);
    }


    bool MainWindowSimCenterPlainApp::saveFile(const QString &fileName)
    {
        //
        // open file
        //

        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Application"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file.errorString()));
            return false;
        }


        //
        // create a json object, fill it in & then use a QJsonDocument
        // to write the contents of the object to the file in JSON format
        //

        QJsonObject json;
        inputWidget->outputToJSON(json);

        //Resolve relative paths before saving
        QFileInfo fileInfo(fileName);
        //    SCUtils::ResolveRelativePaths(json, fileInfo.dir());

        QJsonDocument doc(json);
        file.write(doc.toJson());

        // close file
        file.close();

        // set current file
        setCurrentFile(fileName);

        return true;
    }


    void MainWindowSimCenterPlainApp::loadFile(QString &fileName)
    {
        //inputWidget->loadFile(fileName);
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            sendErrorMessage(QString("Could Not Open File: ") + fileName);
        }

        //
        // place contents of file into json object
        //

        QString val;
        val=file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonObj = doc.object();

        // close file
        file.close();

        //
        // clear current and input from new JSON
        //


        bool result = inputWidget->inputFromJSON(jsonObj);
    }


    void MainWindowSimCenterPlainApp::createActions() {
        QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

        QAction *openAction = new QAction(tr("&Open"), this);
        openAction->setShortcuts(QKeySequence::Open);
        openAction->setStatusTip(tr("Open an existing file"));
        connect(openAction, &QAction::triggered, this, &MainWindowSimCenterPlainApp::open);
        fileMenu->addAction(openAction);
        //fileToolBar->addAction(openAction);


        QAction *saveAction = new QAction(tr("&Save"), this);
        saveAction->setShortcuts(QKeySequence::Save);
        saveAction->setStatusTip(tr("Save the document to disk"));
        connect(saveAction, &QAction::triggered, this, &MainWindowSimCenterPlainApp::save);
        fileMenu->addAction(saveAction);

        QAction *saveAsAction = new QAction(tr("&Save As"), this);
        saveAsAction->setStatusTip(tr("Save the document with new filename to disk"));
        connect(saveAsAction, &QAction::triggered, this, &MainWindowSimCenterPlainApp::saveAs);
        fileMenu->addAction(saveAsAction);

        QAction *exitAction = new QAction(tr("&Quit"), this);
        connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        exitAction->setStatusTip(tr("Exit the application"));
        fileMenu->addAction(exitAction);

        // Add the help menu last
        QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
        helpMenu->addAction(tr("&Version"), this, &MainWindowSimCenterPlainApp::version);
        helpMenu->addAction(tr("&About"), this, &MainWindowSimCenterPlainApp::about);
        helpMenu->addAction(tr("&Manual"), this, &MainWindowSimCenterPlainApp::manual);
        helpMenu->addAction(tr("&Submit Bug/Feature Request"), this, &MainWindowSimCenterPlainApp::submitFeedback);
        // QAction *submitFeature = helpMenu->addAction(tr("&Submit Bug/Feature Request"), this, &MainWindowSimCenterPlainApp::submitFeatureRequest);
        helpMenu->addAction(tr("&How to Cite"), this, &MainWindowSimCenterPlainApp::cite);
        helpMenu->addAction(tr("&License"), this, &MainWindowSimCenterPlainApp::copyright);
    }


    void MainWindowSimCenterPlainApp::version()
    {
        QMessageBox msgBox;
        QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        msgBox.setText(versionText);
        QGridLayout *layout = (QGridLayout*)msgBox.layout();
        layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
        msgBox.exec();
    }


    void MainWindowSimCenterPlainApp::cite()
    {
        QMessageBox msgBox;
        QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        msgBox.setText(citeText);
        QGridLayout *layout = (QGridLayout*)msgBox.layout();
        layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
        msgBox.exec();
    }


    void MainWindowSimCenterPlainApp::about()
    {

        DialogAbout *dlg = new DialogAbout();
        dlg->setTitle(aboutTitle);
        dlg->setTextSource(aboutSource);

        //
        // adjust size of application window to the available display
        //

        QRect rec = QApplication::desktop()->screenGeometry(this);
        int height = 0.50*rec.height();
        int width  = 0.50*rec.width();
        dlg->resize(width, height);

        dlg->exec();
        delete dlg;
    }

    void MainWindowSimCenterPlainApp::submitFeedback()
    {
        QDesktopServices::openUrl(QUrl(feedbackURL, QUrl::TolerantMode));
        //QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
    }


    void MainWindowSimCenterPlainApp::manual()
    {
        QDesktopServices::openUrl(QUrl(manualURL, QUrl::TolerantMode));
        //QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
    }


    void MainWindowSimCenterPlainApp::copyright()
    {
        QMessageBox msgBox;
        QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        msgBox.setText(copyrightText);
        QGridLayout *layout = (QGridLayout*)msgBox.layout();
        layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
        msgBox.exec();

    }


    void
            MainWindowSimCenterPlainApp::setCopyright(QString &newText)
    {
        copyrightText = newText;
    }

    void
            MainWindowSimCenterPlainApp::setVersion(QString &newText)
    {
        versionText = newText;
    }

    void
            MainWindowSimCenterPlainApp::setAbout(QString &newText)
    {
        aboutText = newText +QString("<p> This work is based on material supported by the National Science Foundation under grant 1612843<p>");
        //qDebug() << "ABOUT: " << aboutText;
    }

    void
            MainWindowSimCenterPlainApp::setAbout(QString &newTitle, QString &newTextSource)
    {
        aboutTitle  = newTitle;
        aboutSource = newTextSource;
    }

    void
            MainWindowSimCenterPlainApp::setDocumentationURL(QString &newText)
    {
        manualURL = newText;
    }

    void
            MainWindowSimCenterPlainApp::setFeedbackURL(QString &newText)
    {
        feedbackURL = newText;
    }

    void
            MainWindowSimCenterPlainApp::setCite(QString &newText)
    {
        citeText = newText;
    }

    void
            MainWindowSimCenterPlainApp::loadExamples()
    {
        QObject* senderObj = QObject::sender();

        if(senderObj == nullptr)
            return;

        auto pathToExample = QCoreApplication::applicationDirPath() + QDir::separator() + "Examples" + QDir::separator();
        pathToExample += senderObj->property("inputFile").toString();

        if(pathToExample.isNull())
        {
            QString msg = "Error loading example "+pathToExample;
            sendErrorMessage(msg);
            return;
        }

        // Clear current and input
        currentFile.clear();

        auto exampleName = senderObj->property("name").toString();
        emit sendStatusMessage("Loading example "+exampleName);
        emit sendStatusMessage(" - reading input file "+pathToExample);

        auto description = senderObj->property("description").toString();

        if(!description.isEmpty())
            emit sendInfoMessage(description);

        //    statusWidget->showProgressBar();
        QApplication::processEvents();

        emit sendStatusMessage("Loading Example file. Wait till Done Loading appears before progressing.");
        this->loadFile(pathToExample);

        // statusWidget->hideProgressBar();
        emit sendStatusMessage("Done Loading.");

        // Automatically hide after n seconds
        // progressDialog->hideAfterElapsedTime(4);
    }

    void
            MainWindowSimCenterPlainApp::clear()
    {

    }



