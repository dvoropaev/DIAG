#ifndef ADTVARBUILDERMODE_H
#define ADTVARBUILDERMODE_H

#include "adtvarbuilderinterface.h"

class ADTVarBuilderMode : public ADTVarBuilderInterface
{
public:
    ADTVarBuilderMode()          = default;
    virtual ~ADTVarBuilderMode() = default;

    bool build(const toml::table *paramSection, ADTTool *tool, const QString &varId) override;
};

#endif // ADTVARBUILDERMODE_H
