#include "modifyconstants.h"
#include "ui_pidconst.h"
#include "sousvidecontrol.h"

ModifyConstants::ModifyConstants(QWidget *parent, SousvideControl* ctrl) :
    QDialog(parent),
    ui(new Ui::pidconst),
    _control(ctrl),
    _p0(0.0), _i0(0.0), _d0(0.0)
{
	ui->setupUi(this); 
}

ModifyConstants::~ModifyConstants()
{
    delete ui;
}

void ModifyConstants::reject()
{
	QDialog::reject();
}

void ModifyConstants::accept()
{
	apply();
	QDialog::accept();
}

void ModifyConstants::apply()
{
	double p = ui->p->value();
	double i = ui->i->value();
	double d = ui->d->value();
	_control->setConstantP(p);
	_control->setConstantI(i);
	_control->setConstantD(d);
	setInitialP(p);
	setInitialI(i);
	setInitialD(d);
}

int ModifyConstants::exec()
{
	ui->p->setValue(_p0);
	ui->i->setValue(_i0);
	ui->d->setValue(_d0);
	QDialog::exec();
}

void ModifyConstants::setInitialP(double p)
{
	_p0 = p;
}

void ModifyConstants::setInitialI(double i)
{
	_i0 = i;
}

void ModifyConstants::setInitialD(double d)
{
	_d0 = d;
}

