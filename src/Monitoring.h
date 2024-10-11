#ifndef MONITORING_SERVICE_H
#define MONITORING_SERVICE_H

#include <vector>

#include "CANID.h"
#include "Parser.h"
class Monitoring
{
public:
    Monitoring();
    bool monitoringCanData();

private:
    bool makeInstance();
    std::shared_ptr<Parser> m_pParser;
};
#endif /* MONITORING_SERVICE_H */