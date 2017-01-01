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
#include "sousvide.h"
#include "ui_sousvide.h"
#include "scanner.h"
#include "cooktimer.h"
#include <QMessageBox>
#include <QRadioButton>

sousvide::sousvide(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::sousvide),
    _control(new SousvideControl(this)),
    _modifyConstants(new ModifyConstants(this, _control)),
    _plot(nullptr),
    _refresh(new QTimer(this)),
    _setpointTemperature(55.0),
    _liveDisplay(new QRadioButton(nullptr)),
    _connectionIcon(new QLabel(nullptr)),
    _serialPort(new QLabel(nullptr)),
    _scanner(new Scanner(this))
{
	ui->setupUi(this);
	_cookTimer = new CookTimer(this, ui->timeRemaining);
	_liveDisplay->setText("");
	_liveDisplay->setStyleSheet("color: rgb(170, 255, 0);");
	//_liveDisplay->setCheckable(false);
	ui->statusBar->addPermanentWidget(_serialPort);
	ui->statusBar->addPermanentWidget(_connectionIcon);
	ui->statusBar->addPermanentWidget(_liveDisplay);
	_plot = new StatusPlot(this, ui->plot);
	populateAvailablePorts();
	_refresh->setInterval(1000);
	/*connect(ui->refreshRate, &QSpinBox::valueChanged, [=](int secs) {
		_refresh->setInterval(1000.0 * secs);
	});*/
	connect(_refresh, &QTimer::timeout, this, &sousvide::refresh);
	connectStatusDisplay();
	connect(ui->actionResetController, &QAction::triggered, [=]() {
		QMessageBox msgBox;
		msgBox.setText(tr("Do You Want to Reset the Controller State?"));
		msgBox.setInformativeText(tr("Usually a reset is not critical and can help with wind-up, but can cause a drop in temperature!"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();
		if(ret == QMessageBox::Yes) {
			_control->softReset();
		}
	});
	connect(ui->actionModifyConstants, &QAction::triggered, _modifyConstants, &ModifyConstants::exec);
	connect(_control, &SousvideControl::receivedConstantP, _modifyConstants, &ModifyConstants::setInitialP);
	connect(ui->actionSetSetpointTemperature, &QAction::triggered, [=]() {
		bool ok = false;
		double temperature = QInputDialog::getDouble(this, tr("Set Setpoint Temperature"), tr("Setpoint °C"), _setpointTemperature, 0.0, 100.0, 1, &ok);
		if(ok) {
			qDebug() << "Setpoint Temperature" << temperature << "°C";
			_control->setSetpointTemperature(temperature);
		}
	});
	connect(ui->actionStartTimer, &QAction::triggered, _cookTimer, &CookTimer::openDialog);
	connect(ui->actionStopTimer, &QAction::triggered, _cookTimer, &CookTimer::stop);
	connect(_cookTimer, &CookTimer::started, [=]() {
		ui->actionStartTimer->setEnabled(false);
		ui->actionStopTimer->setEnabled(true);
	});
	connect(_cookTimer, &CookTimer::stopped, [=]() {
		ui->actionStartTimer->setEnabled(true);
		ui->actionStopTimer->setEnabled(false);
	});
	connect(_control, &SousvideControl::receivedConstantI, _modifyConstants, &ModifyConstants::setInitialI);
	connect(_control, &SousvideControl::receivedConstantD, _modifyConstants, &ModifyConstants::setInitialD);
	connect(ui->actionConnect, &QAction::triggered, _scanner, &Scanner::scan);
	connect(_scanner, &Scanner::foundPort, _control, &SousvideControl::changePort);
	connect(_scanner, &Scanner::noDevice, [=]() {
		QMessageBox msgBox;
		msgBox.setText(tr("No Sousvide Cooker found!"));
		msgBox.setInformativeText(tr("Please connect device and rescan."));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
	});
	_scanner->scan();
	setConnected(false);
}

sousvide::~sousvide()
{
    delete ui;
}

void sousvide::setConnected(bool connected)
{
	ui->actionSetSetpointTemperature->setEnabled(connected);
	ui->actionResetController->setEnabled(connected);
	ui->actionModifyConstants->setEnabled(connected);
	ui->actionSaveInto->setEnabled(connected);
	ui->actionClearGraph->setEnabled(connected);
	ui->actionConnect->setEnabled(!connected);
	ui->groupTemperature->setEnabled(connected);
	ui->groupTimer->setEnabled(connected);
	ui->groupConstants->setEnabled(connected);
	ui->groupState->setEnabled(connected);
	_liveDisplay->setChecked(connected);
	_plot->reset();
	if(connected) {
		_serialPort->setText(_control->getPort());
		_refresh->start();
		QPixmap pixmap = QPixmap ("://icons/connected.png");
		_connectionIcon->setPixmap(pixmap);
	} else {
		_serialPort->setText("");
		_refresh->stop();
		QPixmap pixmap = QPixmap ("://icons/disconnected.png");
		_connectionIcon->setPixmap(pixmap);
	}
}

void sousvide::refresh()
{
	_control->requestInputTemperature();
	_control->requestSetpointTemperature();
	_control->requestPidDiagnostic();
	_control->requestConstantP();
	_control->requestConstantI();
	_control->requestConstantD();
}

void sousvide::populateAvailablePorts()
{
	auto ports = QSerialPortInfo::availablePorts();
	// ui->serialport->clear();
	for(const auto& port: ports) {
		// ui->serialport->addItem(port.portName());
	}
}

void sousvide::connectStatusDisplay()
{
	connect(_control, &SousvideControl::receivedInputTemperature, _plot, &StatusPlot::addInputTemperature);
	connect(_control, &SousvideControl::receivedSetpointTemperature, _plot, &StatusPlot::addSetpointTemperature);
	connect(_control, &SousvideControl::connectionChanged, this, &sousvide::setConnected);
	connect(_control, &SousvideControl::receivedConstantP, [=](double val) {
		ui->const_P->setText(QString::number(val));
		_liveDisplay->setChecked(!_liveDisplay->isChecked());
	});
	connect(_control, &SousvideControl::receivedConstantI, [=](double val) {
		ui->const_I->setText(QString::number(val));
	});
	connect(_control, &SousvideControl::receivedConstantD, [=](double val) {
		ui->const_D->setText(QString::number(val));
	});
	connect(_control, &SousvideControl::receivedInputTemperature, [=](double val) {
		QString str(QString::number(val) + " °C");
		ui->T_input->setText(str);
	});
	connect(_control, &SousvideControl::receivedSetpointTemperature, [=](double val) {
		QString str(QString::number(val) + " °C");
		_setpointTemperature = val;
		ui->T_setpoint->setText(str);
	});
	connect(_control, &SousvideControl::receivedPidDiagnostic,
	        [=](double integral, double derivative, double output, double slow) {
		ui->integral->setText(QString::number(integral) + " °C·s");
		ui->derivative->setText(QString::number(derivative) + " °C/s");
		ui->output->setText(QString::number(output));
		ui->T_smooth->setText(QString::number(slow) + " °C");
	});
}
