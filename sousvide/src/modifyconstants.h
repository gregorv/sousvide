#ifndef MODIFY_CONSTANTS_H
#define MODIFY_CONSTANTS_H

#include <QDialog>

namespace Ui {
class pidconst;
}

class SousvideControl;

class ModifyConstants : public QDialog
{
public:
    explicit ModifyConstants(QWidget *parent, SousvideControl* ctrl);
    ~ModifyConstants();

public slots:
	virtual void reject();
	virtual void apply();
	virtual void accept();
	virtual int exec();
	virtual void setInitialP(double p);
	virtual void setInitialI(double i);
	virtual void setInitialD(double d);

private:
	void populateAvailablePorts();
	void connectStatusDisplay();

	double _p0;
	double _i0;
	double _d0;

    Ui::pidconst *ui;
    SousvideControl* _control;
};

#endif // MODIFY_CONSTANTS_H
