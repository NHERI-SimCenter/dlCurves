#include "SimCenterGraphPlot.h"
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>

#include <qcustomplot.h>


SimCenterGraphPlot::SimCenterGraphPlot(
        QString xLabel,
        QString yLabel,
        QWidget *parent)
    : QWidget(parent), numGraphs(0), atStart(true)
{
    // create a main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();

    //
    // graphic window
    //

    thePlot=new QCustomPlot();
    thePlot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    thePlot->setInteractions(QCP::iSelectPlottables);

    QRect rec = QGuiApplication::screens()[0]->geometry();

    int height = 0.2*rec.height();
    int width = 0.2*rec.width();

    thePlot->setMinimumWidth(width);
    thePlot->setMinimumHeight(height);

    mainLayout->addWidget(thePlot);

    thePlot->xAxis->setLabel(xLabel);
    thePlot->yAxis->setLabel(yLabel);
    // thePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);

    this->setLayout(mainLayout);
}

SimCenterGraphPlot::SimCenterGraphPlot(
        QString xLabel,
        QString yLabel,
        int minWidth,
        int minHeight,
        QWidget *parent)
    : QWidget(parent), numGraphs(0), atStart(true)
{
    // create a main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();

    //
    // graphic window
    //

    thePlot=new QCustomPlot();
    thePlot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    thePlot->setInteractions(QCP::iSelectPlottables);


    thePlot->setMinimumWidth(minWidth);
    thePlot->setMinimumHeight(minHeight);

    mainLayout->addWidget(thePlot);

    thePlot->xAxis->setLabel(xLabel);
    thePlot->yAxis->setLabel(yLabel);
    // thePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);

    this->setLayout(mainLayout);
}


SimCenterGraphPlot::~SimCenterGraphPlot()
{

}


void
SimCenterGraphPlot::setLabels(QString x, QString y)
{
    thePlot->xAxis->setLabel(x);
    thePlot->yAxis->setLabel(y);
}

void
SimCenterGraphPlot::clear() {
    thePlot->clearGraphs();
    thePlot->clearPlottables();
    numGraphs=0;
    atStart = true;
}

void
SimCenterGraphPlot::addLine(QVector<double> &x, QVector<double> &y, int width, int r, int g, int b) {

    //
    // check size of vectors
    //

    int numSteps = y.size();
    if (numSteps != x.size()) {
        qDebug() << "SimCenterGraphPlot - vectors of differing sizes";
        return;
    }


    if (atStart == true) {
        xMinValue = x.at(0);
        yMinValue = y.at(0);
        xMaxValue = xMinValue;
        yMaxValue = yMinValue;
        atStart = false;
    }

    //
    // plot data
    //

    graph = thePlot->addGraph();
    QPen pen;
    pen.setWidth(width);
    pen.setColor(QColor(r,g,b));
    graph->setPen(pen);

    thePlot->graph(numGraphs)->setData(x, y, true);
    numGraphs++;

    for (int i=0; i<numSteps; i++) {
        double yValue = y.at(i);
        double xValue = x.at(i);
        if (yValue < yMinValue)
            yMinValue = yValue;
        if (yValue > yMaxValue)
            yMaxValue = yValue;
        if (xValue < xMinValue)
            xMinValue = xValue;
        if (xValue > xMaxValue)
            xMaxValue = xValue;
    }

    //
    // resize axis if needed
    //

    thePlot->yAxis->setRange(yMinValue, yMaxValue);
    thePlot->xAxis->setRange(xMinValue, xMaxValue);

    thePlot->replot();
}

void
SimCenterGraphPlot::addLine(QVector<double> &x, QVector<double> &y, int width, int r, int g, int b, QString &name) {

    //
    // check size of vectors
    //

    int numSteps = y.size();
    if (numSteps != x.size()) {
        qDebug() << "SimCenterGraphPlot - vectors of differing sizes";
        return;
    }


    if (atStart == true) {
        xMinValue = x.at(0);
        yMinValue = y.at(0);
        xMaxValue = xMinValue;
        yMaxValue = yMinValue;
        atStart = false;
    }

    //
    // plot data
    //

    graph = thePlot->addGraph();
    graph->setName(name);
    QPen pen;
    pen.setWidth(width);
    pen.setColor(QColor(r,g,b));
    graph->setPen(pen);

    thePlot->graph(numGraphs)->setData(x, y, true);
    numGraphs++;

    for (int i=0; i<numSteps; i++) {
        double yValue = y.at(i);
        double xValue = x.at(i);
        if (yValue < yMinValue)
            yMinValue = yValue;
        if (yValue > yMaxValue)
            yMaxValue = yValue;
        if (xValue < xMinValue)
            xMinValue = xValue;
        if (xValue > xMaxValue)
            xMaxValue = xValue;
    }

    //
    // resize axis if needed
    //

    thePlot->yAxis->setRange(yMinValue, yMaxValue);
    thePlot->xAxis->setRange(xMinValue, xMaxValue);

    thePlot->legend->setVisible(true);
    thePlot->replot();
}

