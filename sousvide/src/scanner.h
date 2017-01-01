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
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <QObject>
#include <QVector>

class SousvideControl;

class Scanner : public QObject
{
	Q_OBJECT
public:
	explicit Scanner(QObject* parent=nullptr);
	virtual ~Scanner();

public slots:
	void scan();

signals:
	void foundPort(QString port);
	void noDevice();

private slots:
	void next();
	void recvVersion(short major, short minor);

private:
	SousvideControl* _control;
	QVector<QString> _ports;
};

#endif//SCANNER_H
