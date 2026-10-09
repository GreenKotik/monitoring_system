#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QList>

class BreadcrumbWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BreadcrumbWidget(QWidget *parent = nullptr);

    void addItem(const QString &name, const QString &id = QString());
    void popItem();
    void clear();
    int count() const { return m_items.size(); }
    QString lastId() const;

signals:
    void itemClicked(const QString &id);

private slots:
    void onItemClicked();

private:
    struct BreadcrumbItem {
        QString name;
        QString id;
        QLabel *label;
        QPushButton *button;
    };

    QHBoxLayout *m_layout;
    QList<BreadcrumbItem> m_items;
    QPushButton *m_lastButton;
};
