#include "ReportApiController.h"

std::string ReportApiController::handleGetRevenueReport(std::string timeRange) {
    return "{\"timeRange\": \"" + timeRange + "\", \"totalRevenue\": 18450000, \"totalOrders\": 84}";
}
