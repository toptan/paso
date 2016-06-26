#include "logdialog.h"
#include "ui_logdialog.h"

#include <QDebug>

namespace paso {
namespace admin {

LogDialog::LogDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LogDialog) {
    ui->setupUi(this);
}

LogDialog::~LogDialog() {
    delete ui;
    qDebug() << "Log dialog destroyed.";
}

void LogDialog::appendLine(const QString &line) {
    ui->messages->appendPlainText(line);
}

void LogDialog::processingDone() {

}
}
}
