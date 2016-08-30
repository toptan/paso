#include "activitywizardroomsselectionpage.h"
#include "ui_activitywizardroomsselectionpage.h"

ActivityWizardRoomsSelectionPage::ActivityWizardRoomsSelectionPage(
    QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardRoomsSelectionPage) {
    ui->setupUi(this);
}

ActivityWizardRoomsSelectionPage::~ActivityWizardRoomsSelectionPage() {
    delete ui;
}
