#include "Monitoring.h"

Monitoring::Monitoring()
    :m_pParser()
{
    makeInstance();
}

bool Monitoring::makeInstance()
{
    std::vector<CANID> lstMonitoringTarget = { CANID::RPM };
    m_pParser = std::make_shared<Parser>(lstMonitoringTarget);
    return true;
}