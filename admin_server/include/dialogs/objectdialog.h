#ifndef OBJECTDIALOG_H
#define OBJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
// Исправленный путь - теперь указываем относительно include папки core
#include "models/object.h"  // Вместо "core/models/object.h"

class ObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectDialog(QWidget *parent = nullptr);

    void setObject(const Object &object);
    Object getObject() const;

private:
    void setupUi();
    void loadObjectTypes();

    QLineEdit *m_idEdit;
    QLineEdit *m_nameEdit;
    QLineEdit *m_descriptionEdit;
    QComboBox *m_typeCombo;
    QLineEdit *m_parentIdEdit;
    QDoubleSpinBox *m_positionX;
    QDoubleSpinBox *m_positionY;
    QDoubleSpinBox *m_sizeWidth;
    QDoubleSpinBox *m_sizeHeight;
    QLineEdit *m_svgPathEdit;
    QComboBox *m_statusCombo;

    Object m_object;
};

#endif // OBJECTDIALOG_H
