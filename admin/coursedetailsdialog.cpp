#include "coursedetailsdialog.h"
#include "ui_coursedetailsdialog.h"

#include <QMessageBox>
#include <QSqlError>
#include <memory>

using namespace std;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;

namespace paso {
namespace admin {

struct CourseDetailsDialog::CourseDetailsDialogPrivate {
    explicit CourseDetailsDialogPrivate(const Course &course)
        : ENLISTED_COUNT_FORMAT(tr("Enlisted students: %1")), course(course),
          enlistedStudentsModel(nullptr), notEnlistedStudentsModel(nullptr),
          enlistedProxyModel(nullptr), notEnlistedProxyModel(nullptr),
          manager(DEFAULT_DB_NAME), dirty(false) {
        addedStudents = make_shared<vector<shared_ptr<Entity>>>();
        removedStudents = make_shared<vector<shared_ptr<Entity>>>();
    }

    const QString ENLISTED_COUNT_FORMAT;
    const Course course;

    EntityTableModel *enlistedStudentsModel;
    EntityTableModel *notEnlistedStudentsModel;
    StableRowNumberSortFilterProxyModel *enlistedProxyModel;
    StableRowNumberSortFilterProxyModel *notEnlistedProxyModel;
    EntityVector addedStudents;
    EntityVector removedStudents;
    DBManager manager;

    bool dirty;
};

CourseDetailsDialog::CourseDetailsDialog(const Course &course, QWidget *parent)
    : QDialog(parent), ui(new Ui::CourseDetailsDialog),
      mPrivate(new CourseDetailsDialogPrivate(course)) {
    ui->setupUi(this);
    ui->codeLabel->setText(mPrivate->course.code());
    ui->nameLabel->setText(mPrivate->course.name());
    mPrivate->enlistedProxyModel =
        new StableRowNumberSortFilterProxyModel(this);
    mPrivate->notEnlistedProxyModel =
        new StableRowNumberSortFilterProxyModel(this);

    ui->enlistedTableView->setModel(mPrivate->enlistedProxyModel);
    ui->enlistedTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->enlistedTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->enlistedTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    ui->notEnlistedTableView->setModel(mPrivate->notEnlistedProxyModel);
    ui->notEnlistedTableView->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    ui->notEnlistedTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->notEnlistedTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

CourseDetailsDialog::~CourseDetailsDialog() { delete ui; }

int CourseDetailsDialog::exec() {
    QSqlError error;
    auto enlistedStudents = mPrivate->manager.studentsEnlistedToCourse(
        mPrivate->course.code(), error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox(parentWidget());
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return QDialog::Rejected;
    }
    auto notEnlistedStudents = mPrivate->manager.studentsNotEnlistedToCourse(
        mPrivate->course.code(), error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox(parentWidget());
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return QDialog::Rejected;
    }
    QStringList columns{"INDEX_NUMBER", "LAST_NAME", "FIRST_NAME"};
    QMap<QString, QString> columnNames{{"INDEX_NUMBER", tr("Index Number")},
                                       {"LAST_NAME", tr("Last Name")},
                                       {"FIRST_NAME", tr("First Name")}};

    mPrivate->enlistedStudentsModel =
        new EntityTableModel(columns, columnNames, enlistedStudents, this);
    mPrivate->notEnlistedStudentsModel =
        new EntityTableModel(columns, columnNames, notEnlistedStudents, this);
    ui->totalStudentsLabel->setText(mPrivate->ENLISTED_COUNT_FORMAT.arg(
        mPrivate->enlistedStudentsModel->rowCount()));
    mPrivate->enlistedProxyModel->setSourceModel(
        mPrivate->enlistedStudentsModel);
    mPrivate->notEnlistedProxyModel->setSourceModel(
        mPrivate->notEnlistedStudentsModel);
    return QDialog::exec();
}

void CourseDetailsDialog::enlistedCountChanged(size_t count) {
    ui->totalStudentsLabel->setText(mPrivate->ENLISTED_COUNT_FORMAT.arg(count));
}
}
}
