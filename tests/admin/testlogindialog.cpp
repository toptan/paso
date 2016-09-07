#include "testlogindialog.h"

#include "commdata.h"
#include "logindialog.h"
#include "systemuser.h"

#include <QDataStream>
#include <QLabel>
#include <QLineEdit>
#include <QTcpSocket>
#include <QtConcurrent>

using namespace paso::admin;
using namespace paso::comm;
using namespace paso::data;
using namespace paso::data::entity;

TestLoginDialog::TestLoginDialog() : TestBase() {}

void TestLoginDialog::testEntryValidation() {
    LoginDialog dialog;
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);

    auto usernameEdit = dialog.findChild<QLineEdit *>("usernameLineEdit");
    auto passwordEdit = dialog.findChild<QLineEdit *>("passwordLineEdit");
    auto serverEdit = dialog.findChild<QLineEdit *>("serverLineEdit");
    auto messageLabel = dialog.findChild<QLabel *>("messageLabel");

    usernameEdit->clear();
    passwordEdit->clear();
    serverEdit->clear();
    dialog.accept();
    QApplication::processEvents();
    QCOMPARE(messageLabel->text(), QString("You need to provide username."));
    QVERIFY(usernameEdit->hasFocus());

    usernameEdit->setText("username");
    dialog.accept();
    QApplication::processEvents();
    QCOMPARE(messageLabel->text(), QString("You need to provide password."));
    QVERIFY(passwordEdit->hasFocus());

    passwordEdit->setText("password");
    dialog.accept();
    QApplication::processEvents();
    QCOMPARE(messageLabel->text(), QString("You need to provide server name."));
    QVERIFY(serverEdit->hasFocus());
}
