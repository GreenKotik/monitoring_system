#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "widgets/mapwidget.h"
#include "widgets/toolbarwidget.h"
#include "widgets/breadcrumbwidget.h"
#include "widgets/objectdetailwidget.h"
#include "widgets/sensordetailwidget.h"
#include "apiclient.h"
#include "appstate.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onObjectSelected(const QString &objectId);
    void onSensorSelected(const QString &sensorId);
    void onBackClicked();
    void onSearchTextChanged(const QString &text);
    void onRefreshClicked();
    // ДОБАВЛЕНО: объявление слота
    void onObjectsLoaded(const QList<Object> &objects);

private:
    void setupUI();
    void loadObjects();
    void showLevel(int level);

    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;

    BreadcrumbWidget *m_breadcrumb;
    ToolbarWidget *m_toolbar;
    QStackedWidget *m_contentStack;
    MapWidget *m_mapWidget;
    ObjectDetailWidget *m_objectDetailWidget;
    SensorDetailWidget *m_sensorDetailWidget;

    ApiClient *m_apiClient;
    AppState *m_appState;

    QList<Object> m_objects;
    QList<Sensor> m_sensors;
};
