#include "widgets/breadcrumbwidget.h"
#include <QVariant>  // ДОБАВЛЕНО: обязательно для работы с QVariant
#include <QStyle>

BreadcrumbWidget::BreadcrumbWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QHBoxLayout(this))
    , m_lastButton(nullptr)
{
    m_layout->setContentsMargins(10, 5, 10, 5);
    m_layout->setSpacing(5);
    setStyleSheet("background-color: #f0f0f0; border-bottom: 1px solid #ccc;");
}

void BreadcrumbWidget::addItem(const QString &name, const QString &id) {
    // Добавляем разделитель перед новым элементом
    if (!m_items.isEmpty()) {
        QLabel *sep = new QLabel("›", this);
        sep->setStyleSheet("color: #666; font-size: 16px;");
        m_layout->addWidget(sep);
    }

    // Создаем кнопку-ссылку
    QPushButton *btn = new QPushButton(name, this);
    btn->setFlat(true);
    btn->setStyleSheet("QPushButton { color: #0066cc; text-decoration: underline; } "
                       "QPushButton:hover { color: #004499; }");
    // Теперь QVariant полностью определен
    btn->setProperty("itemId", QVariant(id));
    connect(btn, &QPushButton::clicked, this, &BreadcrumbWidget::onItemClicked);

    m_layout->addWidget(btn);
    m_lastButton = btn;

    // Сохраняем информацию
    BreadcrumbItem item;
    item.name = name;
    item.id = id;
    item.label = nullptr;
    item.button = btn;
    m_items.append(item);
}

void BreadcrumbWidget::popItem() {
    if (m_items.isEmpty()) return;

    // Удаляем последний элемент
    BreadcrumbItem &last = m_items.last();
    if (last.label) {
        m_layout->removeWidget(last.label);
        delete last.label;
    }
    if (last.button) {
        m_layout->removeWidget(last.button);
        delete last.button;
    }
    m_items.removeLast();
    m_lastButton = m_items.isEmpty() ? nullptr : m_items.last().button;

    // Удаляем разделитель перед ним
    if (!m_items.isEmpty()) {
        QLayoutItem *item = m_layout->takeAt(m_layout->count() - 1);
        if (item && item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void BreadcrumbWidget::clear() {
    while (!m_items.isEmpty()) {
        popItem();
    }
}

QString BreadcrumbWidget::lastId() const {
    return m_items.isEmpty() ? QString() : m_items.last().id;
}

void BreadcrumbWidget::onItemClicked() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        QString id = btn->property("itemId").toString();
        emit itemClicked(id);
    }
}
