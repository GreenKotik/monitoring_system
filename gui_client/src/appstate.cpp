#include "appstate.h"

AppState::AppState(QObject *parent) : QObject(parent) {}
AppState::~AppState() {}

AppState& AppState::instance() {
    static AppState instance;
    return instance;
}

Object AppState::currentObject() const {
    return m_currentObject;
}

void AppState::setCurrentObject(const Object &object) {
    if (m_currentObject.id() != object.id()) {
        m_currentObject = object;
        emit currentObjectChanged(object);
    }
}

Sensor AppState::currentSensor() const {
    return m_currentSensor;
}

void AppState::setCurrentSensor(const Sensor &sensor) {
    if (m_currentSensor.id() != sensor.id()) {
        m_currentSensor = sensor;
        emit currentSensorChanged(sensor);
    }
}

bool AppState::isConnected() const {
    return m_isConnected;
}

void AppState::setConnected(bool connected) {
    if (m_isConnected != connected) {
        m_isConnected = connected;
        emit connectionStateChanged(connected);
    }
}

QString AppState::authToken() const {
    return m_authToken;
}

void AppState::setAuthToken(const QString &token) {
    m_authToken = token;
}