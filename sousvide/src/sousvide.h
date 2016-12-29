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
#ifndef SOUSVIDE_H
#define SOUSVIDE_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include "sousvidecontrol.h"
#include "modifyconstants.h"
#include "statusplot.h"

namespace Ui {
class sousvide;
}

class sousvide : public QMainWindow
{
    Q_OBJECT

public:
    explicit sousvide(QWidget *parent = 0);
    ~sousvide();

private slots:
	void refresh();

private:
	void populateAvailablePorts();
	void connectStatusDisplay();

    Ui::sousvide *ui;
    SousvideControl* _control;
    ModifyConstants* _modifyConstants;
    StatusPlot* _plot;
    QTimer* _refresh;
};

#endif // SOUSVIDE_H
