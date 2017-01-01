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

#ifndef COOK_TIMER_H
#define COOK_TIMER_H

#include <QObject>
#include <QTime>

class QTimer;
class QLabel;

class CookTimer : public QObject
{
	Q_OBJECT
public:
	explicit CookTimer(QObject* parent, QLabel* output);

public slots:
	void openDialog();
	void start(unsigned int time);
	void stop();

signals:
	void started();
	void timeout();
	void stopped();
	void changeDisplay(QString);

private slots:
	void tick();

private:
	unsigned int _cookSeconds;
	QTime _startTime;
	QTimer* _tickTimer;
	QLabel* _output;
};

#endif // COOK_TIMER_H
