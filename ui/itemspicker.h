#ifndef ITEMSPICKER_H
#define ITEMSPICKER_H

#include <QCheckBox>
#include <QStringList>
#include <QWidget>

namespace paso {
namespace widget {

///
/// \brief The ItemsPicker class supports selecting multiple items via check
/// boxes.
/// \details The items are layed out in a grid, that has predefined number of
/// columns.
///
class ItemsPicker : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QList<int> selectedItems READ selectedItems WRITE
                   setSelectedItems NOTIFY selectedItemsChanged)
public:
    ///
    /// \brief Constructs new ItemsPicker with given parent.
    /// \param parent The parent widget.
    ///
    explicit ItemsPicker(QWidget *parent = nullptr);
    ///
    /// \brief Contructs new ItemsPicker with given column number and parent.
    /// \param columns The column number for laying out items.
    /// \param parent The parent widget.
    ///
    explicit ItemsPicker(int columns, QWidget *parent = nullptr);

    ///
    /// \brief Returns number of columns set for this widget.
    /// \return The number of columns.
    ///
    int columnCount() const;

    ///
    /// \brief Sets new number of columns. All childred will we layed out again
    /// if needed.
    /// \param columnCount The new number of columns.
    ///
    void setColumnCount(int columnCount);

    ///
    /// \brief Sets the new list of item labels. Items currently displayed will
    /// be replaced with new ones that bear given labels.
    /// \param itemLabels The new list of item labels.
    ///
    void setItemLabels(const QStringList &itemLabels);

    ///
    /// \brief Returns the list of currently selected items. Numbers in the list
    /// represent item positions in previously set item labels. This list is 1
    /// based not zero based!
    /// \return List of currently selected items.
    ///
    QList<int> selectedItems() const;

    ///
    /// \brief Sets currently selected items. Numbers in the list represent item
    /// positions in previously set item labels. This list is 1 based not zero
    /// based!
    /// \note If any item in the list is bigger than item count, it will be
    /// ignored.
    /// \param selectedItems The list of items to be selected.
    ///
    void setSelectedItems(const QList<int> &selectedItems);

signals:
    ///
    /// \brief This signal is emited whenever list of selected items change.
    ///
    void selectedItemsChanged();
public slots:

private:
    int mColumnCount; //!< The grid column count.
    QList<QCheckBox *>
        mCheckBoxes; //!< The list of checkboxes that represent items.
};
}
}

#endif // ITEMSPICKER_H
