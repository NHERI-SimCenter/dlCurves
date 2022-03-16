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

#include "DamageState.h"

#include <QString>
#include <QJsonObject>

#include <LimitState.h>

DamageState::DamageState(LimitState *theLS) {
  limitState = theLS;
}

DamageState::~DamageState() {
    // no memory allocated
}

bool
DamageState::outputToJSON(QJsonObject &jsnObj) {
    jsnObj["name"]=name;
    jsnObj["description"]=description;
    jsnObj["repairAction"]=repairAction;
    jsnObj["weight"]=weight;
    return true;
}

bool
DamageState::inputFromJSON(QJsonObject &jsnObj) {

    bool result = true;

    //
    // parse all objects, all same .. check there & do convesion if there .. otherwise return false
    //

    if (jsnObj.contains("name")) {
        QJsonValue theValue = jsnObj["name"];
        name = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no name";
        return false;
    }

    if (jsnObj.contains("description")) {
        QJsonValue theValue = jsnObj["description"];
        description = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no description";
        return false;
    }

    if (jsnObj.contains("repairAction")) {
        QJsonValue theValue = jsnObj["repairAction"];
        repairAction = theValue.toString();
    } else {
        qDebug() << "LimitState::inputFromJson() - no repairAction";
        return false;
    }

    if (jsnObj.contains("weight")) {
        QJsonValue theValue = jsnObj["weight"];
        weight = theValue.toDouble();
    } else {
        qDebug() << "LimitState::inputFromJson() - no weight";
        return false;
    }

    return result;
}

