#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

namespace paso {
namespace admin {

class LogDialog : public QDialog {
    Q_OBJECT

public:
    explicit LogDialog(const QString &title, QWidget *parent = 0);
    ~LogDialog();

public slots:
    void appendLine(const QString &line);
    void processingDone();
    virtual void reject() override;

private:
    Ui::LogDialog *ui;
    bool mWorkInProgress;
};
}
}
#endif // LOGDIALOG_H
