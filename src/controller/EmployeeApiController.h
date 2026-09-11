#ifndef EMPLOYEE_API_CONTROLLER_H
#define EMPLOYEE_API_CONTROLLER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Employee {
    std::string username;
    std::string password;
    std::string fullName;
    std::string role;
};

class EmployeeApiController {
private:
    std::vector<Employee> employeeList;

public:
    EmployeeApiController();
    json createEmployee(const json& requestJson);
    json getAllEmployees() const;
    json deleteEmployee(const std::string& username);
};

#endif // EMPLOYEE_API_CONTROLLER_H