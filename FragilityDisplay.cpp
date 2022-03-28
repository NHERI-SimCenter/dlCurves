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

#include "FragilityDisplay.h"
#include "Fragility.h"
#include "LimitState.h"
#include "DamageState.h"
#include "SimCenterGraphPlot.h"

#include <QGridLayout>
#include <QHBoxLayout>

#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QJsonObject>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGroupBox>
#include <QFontMetrics>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QHBoxLayout>

FragilityDisplay::FragilityDisplay(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *theLayout = new QGridLayout();

    QHBoxLayout *topLayout = new QHBoxLayout();
    index = new QLineEdit();
    blockSize = new QLineEdit();
    complete = new QLineEdit();
    complete->setStyleSheet("color: red;");

    topLayout->addWidget(index);
    topLayout->addStretch();
    topLayout->addWidget(blockSize);
    topLayout->addStretch();
    topLayout->addWidget(complete);
    theLayout->addLayout(topLayout,0,0,1,3);

    //theLayout->addWidget(index,0,0);
    //theLayout->addWidget(blockSize,0,1);
    //theLayout->addWidget(complete,0,2);
    
    description = new QTextEdit();

    theLayout->addWidget(description, 1,0, 1, 3);

    QFontMetrics m (description->font()) ;
    int rowHeight = m.lineSpacing() ;

    description->setFixedHeight(10*rowHeight);
    //  comments->setFixedHeight(5*rowHeight);

    QGroupBox *theLD_Selection = new QGroupBox("Limit States");
    QGridLayout *theLD_SelectionLayout = new QGridLayout();
    theLD_Selection->setLayout(theLD_SelectionLayout);

    theLimitStates = new QTableWidget(1,4);
    theLimitStates->setHorizontalHeaderLabels(QString("LS;Family;Theta_0;Theta_1").split(";"));
    theLimitStates->verticalHeader()->setVisible(false);
    theLimitStates->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    theLimitStates->setSelectionBehavior(QAbstractItemView::SelectRows);
    //theLimitStates->verticalHeader()->setStretchLastSection(true);
    theDamageStates = new QTableWidget(1,3);
    theDamageStates->setHorizontalHeaderLabels(QString("DS;Weight;Description").split(";"));
    theDamageStates->verticalHeader()->setVisible(false);
    theDamageStates->horizontalHeader()->setStretchLastSection(true);
    //theDamageStates->setSelectionBehavior(QAbstractItemView::SelectRows);

    theLD_SelectionLayout->addWidget(theLimitStates,0,0);
    thePlot = new SimCenterGraphPlot("x_axis","y-axis");
    theLD_SelectionLayout->addWidget(thePlot, 0,1);
    theLD_SelectionLayout->setColumnStretch(0,1);
    theLD_SelectionLayout->setColumnStretch(1,3);

    theLayout->addWidget(theLD_Selection, 2,0,1,3);

    //  lsLayout->addWidget(theLimitStates, 0, 0);

    damageBox = new QGroupBox("Damage States:");
    QHBoxLayout *damageStateLayout = new QHBoxLayout();
    damageBox->setLayout(damageStateLayout);


    damageStateLayout->addWidget(theDamageStates);
    theLD_SelectionLayout->addWidget(damageBox, 1,0,1,2);

    theLayout->setColumnStretch(1,1);
    theLayout->setColumnStretch(2,1);

    this->setLayout(theLayout);

    connect(theLimitStates, SIGNAL(cellPressed(int, int)), this, SLOT(lsTableRowClicked(int, int)));
}


int
FragilityDisplay::display(Fragility *theFragility) {

    index->setText(theFragility->id);
    QString blockSizeText = QString("Block Size: " ) + theFragility->blockSize;
    if (theFragility->roundToInt == true)
      blockSizeText += QString(" (Integer Quantity)");

    if (theFragility->incomplete == false) {
      complete->setText("");
      // complete->setStyleSheet("color: green;");
    } else {
      complete->setText("Incomplete!");
      // complete->setStyleSheet("color: red;");
    }
    
    blockSize->setText(blockSizeText);
    description->setText(theFragility->description + "\n\n" + theFragility->comments);

    theLimitStates->clear();
    theLimitStates->setRowCount(0);

    theDamageStates->clear();
    theDamageStates->setRowCount(0);

    thePlot->clear() ;
    lsList.clear();

    static int colors[4*3];
    // https://colorbrewer2.org/#type=qualitative&scheme=Set1&n=5
    colors[0]=55;   colors[1]=126;    colors[2]=184;     // black
    colors[3]=77;  colors[4]=175;   colors[5]=74;        // blue
    colors[6]=152;   colors[7]=78;  colors[8]=163;       // green
    colors[9]=225;  colors[10]=127;  colors[11]=0;       // purplish

    int numLS = 0;
    QString xAxisLabel = theFragility->demandType + QString(" (") + theFragility->demandUnit + QString(") [offset: ")
            + QString::number(theFragility->demandOffset) + QString("]");

    thePlot->setLabels(xAxisLabel, "yAxis");

    foreach (LimitState *lsItem, theFragility->limitStates ) {

        theLimitStates->insertRow(numLS);
        lsList.append(lsItem);

        QTableWidgetItem *name = new QTableWidgetItem;
        name->setText(lsItem->name);
        name->setTextAlignment( Qt::AlignCenter );
        theLimitStates->setItem(numLS, 0, name);

        QTableWidgetItem *family = new QTableWidgetItem;
        family->setText(lsItem->curveDistribution);
        family->setTextAlignment( Qt::AlignCenter );
        theLimitStates->setItem(numLS, 1, family);

        QTableWidgetItem *th0 = new QTableWidgetItem;
        th0->setText(QString::number(lsItem->theta0));
        th0->setTextAlignment( Qt::AlignCenter );
        theLimitStates->setItem(numLS, 2, th0);

        QTableWidgetItem *th1 = new QTableWidgetItem;
        th1->setText(QString::number(lsItem->theta1));
        th1->setTextAlignment( Qt::AlignCenter );
        theLimitStates->setItem(numLS, 3, th1);

        // plot limitState
        double la=0, ze=0, me=0, st=0;
        la=lsItem->theta0;
        ze=lsItem->theta1;
        if (la != NAN && ze != NAN) {

            st = sqrt(exp(2*log(la)+pow(ze,2))*(exp(pow(ze,2))-1));

            if (st > 0.0) {
                double min = me - 5*st; // defined in x>0
                if (min < 0)
                    min = 0;
                double max = me + 5*st;
                QVector<double> x(500);
                QVector<double> y(500);
                y[0]=0.;
                double sum = 0.;
                for (int i=0; i<500; i++) {
                    double xi = min + i*(max-min)/499;
                    x[i] = xi;
                    if (i != 0) y[i] = 1.0/(xi*ze*sqrt(2*3.141592))*exp( - pow((log(xi)-log(la))/ze,2)/2);
                    // y[i] = sum;
                }
                /*
               for (int i=0; i<500; i++)
                  y[i] = y[i]/sum;
               */
                thePlot->addLine(x,y,2, colors[3*numLS], colors[3*numLS+1], colors[3*numLS+2], lsItem->name);
                qDebug() << colors[3*numLS] << " " << colors[3*numLS+1] << " " << colors[3*numLS+2];
            }
        }

        numLS++;
    }
    //theLimitStates->setCurrentCell(0,0);
    //theLimitStates->cellPressed(0,0);
    theLimitStates->selectRow(0);
}


void
FragilityDisplay::lsTableRowClicked(int row, int column)
{

    theDamageStates->clear();
    theDamageStates->setRowCount(0);

    int indexLS = row;
    LimitState *theLS = lsList.at(indexLS);
    int numDS = 0;

    damageBox->setTitle("Damage States in " + theLS->name);

    foreach( DamageState *dsItem, theLS->damageStates) {
        theDamageStates->insertRow(numDS);

        QTableWidgetItem *name = new QTableWidgetItem;
        name->setText(dsItem->name);
        name->setTextAlignment( Qt::AlignCenter );
        theDamageStates->setItem(numDS, 0, name);

        QTableWidgetItem *weight = new QTableWidgetItem;
        weight->setText(QString::number(dsItem->weight));
        weight->setTextAlignment( Qt::AlignCenter );
        theDamageStates->setItem(numDS, 1, weight);

        QTableWidgetItem *descr = new QTableWidgetItem;
        descr->setText(dsItem->description);
        theDamageStates->setItem(numDS, 2, descr);
        numDS++;
    }
}
