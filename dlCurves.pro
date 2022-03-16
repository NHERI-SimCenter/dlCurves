QT += widgets printsupport network 

TARGET = dlViewer
TEMPLATE = app

include(../SimCenterCommon/Common/Common.pri)
INCLUDEPATH += ../SimCenterCommon/Common
INCLUDEPATH += ../SimCenterCommon/Common/Utils
COMMON=../SimCenterCommon/Common

SOURCES       = dlCurves.cpp \
                DamageState.cpp \
                LimitState.cpp \
                Fragility.cpp \
                FragilityDisplay.cpp \
                SimCenterGraphPlot.cpp \
                qcustomplot.cpp \
                MainWindowSimCenterPlainApp.cpp \
                $$COMMON/GoogleAnalytics.cpp \
                $$COMMON/SimCenterWidget.cpp \
                   $$COMMON/FooterWidget.cpp \
                $$COMMON/HeaderWidget.cpp \                                      
                   $$COMMON/Utils/PythonProgressDialog.cpp \
                $$COMMON/Utils/dialogabout.cpp \                                      
                main.cpp
                
HEADERS       = dlCurves.h \
                DamageState.h \
                LimitState.h \                
                FragilityDisplay.h \
                SimCenterGraphPlot.h \
                qcustomplot.h \
                MainWindowSimCenterPlainApp.h \                
                $$COMMON/GoogleAnalytics.h \                
                $$COMMON/SimCenterWidget.h \
                   $$COMMON/FooterWidget.h \
                $$COMMON/HeaderWidget.h \                                                         
                   $$COMMON/Utils/PythonProgressDialog.h \
                $$COMMON/Utils/dialogabout.h \                   
                Fragility.h
  
FORMS += \
    $$COMMON/Utils/dialogabout.ui

RESOURCES += fragilities.qrc
    
    
     
