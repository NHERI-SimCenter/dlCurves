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

#include "dlCurves.h"
#include "Fragility.h"
#include "FragilityDisplay.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QVariant>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QGroupBox>
#include <QComboBox>
#include <QInputDialog>
#include <QtAlgorithms>

dlCurves::dlCurves(QWidget *parent)
    : SimCenterWidget(parent)
{
    QGridLayout *theLayout = new QGridLayout();

    QGroupBox *theFragilitySelection = new QGroupBox("Fragility Selection:");
    QGridLayout *theFragilitySelectionLayout = new QGridLayout();
    theFragilitySelection->setLayout(theFragilitySelectionLayout);

    theFragilitySelectionLayout->addWidget(new QLabel("Source"),0,0);
    QComboBox *fragilitySource = new QComboBox();
    fragilitySource->addItem("Earthquake P58");
    fragilitySource->addItem("Earthquake HAZUS");
    fragilitySource->addItem("Hurricane HAZUS");    
    fragilitySource->addItem("User Provided");
    connect(fragilitySource, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(curvesChanged(QString)));

    theFragilitySelectionLayout->addWidget(fragilitySource,0,1);

    theTree = new QTreeWidget();
    QStringList headerLabels;
    headerLabels.push_back(tr("Index"));
    // headerLabels.push_back(tr("Description"));
    theTree->setColumnCount(headerLabels.count());
    theTree->setHeaderLabels(headerLabels);
    theFragilitySelectionLayout->addWidget(theTree, 1,0, 1, 2);
    
    theLayout->addWidget(theFragilitySelection, 0, 0);
    
    //  theLayout->addWidget(theFragilitySelection, 1, 0, 1, 3);

    // inputFile = new QLineEdit(this);

    connect(theTree,
            SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,
            SLOT(fragilityClicked(QTreeWidgetItem*,int)));

    QGroupBox *theFragilityBox = new QGroupBox("Fragility");
    QGridLayout *theFragilityLayout = new QGridLayout();
    theFragilityBox->setLayout(theFragilityLayout);
    theDisplay = new FragilityDisplay();
    theFragilityLayout->addWidget(theDisplay,0,0);
    

    theLayout->addWidget(theFragilityBox, 0,1);

    this->setLayout(theLayout);
    this->curvesChanged("Earthquake P58");
}

bool
dlCurves::outputToJSON(QJsonObject &jsonObject)
{
    QJsonArray fragArray;
    for (int i=0; i<theFragilities.size(); i++) {
        QJsonObject fragObject;
        theFragilities.at(i)->outputToJSON(fragObject);
        fragArray.append(fragObject);
    }
    jsonObject["fragilities"]=fragArray;
}

bool
dlCurves::inputFromJSON(QJsonObject &jsonObject) {

    // remove old
    qDeleteAll(theFragilities);
    theFragilities.clear();

    theTree->clear();
    QVector<int> breakPoints;
    if (jsonObject.contains("displayBreaks")) {
        QJsonValue theValue = jsonObject["displayBreaks"];
        QString string = theValue.toString();
        breakPointsString = string;
        QStringList listPoints = string.split(",");

        for ( const auto& i : listPoints) {
            breakPoints.append(i.toInt());
        }
    }
    if (jsonObject.contains("fragilities")) {
        QJsonValue theValue = jsonObject["fragilities"];
        QJsonArray theArray = theValue.toArray();
        int numFragilities = theArray.size();
        for (int i=0; i<numFragilities; i++) {
            Fragility *theFragility = new Fragility();
            QJsonObject theObj = theArray[i].toObject();
            theFragility->inputFromJSON(theObj);
            theFragilities.append(theFragility);
        }
        if (breakPoints.size() != 0)
            this->createInterface(breakPoints);
        else
            this->createInterface();
    } else {
        qDebug() << "dlCurves::inputFromJson() - no fragilities";
        return false;
    }

    return true;
}

QTreeWidgetItem *
dlCurves::addRoot(QString name, QString description)
{
    QTreeWidgetItem *theItem = new QTreeWidgetItem(theTree);
    theItem->setText(0, name);
    theItem->setText(1, description);
    theItem->setData(0, Qt::UserRole, QVariant(-1));
    theTree->addTopLevelItem(theItem);
    return theItem;
}

QTreeWidgetItem *
dlCurves::addChild(QString name, QString description, QTreeWidgetItem *parent, int count)
{
    QTreeWidgetItem *theItem = new QTreeWidgetItem(parent);
    theItem->setText(0, name);
    //    theItem->setText(1, description);

    theItem->setData(0, Qt::UserRole, QVariant(count));
    parent->addChild(theItem);
    return theItem;
}
QTreeWidgetItem *
dlCurves::addChild(QString name, QTreeWidgetItem *parent, int count)
{
    QTreeWidgetItem *theItem = new QTreeWidgetItem(parent);
    theItem->setText(0, name);

    theItem->setData(0, Qt::UserRole, QVariant(count));
    parent->addChild(theItem);
    return theItem;
}

void
dlCurves::createInterface(QVector<int> breakPoints) {

    if (theFragilities.size() == 0) {
        qDebug() << "dlCurves::createInterface - no Fragilities!";
        return;
    }

    // clear tree
    theTree->clear();

    Fragility *theFragility = theFragilities.at(0);
    QString index = theFragility->id;

    index.remove(QChar('.'), Qt::CaseInsensitive); // remove any dots .. not displayed

    // Two QVectors to hold current text and current treeItems
    QVector<QString> theCurrentTreeItemStrings;
    QVector<QTreeWidgetItem *> theCurrentTreeItems;

    // initialize the two vectors based on what is in the first fragility
    int currentIndexStart = 0;
    int numLevels = breakPoints.size();
    for (int i=0; i<numLevels; i++) {
        QString currentString = index.mid(currentIndexStart, breakPoints[i]);
        theCurrentTreeItemStrings.append(currentString);
        currentIndexStart = currentIndexStart + breakPoints[i];
        if (i == 0) {
            theCurrentTreeItems.append(addRoot(currentString,  QString("")));
        } else {
            theCurrentTreeItems.append(addChild(currentString,theCurrentTreeItems[i-1]));
        }
    }

    //for (int i=0; i<numLevels; i++) qDebug() << theCurrentTreeItemStrings[i];

    // use this index for fast search when displaying
    int fragIndex = 0;

    // add the first fragility 0 to tree
    addChild(theFragility->id, theFragility->description, theCurrentTreeItems[numLevels-1], fragIndex);

    // now add the rest of them
    for (int i=1; i<theFragilities.size(); i++) {

        Fragility *theFragility = theFragilities.at(i);

        QString index = theFragility->id;
        index.remove(QChar('.'), Qt::CaseInsensitive); // remove any dots .. not displayed

        bool swapCurrent = false;
        currentIndexStart = 0;

        for (int i=0; i<numLevels; i++) {
            QString currentString = index.mid(currentIndexStart, breakPoints[i]);
            currentIndexStart = currentIndexStart + breakPoints[i];
            if (currentString != theCurrentTreeItemStrings[i])
                swapCurrent = true;

            if (swapCurrent == true) {
                // qDebug() << "SWAP : " << i << " " << theCurrentTreeItemStrings[i] << " " << currentString << index;
                theCurrentTreeItemStrings[i] = currentString;

                if (i == 0) {
                    theCurrentTreeItems[i] = addRoot(currentString,  QString(""));
                } else {
                    theCurrentTreeItems[i] = addChild(currentString,theCurrentTreeItems[i-1]);
                }
            }
        }

        // incr fragIndex & add
        fragIndex++;
        addChild(theFragility->id, theFragility->description, theCurrentTreeItems[numLevels-1], fragIndex);
    }
}

// method to find num characters between . characters
void
dlCurves::findBreakPoints(const QString &str, QVector<int> &breakPoints) {
  int n = 0;
  int loc = str.indexOf('.', n);
  while (loc != -1) {
    breakPoints.append(loc-n);
    n = loc+1;
    loc = str.indexOf(".", n);
  }
}

void
dlCurves::createInterface(void) {

    if (theFragilities.size() == 0) {
        qDebug() << "dlCurves::createInterface - no Fragilities!";
        return;
    }

    // clear tree
    theTree->clear();

    Fragility *theFragility = theFragilities.at(0);
    QString index = theFragility->id;

    // find . breakpoints
    QVector<int>breakPoints;
    findBreakPoints(index, breakPoints);
    if (breakPoints.size() == 0) { // add one if none
        breakPoints.append(index.size());
    }

    index.remove(QChar('.'), Qt::CaseInsensitive); // remove any dots .. not displayed
    
    // Two QVectors to hold current text and current treeItems
    QVector<QString> theCurrentTreeItemStrings;
    QVector<QTreeWidgetItem *> theCurrentTreeItems;

    // initialize the two vectors based on what is in the first fragility
    int currentIndexStart = 0;
    int numLevels = breakPoints.size();
    for (int i=0; i<numLevels; i++) {
        QString currentString = index.mid(currentIndexStart, breakPoints[i]);
        theCurrentTreeItemStrings.append(currentString);
        currentIndexStart = currentIndexStart + breakPoints[i];
        if (i == 0) {
            theCurrentTreeItems.append(addRoot(currentString,  QString("")));
        } else {
            theCurrentTreeItems.append(addChild(currentString,theCurrentTreeItems[i-1]));
        }
    }

    //for (int i=0; i<numLevels; i++) qDebug() << theCurrentTreeItemStrings[i];

    // use this index for fast search when displaying
    int fragIndex = 0;

    // add the first fragility 0 to tree
    addChild(theFragility->id, theFragility->description, theCurrentTreeItems[numLevels-1], fragIndex);

    // now add the rest of them
    for (int i=1; i<theFragilities.size(); i++) {

        Fragility *theFragility = theFragilities.at(i);
        QString index = theFragility->id;

        QVector<int>breakPointsCurrent;
        findBreakPoints(index, breakPointsCurrent);

        if (breakPointsCurrent.size() == 0) {
            breakPointsCurrent.append(index.size());
        }

        if (breakPointsCurrent.size() != breakPoints.size()) {
            qDebug() << "WARNING: diffent number of breaks index: " << index;
        }

        numLevels = breakPointsCurrent.size();

        index.remove(QChar('.'), Qt::CaseInsensitive); // remove any dots .. not displayed

        bool swapCurrent = false;
        currentIndexStart = 0;

        for (int i=0; i<numLevels; i++) {
            QString currentString = index.mid(currentIndexStart, breakPointsCurrent[i]);
            currentIndexStart = currentIndexStart + breakPointsCurrent[i];
            if (numLevels > theCurrentTreeItemStrings.size()) {
                theCurrentTreeItemStrings.append(currentString);
                swapCurrent = true;
            } else if (currentString != theCurrentTreeItemStrings[i])
                swapCurrent = true;

            if (swapCurrent == true) {
                // qDebug() << "SWAP : " << i << " " << theCurrentTreeItemStrings[i] << " " << currentString << index;
                theCurrentTreeItemStrings[i] = currentString;

                if (i == 0) {
                    theCurrentTreeItems[i] = addRoot(currentString,  QString(""));
                } else {
                    if (theCurrentTreeItems.size() < i+1)
                        theCurrentTreeItems.append(addChild(currentString,theCurrentTreeItems[i-1]));
                    else
                        theCurrentTreeItems[i] = addChild(currentString,theCurrentTreeItems[i-1]);
                }
            }
        }

        // incr fragIndex & add
        fragIndex++;
        addChild(theFragility->id, theFragility->description, theCurrentTreeItems[numLevels-1], fragIndex);
    }
}




void
dlCurves::parseSimCenter(QString fileName) {


   bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Break Points for indices:"), QLineEdit::Normal,
                                         "1,2,2", &ok);

    if (ok && !text.isEmpty()) {
        qDebug() << "breakPoints: " << text;
    } else {
        qDebug() << "Using built in breakPoints";
    }

    breakPointsString = text;
    QVector<int> breakPoints;

    if (text.contains(',')) {
        QStringList listPoints = text.split(",");

        for ( const auto& i : listPoints) {
            breakPoints.append(i.toInt());
        }
    }
    theTree->clear();
    qDeleteAll(theFragilities);
    theFragilities.clear();

    //
    // open csv & json files (json file name must be similar to csv filename)
    //

    QString theCSV_Filename = fileName;
    QFile csvFile(theCSV_Filename);
    if (!csvFile.open(QIODevice::ReadOnly)) {
        qDebug() << "could not open csv File: " << theCSV_Filename;
        return;
    }
    
    QString theJSON_Filename = theCSV_Filename.replace(QString(".csv"), QString(".json"));
    QFile jsonFile(theJSON_Filename);
    bool haveJSON = true;
    QJsonObject jsonObj;
    if (!jsonFile.open(QFile::ReadOnly | QFile::Text)) {
      haveJSON = false;    
    } else {

      QString val=jsonFile.readAll();
      QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
      
      jsonObj = doc.object();
      if (jsonObj.isEmpty()) {
        qDebug() << QString("Invalid json file: ") << theJSON_Filename << QString(" . Check the format");
        return;
      }
    }

    //
    // first line containing headers
    //

    QByteArray line = csvFile.readLine();
    QList<QByteArray> headerList = line.split(',');

    //
    // read first line, start the tree & create first fragility
    //

    line = csvFile.readLine();
    QList<QByteArray> wordList = line.split(',');
    QByteArray indices = wordList.first();
    QString index = QString(indices);

    // create new fragility & add
    Fragility *newFragility = new Fragility();

    QJsonObject theFragilityDescription;
    if (jsonObj.contains(index))
        theFragilityDescription = jsonObj[index].toObject();

    newFragility->read(wordList, theFragilityDescription);
    theFragilities.append(newFragility);
    if (index != newFragility->id) {
        qDebug() << "Index and ID do not match! " << index << " " << newFragility->id;
        return;
    }


    // parse each line of file, create fragility and update tree as before
    QWidgetItem *current;
    while (!csvFile.atEnd()) {
        QByteArray line = csvFile.readLine();

        wordList = line.split(',');
        QByteArray indices = wordList.first();
        QString index = QString(indices);

        Fragility *newFragility = new Fragility();
        if (jsonObj.contains(index))
            theFragilityDescription = jsonObj[index].toObject();
        newFragility->read(wordList, theFragilityDescription);
        theFragilities.append(newFragility);
    }

    if (breakPoints.size() != 0)
        this->createInterface(breakPoints);
    else
         this->createInterface();
}


void
dlCurves::fragilityClicked(QTreeWidgetItem *current, int column) {

    QVariant intV = current->QTreeWidgetItem::data(0, Qt::UserRole);
    int index = intV.toInt();
    if (index != -1) {
        // inputFile->setText(QString::number(index));
        Fragility *theFragility=theFragilities.at(index);
        if (theFragility != NULL)
            theDisplay->display(theFragility);
    }
}


void
dlCurves::curvesChanged(QString source) {

    if (source == "Earthquake P58") {

        QFile file(":/standardFragilities/femaP58.json");
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "dlCurves - could not open FEMA_P58";
            return;
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

        bool result = this->inputFromJSON(jsonObj);
    }  else if (source == "Earthquake HAZUS") {
      
        QFile file(":/standardFragilities/hazusEQ.json");
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "dlCurves - could not open HAZUS_EQ";
            return;
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

        bool result = this->inputFromJSON(jsonObj);	


    }  else if (source == "Hurricane HAZUS") {
      
        QFile file(":/standardFragilities/hazusHW.json");
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "dlCurves - could not open HAZUS_HU";
            return;
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

        bool result = this->inputFromJSON(jsonObj);	
	

    } else if (source == "User Provided") {

        QString selectedFile = QFileDialog::getOpenFileName(this,
                                                            tr("Postprocess Script"),
                                                            "C://",
                                                            "All files (*.csv)");
        if(!selectedFile.isEmpty()) {
            //inputFile->setText(selectedFile);
            this->parseSimCenter(selectedFile);
        }
    }
}




