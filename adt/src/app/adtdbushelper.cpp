#include "adtdbushelper.h"

#include <QDBusInterface>
#include <QDBusReply>

ADTDBusHelper::ADTDBusHelper() {}

ADTDBusHelper::~ADTDBusHelper() {}

QStringList ADTDBusHelper::getTests(const QString &serviceName,
                                    const QString &path,
                                    const QString &ifaceName,
                                    const QString &listMethod,
                                    bool isSystemBus)
{
    if (isSystemBus)
        return getTestsFromBus(serviceName, path, ifaceName, listMethod, m_systemConn);
    else
        return getTestsFromBus(serviceName, path, ifaceName, listMethod, m_sessionConn);
}

void ADTDBusHelper::applyVar(const QString &serviceName,
                             const QString &path,
                             const QString &ifaceName,
                             const QString &varName,
                             const QString &value,
                             bool isSystemBus,
                             bool isSet)
{
    if (isSystemBus)
        applyVarOnBus(serviceName, path, ifaceName, m_systemConn, varName, value, isSet);
    else
        applyVarOnBus(serviceName, path, ifaceName, m_sessionConn, varName, value, isSet);
}

void ADTDBusHelper::applyVarOnBus(const QString &serviceName,
                                  const QString &path,
                                  const QString &ifaceName,
                                  const QDBusConnection &con,
                                  const QString &varName,
                                  const QString &value,
                                  bool isSet)
{
    QDBusMessage reply;
    QDBusInterface iface = QDBusInterface(serviceName, path, ifaceName, con);

    if (!iface.isValid())
    {
        qWarning() << "ERROR: object path " << path << " reply is not valid. Can't apply var";
        return;
    }

    //UnsetEnvValue
    if (isSet)
        reply = iface.call("SetEnvValue", varName, value);
    else
        reply = iface.call("UnsetEnvValue", varName);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "ERROR: object path " << path << " is not valid. Reply type is error";
        return;
    }

    int exitCode = reply.arguments().at(0).toInt();
    if (exitCode)
    {
        qWarning() << "ERROR: object path " << path << ". Exit code is not zero";
        return;
    }
}

QStringList ADTDBusHelper::getTestsFromBus(const QString &serviceName,
                                           const QString &path,
                                           const QString &ifaceName,
                                           const QString &listMethod,
                                           const QDBusConnection &con)
{
    QDBusInterface iface = QDBusInterface(serviceName, path, ifaceName, con);

    if (!iface.isValid())
    {
        qWarning() << "ERROR: object interface " << path << " is not valid. Can't get tool tests";
        return QStringList();
    }

    QDBusReply<QStringList> testReply = iface.call(listMethod);
    if (!testReply.isValid())
    {
        qWarning() << "ERROR: Reply of " << path << " is not valid, can't get list of tests";
        return QStringList();
    }

    QStringList tests = testReply.value();
    if (tests.empty())
    {
        qWarning() << "WARNING: No tests in " << path << ". Skipping tool.";
        return QStringList();
    }

    for (QString &testName : tests)
        testName = testName.trimmed();

    return tests;
}
