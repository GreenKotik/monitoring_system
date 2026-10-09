#include "widgets/toolbarwidget.h"

ToolbarWidget::ToolbarWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
{
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setSpacing(10);
    setStyleSheet("background-color: #f8f9fa; border-right: 1px solid #dee2e6;");
    setFixedWidth(260);

    // Заголовок
    QLabel *title = new QLabel("Карта страны", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    m_layout->addWidget(title);

    // Поиск
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("🔍 Поиск объектов...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ToolbarWidget::onSearchTextChanged);
    m_layout->addWidget(m_searchEdit);

    // Кнопки управления
    QHBoxLayout *btnLayout1 = new QHBoxLayout();
    m_refreshBtn = createButton("🔄", "Обновить");
    m_zoomInBtn = createButton("➕", "Приблизить");
    m_zoomOutBtn = createButton("➖", "Отдалить");
    m_resetBtn = createButton("↺", "Сбросить");
    btnLayout1->addWidget(m_refreshBtn);
    btnLayout1->addWidget(m_zoomInBtn);
    btnLayout1->addWidget(m_zoomOutBtn);
    btnLayout1->addWidget(m_resetBtn);
    m_layout->addLayout(btnLayout1);

    // Фильтры
    QHBoxLayout *btnLayout2 = new QHBoxLayout();
    m_allBtn = createButton("Все", "Все объекты");
    m_complexBtn = createButton("Комп.", "Комплексы");
    m_buildingBtn = createButton("Здан.", "Здания");
    btnLayout2->addWidget(m_allBtn);
    btnLayout2->addWidget(m_complexBtn);
    btnLayout2->addWidget(m_buildingBtn);
    m_layout->addLayout(btnLayout2);

    // Показ названий
    m_labelsBtn = createButton("🏷️", "Показать названия");
    m_layout->addWidget(m_labelsBtn);

    // Информация
    QGroupBox *infoGroup = new QGroupBox("Информация", this);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    QLabel *info = new QLabel("Объектов: 0", this);
    info->setObjectName("objectCount");
    infoLayout->addWidget(info);
    m_layout->addWidget(infoGroup);

    // Растяжка
    m_layout->addStretch();

    // Подключаем сигналы
    connect(m_refreshBtn, &QPushButton::clicked, this, &ToolbarWidget::onRefreshClicked);
    connect(m_zoomInBtn, &QPushButton::clicked, this, &ToolbarWidget::onZoomIn);
    connect(m_zoomOutBtn, &QPushButton::clicked, this, &ToolbarWidget::onZoomOut);
    connect(m_resetBtn, &QPushButton::clicked, this, &ToolbarWidget::onResetView);
    connect(m_allBtn, &QPushButton::clicked, this, &ToolbarWidget::onFilterAll);
    connect(m_complexBtn, &QPushButton::clicked, this, &ToolbarWidget::onFilterComplex);
    connect(m_buildingBtn, &QPushButton::clicked, this, &ToolbarWidget::onFilterBuilding);
    connect(m_labelsBtn, &QPushButton::clicked, this, &ToolbarWidget::onToggleLabels);
}

// ДОБАВЛЕНО: реализация createButton
QPushButton* ToolbarWidget::createButton(const QString &text, const QString &tooltip) {
    QPushButton *btn = new QPushButton(text, this);
    btn->setToolTip(tooltip);
    btn->setStyleSheet("QPushButton { background: white; border: 1px solid #ccc; padding: 5px; } "
                       "QPushButton:hover { background: #e9ecef; }");
    return btn;
}

void ToolbarWidget::setLevel(int level) {
    QLabel *title = findChild<QLabel*>();
    if (title) {
        switch(level) {
            case 0: title->setText("Карта страны"); break;
            case 1: title->setText("Объект"); break;
            case 2: title->setText("Датчик"); break;
        }
    }
}

void ToolbarWidget::onSearchTextChanged(const QString &text) {
    emit searchTextChanged(text);
}

void ToolbarWidget::onRefreshClicked() { emit refreshClicked(); }
void ToolbarWidget::onZoomIn() { emit zoomIn(); }
void ToolbarWidget::onZoomOut() { emit zoomOut(); }
void ToolbarWidget::onResetView() { emit resetView(); }
void ToolbarWidget::onFilterAll() { emit filterAll(); }
void ToolbarWidget::onFilterComplex() { emit filterComplex(); }
void ToolbarWidget::onFilterBuilding() { emit filterBuilding(); }
void ToolbarWidget::onToggleLabels() { emit toggleLabels(); }
