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

FragilityDisplay::FragilityDisplay(QWidget *parent)
  : QWidget(parent)
{
  QGridLayout *theLayout = new QGridLayout();
  theLayout->addWidget(new QLabel("#"),0,0);

  index = new QLineEdit();
  theLayout->addWidget(index, 0,1);

  theLayout->addWidget(new QLabel("Description"),1,0);
  description = new QTextEdit();
  theLayout->addWidget(description, 1,1);
  
  theLayout->addWidget(new QLabel("Comments"),2,0);
  comments = new QTextEdit();  
  theLayout->addWidget(comments, 2,1, 1, 1);

  QFontMetrics m (comments->font()) ;
  int rowHeight = m.lineSpacing() ;

  description->setFixedHeight(5*rowHeight);
  comments->setFixedHeight(5*rowHeight);

  QGroupBox *theLD_Selection = new QGroupBox("Limit State/Damage State Selection");
  QGridLayout *theLD_SelectionLayout = new QGridLayout();
  theLD_Selection->setLayout(theLD_SelectionLayout);
  
  limitStates = new QTreeWidget();
  limitStates->setHeaderHidden(true);

  theLD_SelectionLayout->addWidget(limitStates,0,0);
  theLayout->addWidget(theLD_Selection, 3,0,1,2);


  thePlot = new SimCenterGraphPlot("x_axis","y-axis");
  theLayout->addWidget(thePlot, 0,2,4,1);


  QGroupBox *lsBox = new QGroupBox("Limit State");
  QGridLayout *lsLayout = new QGridLayout();
  lsBox->setLayout(lsLayout);
  lsLayout->addWidget(new QLabel("Curve Type"), 0, 0);
  lsLayout->addWidget(new QLabel("theta0"), 0, 2);
  lsLayout->addWidget(new QLabel("theta1"), 0, 4);
  lsCurve = new QLineEdit();
  theta0 = new QLineEdit();
  theta1 = new QLineEdit();
  lsLayout->addWidget(lsCurve, 0, 1);
  lsLayout->addWidget(theta0, 0, 3);
  lsLayout->addWidget(theta1, 0, 5);


  QGroupBox *damageBox = new QGroupBox("Damage State");
  QGridLayout *damageStateLayout = new QGridLayout();
  damageStateLayout->addWidget(new QLabel("weight"), 0, 0);
  dsWeight = new QLineEdit();
  damageStateLayout->addWidget(dsWeight, 0,1);

  damageBox->setLayout(damageStateLayout);
  damageStateRepair = new QTextEdit();
  damageStateDescription = new QTextEdit();  
  damageStateLayout->addWidget(new QLabel("Description"), 1,0);
  damageStateLayout->addWidget(damageStateDescription, 1,1, 1, 2);
  damageStateRepair->setFixedHeight(3*rowHeight);
  damageStateDescription->setFixedHeight(3*rowHeight);    
  damageStateLayout->addWidget(new QLabel("Repair Action"), 2,0);
  damageStateLayout->addWidget(damageStateRepair, 2,1, 1, 2);
  damageStateLayout->setColumnStretch(2,1);
  lsLayout->addWidget(damageBox, 1, 0, 1, 7);
  lsLayout->setColumnStretch(6,1);

  theLayout->addWidget(lsBox,4,0,1,4);

  theLayout->setColumnStretch(1,1);
  theLayout->setColumnStretch(2,1);  
  
  this->setLayout(theLayout);

  connect(limitStates,
	  SIGNAL(itemClicked(QTreeWidgetItem*,int)),
	  this,
	  SLOT(dsClicked(QTreeWidgetItem*,int)));    
}


int
FragilityDisplay::display(Fragility *theFragility) {
  
    index->setText(theFragility->id);
    description->setText(theFragility->description);
    comments->setText(theFragility->comments);
    limitStates->clear();

  dsList.clear();
  lsList.clear();

  static int colors[4*3];
  // https://colorbrewer2.org/#type=qualitative&scheme=Set1&n=5
  colors[0]=55;   colors[1]=126;    colors[2]=184;     // black
  colors[3]=77;  colors[4]=175;   colors[5]=74;   // blue
  colors[6]=152;   colors[7]=78;  colors[8]=163;    // green
  colors[9]=225;  colors[10]=127;  colors[11]=0;  // purplish
  
  int numLS = 0;
  int numDS = 0;
  thePlot->clear();

  foreach (LimitState *lsItem, theFragility->limitStates ) { 

      // plot limitState
      double la=0, ze=0, me=0, st=0;
      la=lsItem->theta0;
      ze=lsItem->theta1;
      me = exp(log(la)+(pow(ze,2)/2));
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
          thePlot->addLine(x,y,2, colors[3*numDS], colors[3*numDS+1], colors[3*numDS+2], lsItem->name);
          qDebug() << colors[3*numDS] << " " << colors[3*numDS+1] << " " << colors[3*numDS+2];
      }


      QTreeWidgetItem *theRoot= new QTreeWidgetItem(limitStates);
      theRoot->setText(0, lsItem->name);
      theRoot->setData(0, Qt::UserRole, QVariant(-1*(1+numLS)));
      limitStates->addTopLevelItem(theRoot);
      lsList.append(lsItem);

      foreach( DamageState *dsItem, lsItem->damageStates) {
          QTreeWidgetItem *theChild= new QTreeWidgetItem(theRoot);
          theChild->setText(0, dsItem->name);
          theChild->setData(0, Qt::UserRole, QVariant(numDS));
          theRoot->addChild(theChild);
          dsList.append(dsItem);
          numDS++;
      }
      numLS++;
  }
  damageStateRepair->setText("");
  damageStateDescription->setText("");
  dsWeight->setText("");
  lsCurve->setText("");
  theta0->setText("");
  theta1->setText("");
}

void
FragilityDisplay::dsClicked(QTreeWidgetItem *current, int column) {

  QVariant intV = current->QTreeWidgetItem::data(0, Qt::UserRole);
  int index = intV.toInt();
  if (index >= 0)  {
      DamageState *theDS = dsList.at(index);
      if (theDS != 0) {
          damageStateRepair->setText(theDS->repairAction);
          damageStateDescription->setText(theDS->description);
          dsWeight->setText(QString::number(theDS->weight));
	  LimitState *theLS = theDS->limitState;
	  lsCurve->setText(theLS->curveDistribution);
	  theta0->setText(QString::number(theLS->theta0));
	  theta1->setText(QString::number(theLS->theta1));	  
      }
  } else {
      int indexLS = -1*(index + 1);
      LimitState *theLS = lsList.at(indexLS);
      lsCurve->setText(theLS->curveDistribution);
      theta0->setText(QString::number(theLS->theta0));
      theta1->setText(QString::number(theLS->theta1));
      damageStateRepair->setText("");
      damageStateDescription->setText("");
      dsWeight->setText("");
  }
}
