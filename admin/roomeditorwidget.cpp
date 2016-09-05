#include "roomeditorwidget.h"

#include "addremoveentitiesform.h"
#include "data.h"
#include "entitytablemodel.h"
#include "pasodb.h"
#include "room.h"
#include "student.h"

#include <QCheckBox>
#include <QDebug>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QPushButton>
#include <QSqlError>
#include <QSqlField>
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
    connect(mChangeBarringButton, &QPushButton::clicked, this,
            &RoomEditorWidget::onChangeBarringButtonClicked);

    auto buttonBox = findChild<QDialogButtonBox *>();
    buttonBox->addButton(mChangeBarringButton, QDialogButtonBox::ActionRole);

    const QStringList columns{"INDEX_NUMBER", "LAST_NAME", "FIRST_NAME"};
    const QMap<QString, QString> columnNames{
        {"INDEX_NUMBER", tr("Index number")},
        {"LAST_NAME", tr("Last name")},
        {"FIRST_NAME", tr("First name")}};
    EntityVector emptyData;
    mBarredStudentsTableView = new QTableView(this);
    mBarredStudentsModel =
        new EntityTableModel(columns, columnNames, emptyData, this);
    mBarredStudentsTableView->setModel(mBarredStudentsModel);
    mBarredStudentsTableView->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    mBarredStudentsTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    mBarredStudentsTableView->horizontalHeader()->setCascadingSectionResizes(
        true);
    mBarredStudentsTableView->horizontalHeader()->setStretchLastSection(true);
    mBarredStudentsTableView->sortByColumn(1, Qt::SortOrder::AscendingOrder);
    l->insertRow(l->rowCount() - 1, tr("Barred students"),
                 mBarredStudentsTableView);
}

void RoomEditorWidget::onDisplayRecord(const QSqlRecord &record) {
    RecordEditorWidget::onDisplayRecord(record);
    QSqlError error;
    auto roomId = record.value("id").toULongLong();
    auto barred = mManager.barredStudents(roomId, error);
    mBarredStudentsModel->setEntityData(barred);
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
        QVariant value;
        switch (fieldType) {
        case FieldType::ComboBox:
            value = dynamic_cast<QComboBox *>(field)->currentData().toString();
            break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
        case FieldType::MaskedLineEdit:
            value = dynamic_cast<QLineEdit *>(field)->text().trimmed();
            break;
        case FieldType::NumberEdit:
            value = dynamic_cast<QSpinBox *>(field)->text().trimmed();
            break;
        case FieldType::CheckBox:
            value = dynamic_cast<QCheckBox *>(field)->isChecked();
            break;
        case FieldType::DateEdit:
            value = dynamic_cast<QDateEdit *>(field)->date();
            break;
        case FieldType::TimeEdit:
            value = dynamic_cast<QTimeEdit *>(field)->time();
            break;
        }
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
}
}
}
