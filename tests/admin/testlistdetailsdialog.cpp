#include "testlistdetailsdialog.h"

#include "addremoveentitiesform.h"
#include "listdetailsdialog.h"
#include "pasodb.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTableView>
#include <QtConcurrent>

using namespace std;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::admin;
using namespace paso::widget;

TestListDetailsDialog::TestListDetailsDialog() : TestBase() {}

void TestListDetailsDialog::testReadOnly() {
    QVariantMap listMap{
        {"ID", 1}, {"NAME", "Lista"}, {"SYSTEM", true}, {"PERMANENT", true}};
    List systemList(listMap);
    listMap["SYSTEM"] = false;
    List permanentList(listMap);
    listMap["PERMANENT"] = false;
    List regularList;

    ListDetailsDialog systemListDialog(systemList);
    systemListDialog.show();
    QTest::qWaitForWindowExposed(&systemListDialog);
    auto addRemoveEntitiesForm =
        systemListDialog.findChild<AddRemoveEntitiesForm *>();
    auto buttonBox = systemListDialog.findChild<QDialogButtonBox *>();
    auto importButton =
        systemListDialog.findChild<QPushButton *>("IMPORT_BUTTON");
    auto saveButton = buttonBox->button(QDialogButtonBox::Save);
    QVERIFY(addRemoveEntitiesForm->readOnly());
    QVERIFY(importButton == nullptr);
    QVERIFY(saveButton == nullptr);

    ListDetailsDialog permanentListDialog(permanentList);
    permanentListDialog.show();
    QTest::qWaitForWindowExposed(&permanentListDialog);
    addRemoveEntitiesForm =
        permanentListDialog.findChild<AddRemoveEntitiesForm *>();
    buttonBox = permanentListDialog.findChild<QDialogButtonBox *>();
    importButton =
        permanentListDialog.findChild<QPushButton *>("IMPORT_BUTTON");
    saveButton = buttonBox->button(QDialogButtonBox::Save);
    QVERIFY(!addRemoveEntitiesForm->readOnly());
    QVERIFY(importButton != nullptr);
    QVERIFY(saveButton != nullptr);

    ListDetailsDialog regularListDialog(regularList);
    regularListDialog.show();
    QTest::qWaitForWindowExposed(&regularListDialog);
    addRemoveEntitiesForm =
        regularListDialog.findChild<AddRemoveEntitiesForm *>();
    buttonBox = regularListDialog.findChild<QDialogButtonBox *>();
    importButton = regularListDialog.findChild<QPushButton *>("IMPORT_BUTTON");
    saveButton = buttonBox->button(QDialogButtonBox::Save);
    QVERIFY(!addRemoveEntitiesForm->readOnly());
    QVERIFY(importButton != nullptr);
    QVERIFY(saveButton != nullptr);
}
