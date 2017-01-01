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

#include "cooktimer.h"

#include <QTimer>
#include <QLabel>
#include <QInputDialog>
#include <sstream>
#include <iomanip>

CookTimer::CookTimer(QObject* parent, QLabel* output) : 
 QObject(parent), _cookSeconds(0), _startTime(), _tickTimer(new QTimer(this)), _output(output)
{
	_tickTimer->setInterval(1000);
	connect(this, &CookTimer::changeDisplay, output, &QLabel::setText);
	connect(_tickTimer, &QTimer::timeout, this, &CookTimer::tick);
}

void CookTimer::openDialog()
{
	bool ok = false;
	double time = QInputDialog::getDouble(_output, tr("Start Timer"), tr("Cook time (hours)"), 1.5, 0.0, 100.0, 1, &ok);
	if(ok) {
		start(time*3600);
	}
}

void CookTimer::start(unsigned int seconds)
{
	_cookSeconds = seconds;
	_startTime = QTime::currentTime();
	_tickTimer->start();
	emit started();
}

void CookTimer::stop()
{
	_tickTimer->stop();
	emit changeDisplay("00:00:00");
	emit stopped();
}

void CookTimer::tick()
{
	unsigned int elapsed = _startTime.secsTo(QTime::currentTime());
	if(elapsed >= _cookSeconds) {
		emit timeout();
		stop();
	} else {
		unsigned int remaining = _cookSeconds - elapsed;
		unsigned int hours = remaining / 3600;
		unsigned int mins = (remaining-hours*3600) / 60;
		unsigned int secs = (remaining-hours*3600-mins*60);
		QString str = QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')) \
			      .arg(mins, 2, 10, QChar('0')) \
		              .arg(secs, 2, 10, QChar('0'));
		emit changeDisplay(str);
	}
}
