#include "logdialog.h"
#include "ui_logdialog.h"

#include <QDebug>

namespace paso {
namespace admin {

LogDialog::LogDialog(const QString &title, QWidget *parent)
    : QDialog(parent), ui(new Ui::LogDialog), mWorkInProgress(true) {
    ui->setupUi(this);
    setWindowTitle(title);
    setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                   ~Qt::WindowMinMaxButtonsHint);
    ui->buttonBox->setVisible(false);
}

LogDialog::~LogDialog() { delete ui; }

void LogDialog::appendLine(const QString &line) {
    ui->messages->appendPlainText(line);
}

void LogDialog::processingDone() {
    ui->buttonBox->setVisible(true);
    mWorkInProgress = false;
    ui->messages->ensureCursorVisible();
}

void LogDialog::reject() {
    if (!mWorkInProgress) {
        QDialog::reject();
    }
}
}
}
