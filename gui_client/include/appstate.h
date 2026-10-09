#pragma once

#include <QObject>
#include <QJsonObject>
#include "models/object.h"
#include "models/sensor.h"

class AppState : public QObject
{
    Q_OBJECT
private:
    explicit AppState(QObject *parent = nullptr);
    ~AppState();

public:
    static AppState& instance();

    // Текущий выбранный объект
    Object currentObject() const;
    void setCurrentObject(const Object &object);

    // Текущий сенсор
    Sensor currentSensor() const;
    void setCurrentSensor(const Sensor &sensor);

    // Состояние приложения
    bool isConnected() const;
    void setConnected(bool connected);

    // Токен авторизации
    QString authToken() const;
    void setAuthToken(const QString &token);

signals:
    void currentObjectChanged(const Object &object);
    void currentSensorChanged(const Sensor &sensor);
    void connectionStateChanged(bool connected);

private:
    Object m_currentObject;
    Sensor m_currentSensor;
    bool m_isConnected = false;
    QString m_authToken;
};