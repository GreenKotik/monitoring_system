#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QSplitter>
#include <QToolBar>
#include <QStatusBar>
#include "widgets/mapwidget.h"
#include "widgets/toolbarwidget.h"
#include "widgets/sensordetailwidget.h"
#include "widgets/breadcrumbwidget.h"
#include "apiclient.h"
#include "appstate.h"
#include "core/utils/logger.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLoginSuccess(const QString &token, const QString &username);
    void onLoginFailed(const QString &error);
    void onObjectSelected(const QString &objectId);
    void onSensorSelected(const QString &sensorId);
    void onBackClicked();
    void onObjectsLoaded(const QJsonArray &objects);
    void onSensorsLoaded(const QJsonArray &sensors);
    void onHistoryLoaded(const QJsonArray &history);
    void onApiError(const QString &error);
    void onViewReset();
    void onZoomIn();
    void onZoomOut();

private:
    void setupUi();
    void setupConnections();
    void loadInitialData();
    void updateBreadcrumb();
    void showLoginDialog();
    void setLevel(int level);
    void updateToolbar();

    QSplitter *m_splitter;
    QStackedWidget *m_stackedWidget;
    MapWidget *m_mapWidget;
    ToolbarWidget *m_toolbarWidget;
    SensorDetailWidget *m_sensorDetailWidget;
    BreadcrumbWidget *m_breadcrumbWidget;

    ApiClient *m_apiClient;
    AppState m_appState;

    QList<Object> m_objects;
    QList<Sensor> m_sensors;
};

#endif // MAINWINDOW_H