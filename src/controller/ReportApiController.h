#ifndef REPORT_API_CONTROLLER_H
#define REPORT_API_CONTROLLER_H

#include <string>

class ReportApiController {
public:
    std::string handleGetRevenueReport(std::string timeRange);
};

#endif
