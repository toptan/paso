#ifndef LISTDETAILSDIALOG_H
#define LISTDETAILSDIALOG_H

#include "list.h"
#include <memory>

#include <QDialog>

namespace Ui {
class ListDetailsDialog;
}

class QAbstractButton;

namespace paso {
namespace admin {

class ListDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ListDetailsDialog(const paso::data::entity::List &list,
                               QWidget *parent = 0);
    virtual ~ListDetailsDialog();

public slots:
    virtual void reject() override;
    void onImportFileSelected(const QString &fileName);

signals:
    void newLogLine(const QString &line);
    void importDone();

private slots:
    virtual void onButtonBoxButtonClicked(QAbstractButton *button);

private:
    Ui::ListDetailsDialog *ui;

    struct ListDetailsDialogPrivate;
    std::unique_ptr<ListDetailsDialogPrivate> mPrivate;

    void refresh();
    void loadData();
    void saveData();
    void importListStudents();
};
}
}

#endif // LISTDETAILSDIALOG_H
