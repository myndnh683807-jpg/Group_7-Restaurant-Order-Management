// src/controller/EmployeeApiController.h
// API Controller — Quản lý nhân viên (UI-05 Admin - UC13).
#pragma once

#include "../model/Employee.h"
#include "../repository/EmployeeRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller phục vụ quản lý nhân sự (UI-05 Admin).
class EmployeeApiController {
public:
    EmployeeApiController() = default;
    ~EmployeeApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);
};

} // namespace controller
} // namespace restaurant
