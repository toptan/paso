#include "itemspicker.h"

#include <QCheckBox>
#include <QGridLayout>

namespace paso {
namespace widget {

ItemsPicker::ItemsPicker(QWidget *parent) : QWidget(parent), mColumnCount(0) {
    auto l = new QGridLayout;
    auto margins = l->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    l->setContentsMargins(margins);
    setLayout(l);
}

ItemsPicker::ItemsPicker(int columns, QWidget *parent)
    : QWidget(parent), mColumnCount(columns) {
    auto l = new QGridLayout;
    auto margins = l->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    l->setContentsMargins(margins);
    setLayout(l);
}

QList<int> ItemsPicker::selectedItems() const {
    QList<int> retVal;
    for (auto i = 0; i < mCheckBoxes.size(); i++) {
        if (mCheckBoxes[i]->isChecked()) {
            retVal.push_back(i);
        }
    }
    return retVal;
}

int ItemsPicker::columnCount() const { return mColumnCount; }

void ItemsPicker::setColumnCount(int columnCount) {
    if (mColumnCount == columnCount) {
        return;
    }
    mColumnCount = columnCount;
    for (const auto &widget : mCheckBoxes) {
        layout()->removeWidget(widget);
    }
    int current = 0;
    for (const auto &widget : mCheckBoxes) {
        dynamic_cast<QGridLayout *>(layout())->addWidget(
            widget, current / mColumnCount, current % mColumnCount);
        current++;
    }
}

void ItemsPicker::setItemLabels(const QStringList &itemLabels) {
    for (auto &widget : mCheckBoxes) {
        layout()->removeWidget(widget);
        delete widget;
    }
    mCheckBoxes.clear();
    int current = 0;
    for (const auto &label : itemLabels) {
        auto checkBox = new QCheckBox(this);
        checkBox->setObjectName(
            QString("item%1").arg(current, 3, 10, QLatin1Char('0')));
        checkBox->setText(label);
        checkBox->setChecked(false);
        connect(checkBox, &QCheckBox::toggled, this,
                &ItemsPicker::selectedItemsChanged);
        dynamic_cast<QGridLayout *>(layout())->addWidget(
            checkBox, current / mColumnCount, current % mColumnCount);
        mCheckBoxes.push_back(checkBox);
        current++;
    }
    emit selectedItemsChanged();
}

void ItemsPicker::setSelectedItems(const QList<int> &selectedItems) {
    for (auto i = 0; i < mCheckBoxes.size(); i++) {
        mCheckBoxes[i]->setChecked(selectedItems.contains(i));
    }
    emit selectedItemsChanged();
}
}
}
