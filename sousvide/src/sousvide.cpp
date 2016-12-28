#include "sousvide.h"
#include "ui_sousvide.h"
#include <QMessageBox>

sousvide::sousvide(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::sousvide),
    _control(new SousvideControl(this)),
    _modifyConstants(new ModifyConstants(this, _control)),
    _refresh(new QTimer(this))
{
	ui->setupUi(this); 
	ui->plot->xAxis->setLabel(tr("Time (s)"));
	ui->plot->yAxis->setLabel(tr("Temperature (°C)"));
	populateAvailablePorts();
	connect(_control, &SousvideControl::connectionChanged, ui->connect, &QPushButton::setDisabled);
	connect(ui->connect, &QPushButton::clicked, [=]() {
		_control->changePort(ui->serialport->currentText());
	});
	connect(ui->serialport, &QComboBox::currentTextChanged, [=](const QString& port) {
		ui->connect->setDisabled(port == _control->getPort());
	});
	_refresh->setInterval(1000);
	/*connect(ui->refreshRate, &QSpinBox::valueChanged, [=](int secs) {
		_refresh->setInterval(1000.0 * secs);
	});*/
	connect(_refresh, &QTimer::timeout, this, &sousvide::refresh);
	_refresh->start();
	connectStatusDisplay();
	connect(ui->resetTemp, &QPushButton::clicked, [=]() {
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
	connect(ui->modifyConsts, &QPushButton::clicked, _modifyConstants, &ModifyConstants::exec);
	connect(_control, &SousvideControl::receivedConstantP, _modifyConstants, &ModifyConstants::setInitialP);
	connect(ui->setTemp, &QPushButton::clicked, [=]() {
		_control->setSetpointTemperature(ui->setpointTemp->value());
	});
	connect(_control, &SousvideControl::receivedConstantI, _modifyConstants, &ModifyConstants::setInitialI);
	connect(_control, &SousvideControl::receivedConstantD, _modifyConstants, &ModifyConstants::setInitialD);
}

sousvide::~sousvide()
{
    delete ui;
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
	ui->serialport->clear();
	for(const auto& port: ports) {
		ui->serialport->addItem(port.portName());
	}
}

void sousvide::connectStatusDisplay()
{
	connect(_control, &SousvideControl::receivedConstantP, [=](double val) {
		ui->const_P->setText(QString::number(val));
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
		ui->T_setpoint->setText(str);
	});
	connect(_control, &SousvideControl::receivedPidDiagnostic,
	        [=](double integral, double derivative, double output, double slow) {
		ui->integral->setText(QString::number(integral) + " °C·s");
		ui->derivative->setText(QString::number(derivative) + " °C/s");
		ui->output->setText(QString::number(output) + " °C/s");
		ui->T_smooth->setText(QString::number(slow) + " °C");
	});
}
