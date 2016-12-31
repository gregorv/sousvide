#include "statusplot.h"

#include "qcustomplot.h"
#include <QTimer>

StatusPlot::StatusPlot(QObject* parent, QCustomPlot* plot) :
 QObject(parent), _plot(plot),
 _startTime(),
 _timer(new QTimer(this)),
 _graphInput(plot->addGraph()),
 _graphSetpoint(plot->addGraph())
{
	_plot->xAxis->setLabel(tr("Time (min)"));
	_plot->yAxis->setLabel(tr("Temperature (°C)"));
	_timer->setInterval(60000);
	connect(_timer, &QTimer::timeout, this, &StatusPlot::thinOut);
	reset();
}

void StatusPlot::reset()
{
	_timer->stop();
	_startTime = QTime::currentTime();
	_timer->start();
	/*for(double t = -1000; t < 0; t += 1.0) {
		double x = (1.0 - exp(-(t+1000)/100)) * 70.0;
		_graphInput->addData(t/60, x + 0.1*(static_cast<int>(t)%9));
		_graphSetpoint->addData(t/60, 70);
	}
	thinOut();*/
}

void StatusPlot::addInputTemperature(double t)
{
	double seconds = _startTime.secsTo(QTime::currentTime());
	qDebug() << "addInputTemperature" << seconds/60.0 << t;
	_graphInput->addData(seconds/60.0, t);
	_plot->rescaleAxes();
	_plot->replot();
}

void StatusPlot::addSetpointTemperature(double t)
{
	double seconds = _startTime.secsTo(QTime::currentTime());
	_graphSetpoint->addData(seconds/60.0, t);
}

void StatusPlot::thinOut()
{
	qDebug() << "thinOut";
	thinOutGraph(_graphInput);
	thinOutGraph(_graphSetpoint);
	_plot->rescaleAxes();
	_plot->replot();
}

void StatusPlot::thinOutGraph(QCPGraph* graph)
{
	// Should implement gaussian smoothing
	const double smoothStopTime = 300;
	const double smoothSigma = 10;
	const double pointDist = 60;
	double maxSmoothTime = _startTime.secsTo(QTime::currentTime()) - smoothStopTime;
	QVector<double> t;
	QVector<double> x;
	t.reserve(maxSmoothTime/pointDist);
	x.reserve(maxSmoothTime/pointDist);
	for(const auto& point: *graph->data()) {
		double ti = point.key*60;
		double xi = point.value;
		if(static_cast<int>(ti)%60 == 0 ||
		   ti > maxSmoothTime) {
			t.push_back(ti/60);
			x.push_back(xi);
		}
	}
	graph->setData(t, x, true);
}
