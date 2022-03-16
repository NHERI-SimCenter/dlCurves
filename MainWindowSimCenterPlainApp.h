#ifndef MAIN_WINDOW_SIMCENTER_PLAIN_APP_H
#define MAIN_WINDOW_SIMCENTER_PLAIN_APP_H

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

#include <QMainWindow>

class SimCenterWidget;
class QLabel;
class QLineEdit;
//class PythonProgressDialog;

class MainWindowSimCenterPlainApp : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindowSimCenterPlainApp(QString appName,
                                         SimCenterWidget *theApp,
                                         QWidget *parent = 0);
    ~MainWindowSimCenterPlainApp();
    void setCopyright(QString &copyright);
    void setVersion(QString &version);
    void setDocumentationURL(QString &version);
    void setAbout(QString &about);
    void setAbout(QString &, QString&);
    void setFeedbackURL(QString &feedback);
    void setCite(QString &cite);

protected:

signals:
    void attemptLogin(QString, QString);
    void logout();

    void sendStatusMessage(QString message);
    void sendErrorMessage(QString message);
    void sendFatalMessage(QString message);
    void sendInfoMessage(QString message);

public slots:
    // for menu items
    void newFile();
    void open();
    void openFile(QString filename);
    bool save();
    bool saveAs();
    void clear();

    void about();
    void manual();
    void submitFeedback();
    void version();
    void copyright();
    void cite();
    virtual void loadExamples();

private:
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(QString &fileName);

    void createActions();
    
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QMenu *exampleMenu;

    QString currentFile;


    QString versionText;
    QString aboutText;
    QString aboutTitle;
    QString aboutSource;
    QString copyrightText;
    QString manualURL;
    QString feedbackURL;
    QString citeText;

    //  QDockWidget* statusDockWidget;
    SimCenterWidget *inputWidget;
    //    PythonProgressDialog* statusWidget;
};

#endif // MAIN_WINDOW_SIMCENTER_PLAIN_APP_H
