#include "testlogindialog.h"

#include "logindialog.h"
#include <QLabel>
#include <QLineEdit>

using namespace paso::admin;

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
