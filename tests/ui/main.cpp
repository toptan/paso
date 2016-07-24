#include <QTest>
#include <QSqlDatabase>

#include "testforms.h"
#include "testwidgets.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    auto db = QSqlDatabase::addDatabase("QSQLITE", "paso");
    db.setDatabaseName(":memory:");
    db.open();
    TestWidgets testWidgets;
    TestForms testForms;

    auto retVal = QTest::qExec(&testWidgets, argc, argv);
    if (retVal != 0) {
        return retVal;
    }
    retVal = QTest::qExec(&testForms, argc, argv);
    return retVal;
}
