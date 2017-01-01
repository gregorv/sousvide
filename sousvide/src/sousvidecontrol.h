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
#ifndef _SOUSVIDE_CONTROL_H_
#define _SOUSVIDE_CONTROL_H_

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <queue>
#include <vector>
#include <memory>

class SousvideControl : public QObject
{
	Q_OBJECT
public:
	SousvideControl(QObject* parent=nullptr);
	virtual ~SousvideControl();

	size_t commandQueueSize() const;
	QString getPort() const;

	static QString toHex(unsigned char value);
	static QString toHex(double value);
	static double toDouble(QString hex);
	static int toInt(QString hex);
	static unsigned char toUchar(QString hex);

public slots:
	void changePort(QString name);

	void setConstantP(double val);
	void setConstantI(double val);
	void setConstantD(double val);
	void setSetpointTemperature(double val);
	void requestConstantP();
	void requestConstantI();
	void requestConstantD();
	void requestSetpointTemperature();
	void requestInputTemperature();
	void requestPidDiagnostic();
	void requestVersion();
	void softReset();

signals:
	void connectionChanged(bool);
	void connectionError(QSerialPort::SerialPortError);
	void receivedConstantP(double val);
	void receivedConstantI(double val);
	void receivedConstantD(double val);
	void receivedSetpointTemperature(double val);
	void receivedInputTemperature(double val);
	void receivedPidDiagnostic(double integral, double derivative, double output, double slowTemp);
	void receivedFailState(int);
	void receivedVersion(short, short);
	void failure(bool timeout, int type);

private slots:
	void dataAvailable();
	void dispatchCommand();
	void timeout();

private:
	QSerialPort* _serial;
	QTimer* _timeoutTimer;
	enum command_type_t {
		CMD_GET_P = 0x00,
		CMD_GET_I = 0x01,
		CMD_GET_D = 0x02,
		CMD_GET_TS = 0x03,
		CMD_GET_TI = 0x04,
		CMD_SET_P = 0x05,
		CMD_SET_I = 0x06,
		CMD_SET_D = 0x07,
		CMD_SET_TS = 0x08,
		CMD_PID_DIAG,
		CMD_SOFT_RESET,
		CMD_VERSION = 0x76,
		CMD_NO_CMD = 0xff
	};
	struct command_t {
		command_type_t cmd;
		std::vector<double> args;
	};
	std::queue<command_t> _queue;
	bool _awaitResponse;
};

#endif//_SOUSVIDE_CONTROL_H_
