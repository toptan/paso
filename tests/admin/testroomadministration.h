#ifndef TESTROOMADMINISTRATION_H
#define TESTROOMADMINISTRATION_H

#include <QTest>

class TestRoomAdministration : public QObject {
    Q_OBJECT
public:
    TestRoomAdministration();

private slots:

    void initTestCase();
    void init();
    void cleanup();

    void testRoomValidator();
    void testRoomEditorWidget();
    void testRoomTableModel();

private:
    const QString dbName;
};

#endif // TESTROOMADMINISTRATION_H
