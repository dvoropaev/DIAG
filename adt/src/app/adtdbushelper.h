#ifndef ADT_DBUS_HELPER_H
#define ADT_DBUS_HELPER_H

#include <QDBusConnection>

#include <vars/adtvarsinterface.h>

class ADTDBusHelper
{
public:
    ADTDBusHelper();
    virtual ~ADTDBusHelper();

    QStringList getTests(const QString &serviceName,
                         const QString &path,
                         const QString &ifaceName,
                         const QString &listMethod,
                         bool isSystemBus);

    void applyVar(const QString &serviceName,
                  const QString &path,
                  const QString &ifaceName,
                  const QString &varName,
                  const QString &value,
                  bool isSystemBus,
                  bool isSet);

private:
    QStringList getTestsFromBus(const QString &serviceName,
                                const QString &path,
                                const QString &ifaceName,
                                const QString &listMethod,
                                const QDBusConnection &con);

    void applyVarOnBus(const QString &serviceName,
                       const QString &path,
                       const QString &ifaceName,
                       const QDBusConnection &con,
                       const QString &varName,
                       const QString &value,
                       bool isSet);

private:
    QDBusConnection m_sessionConn = QDBusConnection::sessionBus();
    QDBusConnection m_systemConn  = QDBusConnection::systemBus();
};

#endif //ADT_DBUS_HELPER_H
