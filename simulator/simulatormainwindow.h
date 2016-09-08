#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class SimulatorMainWindow;
}

class SimulatorMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit SimulatorMainWindow(QWidget *parent = 0);
    ~SimulatorMainWindow();

public slots:
    void onRegisterButtonClicked();
    void onServerOrUUIDTextChanged(const QString &newText);

private:
    Ui::SimulatorMainWindow *ui;

    bool mRegistered;
    QString mServer;
};

#endif // SIMULATORMAINWINDOW_H
