#ifndef SOUSVIDE_H
#define SOUSVIDE_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include "sousvidecontrol.h"
#include "modifyconstants.h"

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
    QTimer* _refresh;
};

#endif // SOUSVIDE_H
