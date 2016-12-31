/*  Sousvide Control Application
    Copyright (C) 2016 Gregor Vollmer <git@dynamic-noise.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef STATUS_PLOT_H
#define STATUS_PLOT_H

#include <QObject>
#include <QTime>

class QCustomPlot;
class QCPGraph;
class QTimer;

class StatusPlot : public QObject
{
	Q_OBJECT
public:
	explicit StatusPlot(QObject* parent, QCustomPlot* plot);

public slots:
	void reset();
	void addInputTemperature(double t);
	void addSetpointTemperature(double t);
	void thinOut();

private:
	void thinOutGraph(QCPGraph* graph);
	QCustomPlot* _plot;
	QTime _startTime;
	QTimer* _timer;
	QCPGraph* _graphInput;
	QCPGraph* _graphSetpoint;
};

#endif // STATUS_PLOT_H
