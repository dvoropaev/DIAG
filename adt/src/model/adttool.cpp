#include "adttool.h"
#include "builder/adttoolvarsmodelbuilder.h"

#include <QDBusInterface>
#include <QDBusReply>

ADTTool::ADTTool()
    : m_id()
    , m_bus(BusType::None)
    , m_type()
    , m_category()
    , m_icon()
    , m_displayName()
    , m_comment()
    , m_reportSuffix()
    , m_reportSuffixUser()
    , m_dbusPath()
    , m_dbusIface()
    , m_dbusServiceName()
    , m_dbusRunMethodName()
    , m_dbusReportMethodName()
    , m_dbusInfoMethodName()
    , m_displayNameLocaleStorage{}
    , m_commentLocaleStorage{}
    , m_tests{}
    , m_filteredTestsByMode{}
    , m_filter{}
    , m_vars{}
    , m_dbusHelper{new ADTDBusHelper()}
{}

void ADTTool::setLocale(QString &locale)
{
    auto displayNameIt = m_displayNameLocaleStorage.find(locale);
    if (displayNameIt != m_displayNameLocaleStorage.end())
    {
        m_displayName = *displayNameIt;
    }
    else
    {
        m_displayName = m_displayNameLocaleStorage["en"];
    }

    auto commentIt = m_commentLocaleStorage.find(locale);
    if (commentIt != m_commentLocaleStorage.end())
    {
        m_comment = *commentIt;
    }

    std::for_each(m_tests.begin(), m_tests.end(), [&locale](std::unique_ptr<ADTTest> &test) {
        test->setLocale(locale);
    });

    std::for_each(m_vars.begin(), m_vars.end(), [&locale](std::unique_ptr<ADTVarInterface> &var) {
        var->translate(locale);
    });
}

void ADTTool::setFilter(QString filter)
{
    m_filter = filter;
}

QString ADTTool::getFilter()
{
    return m_filter;
}

int ADTTool::getReport(QDBusConnection conn, QByteArray &result)
{
    QDBusInterface iface(m_dbusServiceName, m_dbusPath, m_dbusIface, conn);

    if (!iface.isValid())
    {
        return 1;
    }

    QDBusReply<QByteArray> reply = iface.call(m_dbusReportMethodName);

    if (!reply.isValid())
    {
        return 1;
    }

    result = reply.value();

    return 0;
}

std::unique_ptr<QStandardItemModel> ADTTool::getVarsModel()
{
    ADTToolVarsModelBuilder builder;

    return builder.buildVarsModel(m_vars, this);
}

void ADTTool::setVars()
{
    std::for_each(m_vars.begin(), m_vars.end(), [this](std::unique_ptr<ADTVarInterface> &var) {
        applyVar(var->id(), var.get(), true, true);
        applyVar(var->id(), var.get(), false, true);
    });
}

void ADTTool::unsetVars()
{
    std::for_each(m_vars.begin(), m_vars.end(), [this](std::unique_ptr<ADTVarInterface> &var) {
        applyVar(var->id(), var.get(), true, false);
        applyVar(var->id(), var.get(), false, false);
    });
}

void ADTTool::applyVar(const QString &varName, ADTVarInterface *var, bool isSystemBus, bool isSet)
{
    auto varType = var->getType();
    QString value;
    int val;
    switch (varType)
    {
    case ADTVarInterface::ADTVarType::INT:
        if (!var->get(&val))
            return;
        value = QString::number(val);

        break;

    case ADTVarInterface::ADTVarType::STRING:
        if (!var->get(&value))
            return;

        break;

    case ADTVarInterface::ADTVarType::ENUM_STRING:
        if (!var->get(&value))
            return;

        break;

    case ADTVarInterface::ADTVarType::ENUM_INT:
        if (!var->get(&val))
            return;

        value = QString::number(val);

        break;

    case ADTVarInterface::ADTVarType::MODE:
        if (!var->get(&value))
            return;

        break;

    default: //unsupported type
        qWarning() << "ADTTool can't apply var" << var->getDisplayName() << " in tool: " << m_id
                   << " - unsupported type";
        return;
    }

    m_dbusHelper->applyVar(m_dbusServiceName,
                           "/org/altlinux/alterator",
                           "org.altlinux.alterator.manager",
                           varName,
                           value,
                           isSystemBus,
                           isSet);
}

ADTTool::BusType ADTTool::bus() const
{
    return m_bus;
}

QString ADTTool::id()
{
    return m_id;
}

QString ADTTool::type() const
{
    return m_type;
}

QString ADTTool::category() const
{
    return m_category;
}

QString ADTTool::icon() const
{
    return m_icon;
}

QString ADTTool::displayName() const
{
    return m_displayName;
}

QString ADTTool::comment() const
{
    return m_comment;
}

QString ADTTool::reportSuffix(ADTTool::BusType bus) const
{
    switch (bus)
    {
    case ADTTool::System:
        return m_reportSuffix;
    case ADTTool::Session:
        return m_reportSuffixUser;
    default:
        qCritical() << "failed to get report suffix: invalid BusType";
        return {};
    }
}

QString ADTTool::dbusPath() const
{
    return m_dbusPath;
}

QString ADTTool::dbusIface() const
{
    return m_dbusIface;
}

QString ADTTool::dbusServiceName() const
{
    return m_dbusServiceName;
}

QString ADTTool::runMethodName() const
{
    return m_dbusRunMethodName;
}

QString ADTTool::reportMethodName() const
{
    return m_dbusReportMethodName;
}

QString ADTTool::infoMethodName() const
{
    return m_dbusInfoMethodName;
}

void ADTTool::changeMode()
{
    QStringList currentSysTests;
    QStringList currentSesTests;
    QStringList sysTestWithSuffix;
    QStringList sesTestWithSuffix;

    auto setName = [](const QStringList &list, const QString suffix, QStringList &result) {
        std::for_each(list.cbegin(), list.cend(), [&list, &suffix, &result](const QString &value) {
            result << value + suffix;
        });
    };

    setVars();

    if (m_bus == ADTTool::All || m_bus == ADTTool::System)
        currentSysTests = m_dbusHelper->getTests(m_dbusServiceName, m_dbusPath, m_dbusIface, "List", true);

    if (m_bus == ADTTool::All || m_bus == ADTTool::Session)
        currentSesTests = m_dbusHelper->getTests(m_dbusServiceName, m_dbusPath, m_dbusIface, "List", false);

    unsetVars();

    setName(currentSysTests, "system", sysTestWithSuffix);
    setName(currentSesTests, "session", sesTestWithSuffix);

    std::sort(sysTestWithSuffix.begin(), sysTestWithSuffix.end());
    std::sort(sesTestWithSuffix.begin(), sesTestWithSuffix.end());

    m_filteredTestsByMode.clear();

    m_filteredTestsByMode << sysTestWithSuffix;
    m_filteredTestsByMode << sesTestWithSuffix;
}

QStringList ADTTool::getVarNames() const
{
    QStringList toolVars;

    std::for_each(m_vars.cbegin(), m_vars.cend(), [&toolVars](const std::unique_ptr<ADTVarInterface> &var) {
        toolVars.append(var->id());
    });

    return toolVars;
}

ADTVarInterface *ADTTool::getVar(QString &varId)
{
    ADTVarInterface *var = nullptr;

    for (auto &v : m_vars)
    {
        if (varId == v->id())
        {
            var = v.get();
            break;
        }
    }

    return var;
}

QStringList ADTTool::getFilteredTests()
{
    QStringList result;

    changeMode();

    std::copy_if(m_filteredTestsByMode.begin(), m_filteredTestsByMode.end(), std::back_inserter(result),
                 [this](const QString &test){return getTest(test)->displayName().contains(m_filter);});

    return result;
}

QStringList ADTTool::getTests(BusType bus)
{
    if (bus == BusType::None)
        bus = BusType::All;

    QStringList tests{};

    std::for_each(m_tests.begin(), m_tests.end(), [this, &tests, &bus](std::unique_ptr<ADTTest> &test) {
        if (test->bus() & bus)
            tests.push_back(test->id());
    });

    return tests;
}

ADTTest *ADTTool::getTest(QString id)
{
    ADTTest *test = nullptr;

    auto it = std::find_if(m_tests.begin(), m_tests.end(), [this, &id](std::unique_ptr<ADTTest> &testPtr) {
        return !QString::compare(testPtr->id(), id, Qt::CaseSensitive);
    });

    if ( it != m_tests.cend() )
        test = it->get();

    return test;
}

void ADTTool::clearTestsLogs()
{
    std::for_each(m_tests.begin(), m_tests.end(), [](std::unique_ptr<ADTTest> &test) { test->clearLogs(); });
}
