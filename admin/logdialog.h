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
    explicit LogDialog(QWidget *parent = 0);
    ~LogDialog();

public slots:
    void appendLine(const QString &line);
    void processingDone();

private:
    Ui::LogDialog *ui;
};
}
}
#endif // LOGDIALOG_H
