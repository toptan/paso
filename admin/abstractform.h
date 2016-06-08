#ifndef ABSTRACTFORM_H
#define ABSTRACTFORM_H

#include <QAction>
#include <QWidget>

namespace paso {
namespace admin {

///
/// \brief The AbstractForm class is a base abstract class for all forms that
/// have toolbar actions.
///
class AbstractForm : public QWidget {
public:
    explicit AbstractForm(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~AbstractForm() {}

    ///
    /// \brief toolBarActions returns reference to the list with actions for the
    /// toolbar.
    /// \return reference to the list with actions.
    ///
    virtual const QList<QAction *> &toolBarActions() const = 0;
};
}
}

#endif // ABSTRACTFORM_H
