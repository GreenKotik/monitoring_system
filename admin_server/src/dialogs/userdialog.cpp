#include "dialogs/userdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>

UserDialog::UserDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void UserDialog::setupUi()
{
    setWindowTitle("Пользователь");
    setMinimumWidth(350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    m_usernameEdit = new QLineEdit(this);
    formLayout->addRow("Имя пользователя:", m_usernameEdit);

    m_emailEdit = new QLineEdit(this);
    formLayout->addRow("Email:", m_emailEdit);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Пароль:", m_passwordEdit);

    m_roleCombo = new QComboBox(this);
    m_roleCombo->addItems({"user", "admin", "viewer"});
    formLayout->addRow("Роль:", m_roleCombo);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("OK", this);
    QPushButton *cancelBtn = new QPushButton("Отмена", this);

    okBtn->setProperty("acceptButton", true);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okBtn);
    buttonLayout->addWidget(cancelBtn);

    mainLayout->addLayout(buttonLayout);
}

void UserDialog::setUser(const User &user)
{
    m_user = user;

    m_usernameEdit->setText(user.username());
    m_emailEdit->setText(user.email());

    int index = m_roleCombo->findText(user.role());
    if (index >= 0) {
        m_roleCombo->setCurrentIndex(index);
    }

    // Пароль не заполняем при редактировании
    m_passwordEdit->clear();
}

User UserDialog::getUser() const
{
    User user;

    user.setUsername(m_usernameEdit->text().trimmed());
    user.setEmail(m_emailEdit->text().trimmed());

    if (!m_passwordEdit->text().isEmpty()) {
        user.setPasswordHash(m_passwordEdit->text().trimmed());
    }

    user.setRole(m_roleCombo->currentText());

    return user;
}