#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "core/models/user.h"

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr);

    void setUser(const User &user);
    User getUser() const;

private:
    void setupUi();

    QLineEdit *m_usernameEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_passwordEdit;
    QComboBox *m_roleCombo;

    User m_user;
};

#endif // USERDIALOG_H