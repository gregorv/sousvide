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
