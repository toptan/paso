#include "roomeditorwidget.h"

#include "addremoveentitiesform.h"
#include "data.h"
#include "entitytablemodel.h"
#include "pasodb.h"
#include "room.h"
#include "student.h"

#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QTableView>
#include <QUuid>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;
using namespace paso::widget;

using namespace std;

namespace paso {
namespace admin {

RoomEditorWidget::RoomEditorWidget(const FieldTypes &fieldTypes,
                                   QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void RoomEditorWidget::setupUi(const QVariantMap &columnLabels,
                               const QSqlRecord &record, const QStringList &) {
    RecordEditorWidget::setupUi(columnLabels, record, {"barred_students"});
    QFormLayout *l = dynamic_cast<QFormLayout *>(layout());

    mChangeBarringButton = new QPushButton(tr("Change barring"), this);
    mChangeBarringButton->setObjectName("barringButton");
    connect(mChangeBarringButton, &QPushButton::clicked, this,
            &RoomEditorWidget::onChangeBarringButtonClicked);

    auto buttonBox = findChild<QDialogButtonBox *>();
    buttonBox->addButton(mChangeBarringButton, QDialogButtonBox::ActionRole);

    const QStringList columns{"INDEX_NUMBER", "LAST_NAME", "FIRST_NAME"};
    const QMap<QString, QString> columnNames{
        {"INDEX_NUMBER", tr("Index number")},
        {"LAST_NAME", tr("Last name")},
        {"FIRST_NAME", tr("First name")}};

    mOccupancyTableView = new QTableView();
    mOccupancyTableView->setObjectName("occupancyTableView");
    mExportOccupancy = new QPushButton(tr("Export"), this);
    mExportOccupancy->setEnabled(false);
    auto vLayout = new QVBoxLayout;
    auto hLayout = new QHBoxLayout;
    hLayout->addStretch(1);
    hLayout->addWidget(mExportOccupancy);
    vLayout->addWidget(mOccupancyTableView);
    vLayout->addItem(hLayout);
    l->insertRow(l->rowCount() - 1, new QLabel(tr("Occupancy"), this), vLayout);
    mOccupancyTableModel = new QSqlQueryModel(this);
    auto query =
        Room::occupancyQuery(QSqlDatabase::database(DEFAULT_DB_NAME), 0);
    query.exec();
    auto occupancySortModel = new QSortFilterProxyModel(this);
    occupancySortModel->setSourceModel(mOccupancyTableModel);
    mOccupancyTableModel->setQuery(query);
    mOccupancyTableView->setModel(occupancySortModel);
    mOccupancyTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mOccupancyTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    mOccupancyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mOccupancyTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    mOccupancyTableView->horizontalHeader()->setCascadingSectionResizes(true);
    mOccupancyTableView->horizontalHeader()->setStretchLastSection(true);
    mOccupancyTableView->horizontalHeader()->model()->setHeaderData(
        0, Qt::Horizontal, tr("From"), Qt::DisplayRole);
    mOccupancyTableView->horizontalHeader()->model()->setHeaderData(
        1, Qt::Horizontal, tr("To"), Qt::DisplayRole);
    mOccupancyTableView->horizontalHeader()->model()->setHeaderData(
        2, Qt::Horizontal, tr("Activity"), Qt::DisplayRole);
    mOccupancyTableView->setSortingEnabled(true);
    mOccupancyTableView->setAlternatingRowColors(true);
    mOccupancyTableView->hideColumn(3);
    mOccupancyTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    EntityVector emptyData;
    mBarredStudentsTableView = new QTableView(this);
    mBarredStudentsTableView->setObjectName("barredTableView");
    mBarredStudentsModel =
        new EntityTableModel(columns, columnNames, emptyData, this);
    auto barredStudentsSortModel = new QSortFilterProxyModel(this);
    barredStudentsSortModel->setSourceModel(mBarredStudentsModel);
    mBarredStudentsTableView->setModel(barredStudentsSortModel);
    mBarredStudentsTableView->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    mBarredStudentsTableView->setSelectionMode(
        QAbstractItemView::SingleSelection);
    mBarredStudentsTableView->setSelectionBehavior(
        QAbstractItemView::SelectRows);
    mBarredStudentsTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    mBarredStudentsTableView->horizontalHeader()->setCascadingSectionResizes(
        true);
    mBarredStudentsTableView->horizontalHeader()->setStretchLastSection(true);
    mBarredStudentsTableView->setSortingEnabled(true);
    mBarredStudentsTableView->setAlternatingRowColors(true);
    mBarredStudentsTableView->sortByColumn(1, Qt::SortOrder::AscendingOrder);
    l->insertRow(l->rowCount() - 1, tr("Barred students"),
                 mBarredStudentsTableView);
    connect(this, &RecordEditorWidget::editFinished, [this]() {
        mExportOccupancy->setEnabled(true);
        mOccupancyTableView->setEnabled(true);
        mBarredStudentsTableView->setEnabled(true);
    });
    connect(mExportOccupancy, &QPushButton::clicked, this,
            &RoomEditorWidget::onExportOccupancyClicked);
}

void RoomEditorWidget::onDisplayRecord(const QSqlRecord &record) {
    RecordEditorWidget::onDisplayRecord(record);
    QSqlError error;
    mRoomId = record.value("id").toULongLong();
    auto barred = mManager.barredStudents(mRoomId, error);
    mBarredStudentsModel->setEntityData(barred);
    auto query =
        Room::occupancyQuery(QSqlDatabase::database(DEFAULT_DB_NAME), mRoomId);
    query.exec();
    mOccupancyTableModel->setQuery(query);
    mExportOccupancy->setEnabled(mOccupancyTableModel->rowCount() != 0);
}

void RoomEditorWidget::prepareEdit(QSqlRecord &record) {
    if (record.field("room_uuid").isNull()) {
        record.setValue("room_uuid", QUuid::createUuid());
    }
}

bool RoomEditorWidget::fieldReadOnly(const QString &key) {
    return key == "room_uuid";
}

QLineEdit *RoomEditorWidget::createLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createLineEdit(field);
    if (field == "name") {
        retVal->setMaxLength(64);
    } else if (field == "room_number") {
        retVal->setMaxLength(8);
    }
    return retVal;
}

void RoomEditorWidget::onChangeBarringButtonClicked() {
    QSqlError error;
    auto roomId = mNewRecord ? 0 : mRecord.value("id").toULongLong();
    const QStringList columns{"INDEX_NUMBER", "LAST_NAME", "FIRST_NAME"};
    const QMap<QString, QString> columnNames{
        {"INDEX_NUMBER", tr("Index number")},
        {"LAST_NAME", tr("Last name")},
        {"FIRST_NAME", tr("First name")}};
    auto nonBarred = mManager.allowedStudents(roomId, error);
    auto barred = mManager.barredStudents(roomId, error);

    QDialog dlg(this);
    auto layout = new QHBoxLayout(&dlg);
    dlg.setLayout(layout);

    auto form = new AddRemoveEntitiesForm(&dlg);
    form->setData(tr("Allowed students"), columns, columnNames, nonBarred,
                  tr("Barred students"), columns, columnNames, barred);
    layout->addWidget(form);
    auto buttonBox = new QDialogButtonBox(Qt::Vertical, &dlg);
    buttonBox->addButton(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->addButton(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    dlg.setWindowTitle(tr("Change room barring"));
    dlg.setModal(true);
    auto result = dlg.exec();
    if (result != QDialog::Accepted) {
        return;
    }
    mBarredStudentsModel->setEntityData(form->destinationEntities());
}

void RoomEditorWidget::clearData() {
    RecordEditorWidget::clearData();
    EntityVector emptyData;
    mBarredStudentsModel->setEntityData(emptyData);
}

void RoomEditorWidget::accepted() {
    shared_ptr<ValidationError> error;
    if (validator() != nullptr) {
        error = validator()->validate(mRecord);
    }

    if (error) {
        showEntryError(error->editor, error->title, error->text,
                       error->detailedText, error->icon);
        return;
    }

    for (const auto &key : fieldEditors().keys()) {
        auto field = fieldEditors()[key];
        auto fieldType = fieldTypes()[key];
        QVariant value = dynamic_cast<QLineEdit *>(field)->text().trimmed();
        mRecord.setValue(key, value);
    }
    QVariantList ids;
    for (const auto &entity : mBarredStudentsModel->data()) {
        ids << entity->id();
    }
    mRecord.setValue("barred_students", variantListToJsonArrayString(ids));

    if (mNewRecord) {
        emit requestSave(mRecord);
    } else {
        emit requestUpdate(mRecord);
    }
    mExportOccupancy->setEnabled(true);
    mOccupancyTableView->setEnabled(true);
    mBarredStudentsTableView->setEnabled(true);
}

void RoomEditorWidget::onEditExistingRecord(QSqlRecord record) {
    RecordEditorWidget::onEditExistingRecord(record);
    mExportOccupancy->setEnabled(false);
    mOccupancyTableView->setEnabled(false);
    mBarredStudentsTableView->setEnabled(false);
}

void RoomEditorWidget::onEditNewRecord(QSqlRecord record) {
    RecordEditorWidget::onEditNewRecord(record);
    mExportOccupancy->setEnabled(false);
    mOccupancyTableView->setEnabled(false);
    mBarredStudentsTableView->setEnabled(false);
}

void RoomEditorWidget::onExportOccupancyClicked() {
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Select file to export room occupancy"), QString(), QString(),
        nullptr, QFileDialog::DontUseNativeDialog);
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error"));
        msgBox.setText(
            tr("The export file '%1' could not be created.").arg(fileName));
        msgBox.setDetailedText(file.errorString());
        msgBox.exec();
        return;
    }
    auto roomNumber = mRecord.value("room_number").toString();
    auto query =
        Room::occupancyQuery(QSqlDatabase::database(DEFAULT_DB_NAME), mRoomId);
    query.exec();
    QTextStream out(&file);
    while (query.next()) {
        out << query.value("room_number").toString() << ","
            << query.value("name").toString() << ","
            << query.value("start").toDateTime().toString(Qt::ISODate) << ","
            << query.value("finish").toDateTime().toString(Qt::ISODate) << "\n";
    }
    out.flush();
    file.close();
    QMessageBox::information(
        this, tr("Information"),
        tr("Exporting room occupancy to '%1' has finished.").arg(fileName));
}
}
}
