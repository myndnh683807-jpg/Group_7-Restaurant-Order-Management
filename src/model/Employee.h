#pragma once

#include <optional>
#include <string>

namespace restaurant {
namespace model {

class Employee {
public:
    Employee() = default;

    Employee(
        std::string username,
        std::string passwordHash,
        std::string fullName,
        std::string role,
        std::optional<std::string> phone = std::nullopt
    );

    Employee(
        int id,
        std::string username,
        std::string passwordHash,
        std::string fullName,
        std::string role,
        std::optional<std::string> phone = std::nullopt
    );

    int getId() const noexcept;
    const std::string& getUsername() const noexcept;
    const std::string& getPasswordHash() const noexcept;
    const std::string& getFullName() const noexcept;
    const std::string& getRole() const noexcept;
    const std::optional<std::string>& getPhone() const noexcept;

    void setId(int id);
    void setUsername(const std::string& username);
    void setPasswordHash(const std::string& passwordHash);
    void setFullName(const std::string& fullName);
    void setRole(const std::string& role);
    void setPhone(const std::optional<std::string>& phone);

private:
    int id_{0};
    std::string username_;
    std::string passwordHash_;
    std::string fullName_;
    std::string role_;
    std::optional<std::string> phone_;
};

}
}