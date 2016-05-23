#include "logindialog.h"
#include "ui_logindialog.h"

namespace paso {
namespace admin {

using namespace Ui;

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
}

LoginDialog::~LoginDialog() { delete ui; }
}
}
