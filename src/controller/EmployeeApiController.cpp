#include "EmployeeApiController.h"

EmployeeApiController::EmployeeApiController() {}

json EmployeeApiController::createEmployee(const json& requestJson) {
    json response;
    try {
        if (!requestJson.contains("username") || !requestJson.contains("password") ||
            !requestJson.contains("fullName") || !requestJson.contains("role")) {
            response["status"] = "error";
            response["message"] = "Missing required employee fields";
            return response;
        }

        std::string username = requestJson["username"];
        std::string password = requestJson["password"];
        std::string fullName = requestJson["fullName"];
        std::string role = requestJson["role"];

        for (const auto& emp : employeeList) {
            if (emp.username == username) {
                response["status"] = "error";
                response["message"] = "Username already exists";
                return response;
            }
        }

        employeeList.push_back({username, password, fullName, role});
        response["status"] = "success";
        response["message"] = "Employee created successfully";
        response["data"] = {{"username", username}, {"fullName", fullName}, {"role", role}};
    } catch (const std::exception& e) {
        response["status"] = "error";
        response["message"] = e.what();
    }
    return response;
}

json EmployeeApiController::getAllEmployees() const {
    json response;
    json list = json::array();
    for (const auto& emp : employeeList) {
        list.push_back({{"username", emp.username}, {"fullName", emp.fullName}, {"role", emp.role}});
    }
    response["status"] = "success";
    response["data"] = list;
    return response;
}

json EmployeeApiController::deleteEmployee(const std::string& username) {
    json response;
    for (auto it = employeeList.begin(); it != employeeList.end(); ++it) {
        if (it->username == username) {
            employeeList.erase(it);
            response["status"] = "success";
            response["message"] = "Employee deleted successfully";
            return response;
        }
    }
    response["status"] = "error";
    response["message"] = "Employee not found";
    return response;
}