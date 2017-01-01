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

#include "scanner.h"
#include "sousvidecontrol.h"
#include <cassert>
#include <QSerialPortInfo>
#include <QDebug>

Scanner::Scanner(QObject* parent) :
 QObject(parent), _control(new SousvideControl(this)), _ports()
{
	connect(_control, &SousvideControl::receivedVersion, this, &Scanner::recvVersion);
	connect(_control, &SousvideControl::failure, this, &Scanner::next);
}

Scanner::~Scanner()
{
}

void Scanner::scan()
{
	auto ports = QSerialPortInfo::availablePorts();
	_ports.clear();
	for(const auto& port: ports) {
		_ports.push_back(port.portName());
	}
	next();
}

void Scanner::next()
{
	if(_ports.size() == 0) {
		_control->changePort("/");
		emit noDevice();
	} else {
		qDebug() << "Test port" << _ports.last();
		_control->changePort(_ports.last());
		_ports.pop_back();
		_control->requestVersion();
	}
}

void Scanner::recvVersion(short major, short minor)
{
	assert(_control);
	QString port = _control->getPort();
	_control->changePort("/");
	emit foundPort(port);
}

