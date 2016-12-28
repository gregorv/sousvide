#include "sousvidecontrol.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <QDebug>

SousvideControl::SousvideControl(QObject* parent) :
 QObject(parent), _serial(new QSerialPort(this)), _timeoutTimer(new QTimer(this)),
 _queue(), _awaitResponse(false)
{
	connect(_timeoutTimer, &QTimer::timeout, this, &SousvideControl::timeout);
	_timeoutTimer->setInterval(500);
	_timeoutTimer->setSingleShot(true);
	connect(_serial, &QSerialPort::readyRead, this, &SousvideControl::dataAvailable);
}

SousvideControl::~SousvideControl()
{
}

size_t SousvideControl::commandQueueSize() const
{
	return _queue.size();
}

QString SousvideControl::getPort() const
{
	return _serial->portName();
}

QString SousvideControl::toHex(unsigned char value)
{
	QString hex;
	hex += (char)('0' + ((value&0xf0)>>4));
	hex += (char)('0' + (value&0x0f));
	return hex;
}

QString SousvideControl::toHex(double value)
{
	value = std::min<double>(std::max<double>(
			 value,
	                 std::numeric_limits<short>::min()),
			 std::numeric_limits<short>::max());
	short integer = static_cast<short>(value);
	int fractional = static_cast<int>(std::abs(value - static_cast<int>(value))*0xffff);
	QString hex;
	hex += toHex(static_cast<unsigned char>(integer>>8));
	hex += toHex(static_cast<unsigned char>(integer));
	hex += toHex(static_cast<unsigned char>(fractional>>8));
	hex += toHex(static_cast<unsigned char>(fractional));
	return hex;
}

double SousvideControl::toDouble(QString hex)
{
	hex = hex.left(8);
	int i = toInt(hex);
	unsigned int frac = i&0xffff;
	double x = static_cast<double>(i>>16) + static_cast<double>(frac)/0xffff;
	qDebug() << "toDouble" << hex << x;
	return x;
}

int SousvideControl::toInt(QString hex)
{
	hex = hex.left(8);
	int val = 0;
	int digits = 0;
	for(const auto& ch: hex) {
		if('0' <= ch && ch <= '0'+15) {
			val <<= 4;
			val += ch.toLatin1() - '0';
		}
		if(++digits == 8) {
			break;
		}
	}
	qDebug() << "toInt" << hex << QByteArray::number(val, 16);
	return val;
}

unsigned char SousvideControl::toUchar(QString hex)
{
	hex = hex.left(2);
	int val = 0;
	int digits = 0;
	for(const auto& ch: hex) {
		if('0' <= ch && ch <= '0'+15) {
			val <<= 4;
			val += ch.toLatin1() - '0';
		}
		if(++digits == 2) {
			break;
		}
	}
	qDebug() << "toUchar" << hex << val;
	return val;
}

void SousvideControl::changePort(QString name)
{
	if(_serial->isOpen()) {
		emit connectionChanged(false);
		_serial->close();
	}
	_timeoutTimer->stop();
	_awaitResponse = false;
	while(!_queue.empty()) {
		_queue.pop();
	}
	_serial->setBaudRate(9600);
	_serial->setPortName(name);
	if(!_serial->open(QIODevice::ReadWrite)) {
		// TODO: Error reporting
		emit connectionError(_serial->error());
	} else {
		emit connectionChanged(true);
	}
}

void SousvideControl::setConstantP(double val)
{
	_queue.push({CMD_SET_P, {val}});
	dispatchCommand();
}

void SousvideControl::setConstantI(double val)
{
	_queue.push({CMD_SET_I, {val}});
	dispatchCommand();
}

void SousvideControl::setConstantD(double val)
{
	_queue.push({CMD_SET_D, {val}});
	dispatchCommand();
}

void SousvideControl::setSetpointTemperature(double val)
{
	_queue.push({CMD_SET_TS, {val}});
	dispatchCommand();
}

void SousvideControl::requestConstantP()
{
	_queue.push({CMD_GET_P, {}});
	dispatchCommand();
}

void SousvideControl::requestConstantI()
{
	_queue.push({CMD_GET_I, {}});
	dispatchCommand();
}

void SousvideControl::requestConstantD()
{
	_queue.push({CMD_GET_D, {}});
	dispatchCommand();
}

void SousvideControl::requestSetpointTemperature()
{
	_queue.push({CMD_GET_TS, {}});
	dispatchCommand();
}

void SousvideControl::requestInputTemperature()
{
	_queue.push({CMD_GET_TI, {}});
	dispatchCommand();
}

void SousvideControl::requestPidDiagnostic()
{
	_queue.push({CMD_PID_DIAG, {}});
	dispatchCommand();
}

void SousvideControl::softReset()
{
	_queue.push({CMD_SOFT_RESET, {}});
	dispatchCommand();
}

void SousvideControl::dataAvailable()
{
	if(!_serial->canReadLine()) {
		return;
	}
	auto line = _serial->readLine();
	qDebug() << "RESPONSE: '" << line << "'";
	_awaitResponse = false;
	_timeoutTimer->stop();
	dispatchCommand();

	QString state = line.left(4);
	command_type_t cmd = static_cast<command_type_t>(toUchar(&line.data()[4]));
	if(state == "OKAY") {
		QString argstr = line.right(line.size()-6);
		argstr = argstr.left(argstr.size()-2);
		if(argstr.size()%8 != 0) {
			qDebug() << " -> Syntax Error! argstrlen=" << argstr.size()
			         << argstr;
			return;
		}
		std::vector<double> args;
		for(size_t i = 0;  i < argstr.size(); i += 8) {
			args.push_back(toDouble(argstr.mid(i, 8)));
		}
		try {
			if(cmd == CMD_GET_P) {
				emit receivedConstantP(args.at(0));
			} else if(cmd == CMD_GET_I) {
				emit receivedConstantI(args.at(0));
			} else if(cmd == CMD_GET_D) {
				emit receivedConstantD(args.at(0));
			} else if(cmd == CMD_GET_TS) {
				emit receivedSetpointTemperature(args.at(0));
			} else if(cmd == CMD_GET_TI) {
				emit receivedInputTemperature(args.at(0));
			} else if(cmd == CMD_PID_DIAG) {
				emit receivedPidDiagnostic(args.at(0), args.at(1), args.at(2), args.at(3));
			} else {
				qDebug() << " -> Unknown Command " << cmd;
			}
		} catch(std::out_of_range& e) {
			qDebug() << " -> Tried to access not-sent argument" << e.what();
		}
	} else {
		qDebug() << " -> Failure-Response! " << line;
	}
}

void SousvideControl::dispatchCommand()
{
	if(_awaitResponse) {
		return;
	}
	if(_queue.size() == 0) {
		return;
	}
	auto cmd = _queue.front();
	_queue.pop();
	QString request;
	request += toHex(static_cast<unsigned char>(cmd.cmd));
	for(const auto& arg: cmd.args) {
		request += toHex(arg);
	}
	request += "\r\n";
	QByteArray req;
	_awaitResponse = true;
	req.append(request);
	_serial->write(req);
	qDebug() << "REQUEST: " << request;
	_timeoutTimer->start();
}

void SousvideControl::timeout()
{
	qDebug() << "TIMEOUT!";
	if(_awaitResponse) {
		_awaitResponse = false;
		dispatchCommand();
	}
}
