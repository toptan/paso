#ifndef MOCKACTIVITYWIZARDPAGE_H
#define MOCKACTIVITYWIZARDPAGE_H

#include <QWizardPage>

class MockActivityWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    MockActivityWizardPage(QWidget *parent=nullptr);
};

#endif // MOCKACTIVITYWIZARDPAGE_H
