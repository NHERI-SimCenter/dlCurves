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

#include "Fragility.h"
#include "LimitState.h"
#include "DamageState.h"

#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QtAlgorithms>


Fragility::Fragility()
{

}

Fragility::~Fragility()
{
    // no memory allocated
    qDeleteAll(limitStates);
    limitStates.clear();
}

bool
Fragility::outputToJSON(QJsonObject &jsnObj) {
    jsnObj["id"]=id;
    jsnObj["demandType"]=demandType;
    jsnObj["demandUnit"]=demandUnit;
    jsnObj["demandOffset"]=demandOffset;
    jsnObj["demandDirectional"]=demandDirectional;
    jsnObj["description"]=description;
    jsnObj["comments"]=comments;
    jsnObj["incomplete"]=incomplete;

    QJsonArray lsArray;
    for (int i=0; i<limitStates.size(); i++) {
        QJsonObject lsObject;
        limitStates.at(i)->outputToJSON(lsObject);
        lsArray.append(lsObject);
    }
    jsnObj["limitStates"]=lsArray;

    return true;
}

bool
Fragility::inputFromJSON(QJsonObject &jsnObj) {

    bool result = true;

    qDebug() << "Fragility::inputFromJSON";
    //
    // parse all objects, all same .. check there & do convesion if there .. otherwise return false
    //

    if (jsnObj.contains("id")) {
        QJsonValue theValue = jsnObj["id"];
        id = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no id";
        return false;
    }

    if (jsnObj.contains("demandType")) {
        QJsonValue theValue = jsnObj["demandType"];
        demandType = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no demandType";
        return false;
    }

    if (jsnObj.contains("demandUnit")) {
        QJsonValue theValue = jsnObj["demandUnit"];
        demandUnit = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no demandUnit";
        return false;
    }

    if (jsnObj.contains("demandOffset")) {
        QJsonValue theValue = jsnObj["demandOffset"];
        demandOffset = theValue.toInt();
    } else {
        qDebug() << "LimitState::inputFromJson() - no demandOffset";
        return false;
    }

    if (jsnObj.contains("demandDirectional")) {
        QJsonValue theValue = jsnObj["demandDirectional"];
        demandDirectional = theValue.toInt();
    } else {
        qDebug() << "LimitState::inputFromJson() - no demandDirectional";
        return false;
    }

    if (jsnObj.contains("description")) {
        QJsonValue theValue = jsnObj["description"];
        description = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no description";
        return false;
    }

    if (jsnObj.contains("comments")) {
        QJsonValue theValue = jsnObj["comments"];
        comments = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no comments";
        return false;
    }

    if (jsnObj.contains("incomplete")) {
        QJsonValue theValue = jsnObj["incomplete"];
        incomplete = theValue.toInt();
    } else {
        qDebug() << "LimitState::inputFromJson() - no incomplete";
        return false;
    }

    if (jsnObj.contains("limitStates")) {
        QJsonValue theValue = jsnObj["limitStates"];
        QJsonArray lsArray = theValue.toArray();
        int numDS = lsArray.size();
        for (int i=0; i<numDS; i++) {
            LimitState *theLS = new LimitState();
            QJsonObject theLS_Obj = lsArray[i].toObject();
            theLS->inputFromJSON(theLS_Obj);
            limitStates.append(theLS);
        }
    } else {
        qDebug() << "Fragility::inputFromJson() - no limitStates";
        return false;
    }
    return result;
}


int
Fragility::read(QList<QByteArray> inputData, QJsonObject jsonObject)
{

    QStringList families;
    double theta0[4];
    double theta1[4];
    double *weights[4];
    int weightLengths[4];
    
    for (int i=0; i<4; i++) {
        theta0[i]=NAN;
        theta1[i]=NAN;
        weights[i] = NULL;
        weightLengths[i] = 0;
    }

    id = QString(inputData[0]);
    incomplete = inputData[1].toInt();
    demandType = QString(inputData[2]);
    demandUnit = QString(inputData[3]);
    demandOffset = inputData[4].toInt();
    demandDirectional = inputData[5].toInt();


    families.append(QString(inputData[6]));
    if (!inputData[7].isEmpty())
        theta0[0] = inputData[7].toDouble();
    if (!inputData[8].isEmpty())
        theta1[0] = inputData[8].toDouble();
    if (!inputData[9].isEmpty()) {
        QList<QByteArray> weightsList = inputData[9].split('|');
        double *weightL=new double[weightsList.size()];
        weights[0]= weightL;
        for (int j=0; j<weightsList.size(); j++) {
            weightL[j]=weightsList.at(j).toDouble();
        }
    }

    if (!inputData[10].isEmpty()) {
        families.append(QString(inputData[10]));
        if (!inputData[11].isEmpty())
            theta0[1] = inputData[11].toDouble();
        if (!inputData[12].isEmpty())
            theta1[1] = inputData[12].toDouble();
        if (!inputData[13].isEmpty()) {
            QList<QByteArray> weightsList = inputData[13].split('|');
            double *weightL=new double[weightsList.size()];
            weights[1]= weightL;
            for (int j=0; j<weightsList.size(); j++) {
                weightL[j]=weightsList.at(j).toDouble();
            }
        }
    }

    if (!inputData[14].isEmpty()) {
        families.append(QString(inputData[14]));
        if (!inputData[15].isEmpty())
            theta0[2] = inputData[15].toDouble();
        if (!inputData[16].isEmpty())
            theta1[2] = inputData[16].toDouble();
        if (!inputData[17].isEmpty()) {
            QList<QByteArray> weightsList = inputData[17].split('|');
            double *weightL=new double[weightsList.size()];
            weights[2]= weightL;
            for (int j=0; j<weightsList.size(); j++) {
                weightL[j]=weightsList.at(j).toDouble();
            }
        }
    }


    if (!inputData[18].isEmpty()) {
        families.append(QString(inputData[18]));
        if (!inputData[19].isEmpty())
            theta0[3] = inputData[19].toDouble();
        if (!inputData[20].isEmpty())
            theta1[3] = inputData[20].toDouble();
        if (!inputData[21].isEmpty()) {
            QList<QByteArray> weightsList = inputData[21].split('|');
            double *weightL=new double[weightsList.size()];
            weights[3]= weightL;
            for (int j=0; j<weightsList.size(); j++) {
                weightL[j]=weightsList.at(j).toDouble();
            }
        }
    }

    int res = this->parseDescription(jsonObject, families, theta0, theta1, weights, weightLengths);

    for (int i=0; i<4; i++)
        if (weights[i] != 0)
            delete [] weights[i];
    return res;

}
int
Fragility::read(QList<QByteArray>header, QList<QByteArray>data, QJsonObject obj)
{
    qDebug() << "Fragility::read(QList, QList) - not implemented";
    return -1;
}

int
Fragility::parseDescription(QJsonObject &jsonObj,
                            QStringList &families,
                            double *theta0,
                            double *theta1,
                            double **weights,
                            int *weightLengths) {

    if (jsonObj.contains("Description")) {
        description = jsonObj["Description"].toString();
    } else {
        qDebug() << "No description in json object with key Description" << jsonObj;
        return -1;
    }

    if (jsonObj.contains("Comments")) {
        comments = jsonObj["Comments"].toString();
    } else {
        qDebug() << "No description in json object with key Comments";
        return -1;
    }


    if (jsonObj.contains("LimitStates")) {
        QJsonObject limitStatesObj = jsonObj["LimitStates"].toObject();
        int numLS = 0;
        // loop over all limit states in object; create new LimitState and append to fragility
        QStringList keysLS = limitStatesObj.keys();
        foreach (const QString &keyLS, keysLS) {

            LimitState *newLS = new LimitState;
            QJsonObject limitStateObj = limitStatesObj[keyLS].toObject();
            newLS->name=keyLS;
            newLS->theta0 = theta0[numLS];
            newLS->theta1 = theta1[numLS];
            newLS->curveDistribution = families.at(numLS);

            QStringList keysDS = limitStateObj.keys();

            // loop over all damage states, create new damage state and append to limit state
            int numDS = 0;
            foreach (const QString &keyDS, keysDS) {
                DamageState *newDS = new DamageState;
                QJsonObject damageState = limitStateObj[keyDS].toObject();
                newDS->name = keyDS;
                newDS->limitState = newLS;

                if (damageState.contains("Description"))
                    newDS->description = damageState["Description"].toString();
                if (damageState.contains("RepairAction"))
                    newDS->repairAction = damageState["RepairAction"].toString();

                if (weights[numLS] != NULL)
                    newDS->weight = weights[numLS][numDS];
                else
                    newDS->weight = 1.0;
                newLS->damageStates.append(newDS);
                numDS++;
            }

            // append new limit state
            limitStates.append(newLS);
            numLS++;
        }
    } else {
        qDebug() << "No description in json object with key: LimitStates ";
        return -1;
    }
}



