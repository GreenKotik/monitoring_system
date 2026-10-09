#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarWidget(QWidget *parent = nullptr);

    void setLevel(int level); // 0 - карта, 1 - объект, 2 - датчик

signals:
    void searchTextChanged(const QString &text);
    void refreshClicked();
    void zoomIn();
    void zoomOut();
    void resetView();
    void filterAll();
    void filterComplex();
    void filterBuilding();
    void toggleLabels();

private slots:
    void onSearchTextChanged(const QString &text);
    void onRefreshClicked();
    void onZoomIn();
    void onZoomOut();
    void onResetView();
    void onFilterAll();
    void onFilterComplex();
    void onFilterBuilding();
    void onToggleLabels();

private:
    // ДОБАВЛЕНО: объявление метода createButton
    QPushButton* createButton(const QString &text, const QString &tooltip);

    QLineEdit *m_searchEdit;
    QComboBox *m_filterCombo;
    QPushButton *m_refreshBtn;
    QPushButton *m_zoomInBtn;
    QPushButton *m_zoomOutBtn;
    QPushButton *m_resetBtn;
    QPushButton *m_allBtn;
    QPushButton *m_complexBtn;
    QPushButton *m_buildingBtn;
    QPushButton *m_labelsBtn;

    QVBoxLayout *m_layout;
};
