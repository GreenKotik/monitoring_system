#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>

class QVBoxLayout;
class QPushButton;
class QLabel;
class QListWidget;
class QToolButton;

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    enum Level {
        LevelCountry,
        LevelObject,
        LevelSensor
    };

    explicit ToolbarWidget(QWidget *parent = nullptr);

    void setLevel(Level level);
    void setObject(const QJsonObject &object);
    void setSensor(const QJsonObject &sensor);
    void setObjects(const QJsonArray &objects);
    void setSensors(const QJsonArray &sensors);
    void updateCounters(int objectsCount, int sensorsCount);
    void showBackButton(bool show);

signals:
    void backClicked();
    void objectSelected(const QString &objectId);
    void sensorSelected(const QString &sensorId);
    void filterChanged(const QString &filter);
    void viewReset();
    void zoomIn();
    void zoomOut();

private:
    void setupUi();
    void clearContent();
    void updateObjectList();
    void updateSensorList();

    Level m_currentLevel = LevelCountry;
    QJsonObject m_currentObject;
    QJsonObject m_currentSensor;
    QJsonArray m_objects;
    QJsonArray m_sensors;

    QVBoxLayout *m_mainLayout;
    QWidget *m_headerWidget;
    QPushButton *m_backButton;
    QLabel *m_titleLabel;
    QWidget *m_contentWidget;
    QListWidget *m_objectList;
    QListWidget *m_sensorList;
    QLabel *m_statusLabel;
    QLabel *m_objectCountLabel;
    QLabel *m_sensorCountLabel;

    // Кнопки инструментов
    QToolButton *m_zoomInBtn;
    QToolButton *m_zoomOutBtn;
    QToolButton *m_resetBtn;
};

#endif // TOOLBARWIDGET_H