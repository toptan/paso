#include "studentdetailsdialog.h"
#include "ui_studentdetailsdialog.h"

#include "data.h"
#include "entitytablemodel.h"
#include "pasodb.h"
#include "stablerownumbersortfilterproxymodel.h"

#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;

namespace paso {
namespace admin {

struct StudentDetailsDialog::StudentDetailsDialogPrivate {
    explicit StudentDetailsDialogPrivate(const Student &student)
        : student(student), manager(DEFAULT_DB_NAME) {}

    const Student student;

    DBManager manager;
};

StudentDetailsDialog::StudentDetailsDialog(const data::entity::Student &student,
                                           QWidget *parent)
    : QDialog(parent), ui(new Ui::StudentDetailsDialog),
      mPrivate(new StudentDetailsDialogPrivate(student)) {
    ui->setupUi(this);
    ui->indexNumber->setText(student.indexNumber());
    ui->firstName->setText(student.firstName());
    ui->lastName->setText(student.lastName());
    ui->email->setText(student.email());
    ui->yearOfStudy->setText(QString("%1").arg(student.yearOfStudy()));
    ui->rfid->setText(student.rfid());

    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
    connect(ui->buttonBox->button(QDialogButtonBox::Close),
            &QPushButton::clicked, this, &StudentDetailsDialog::reject);
    auto refreshButton = new QPushButton(tr("Refresh"), ui->buttonBox);
    refreshButton->setObjectName("REFRESH_BUTTON");
    ui->buttonBox->addButton(refreshButton, QDialogButtonBox::ResetRole);
    connect(refreshButton, &QPushButton::clicked, this,
            &StudentDetailsDialog::loadData);

    QMap<QString, QString> courseColumns{{"CODE", tr("Code")},
                                         {"NAME", tr("Course")}};
    QMap<QString, QString> listColumns{{"NAME", tr("List Name")}};

    mCoursesModel = new EntityTableModel({"CODE", "NAME"}, courseColumns,
                                         EntityVector(), this);
    mCoursesProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mCoursesProxyModel->setSourceModel(mCoursesModel);
    mCoursesProxyModel->setSortLocaleAware(true);
    mCoursesProxyModel->setDynamicSortFilter(false);

    mListsModel =
        new EntityTableModel({"NAME"}, listColumns, EntityVector(), this);
    mListsProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mListsProxyModel->setSourceModel(mListsModel);
    mListsProxyModel->setSortLocaleAware(true);
    mListsProxyModel->setDynamicSortFilter(false);

    ui->coursesTableView->setModel(mCoursesProxyModel);
    ui->coursesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->coursesTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    ui->coursesTableView->horizontalHeader()->setCascadingSectionResizes(true);
    ui->coursesTableView->horizontalHeader()->setStretchLastSection(true);
    ui->coursesTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
    ui->listsTableView->setModel(mListsProxyModel);
    ui->listsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listsTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    ui->listsTableView->horizontalHeader()->setCascadingSectionResizes(true);
    ui->listsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->listsTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    loadData();
}

StudentDetailsDialog::~StudentDetailsDialog() { delete ui; }

void StudentDetailsDialog::loadData() {
    QSqlError cErr;
    QSqlError lErr;
    auto courses = mPrivate->manager.getStudentCourses(
        mPrivate->student.indexNumber(), cErr);
    auto lists =
        mPrivate->manager.studentLists(mPrivate->student.indexNumber(), lErr);

    if (cErr.type() != QSqlError::NoError ||
        lErr.type() != QSqlError::NoError) {
        auto t = lErr.type() != QSqlError::NoError ? lErr.text() : cErr.text();
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(t);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

    mCoursesModel->setEntityData(courses);
    mListsModel->setEntityData(lists);
}
}
}
