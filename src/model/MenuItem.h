#pragma once

#include <optional>
#include <string>

namespace restaurant {
namespace model {

class MenuItem {
public:
    MenuItem() = default;

    MenuItem(int categoryId, std::string name,
             std::optional<std::string> description, double price,
             std::optional<std::string> imageUrl = std::nullopt,
             bool isAvailable = true);

    MenuItem(int id, int categoryId, std::string name,
             std::optional<std::string> description, double price,
             std::optional<std::string> imageUrl = std::nullopt,
             bool isAvailable = true);

    int getId() const noexcept;
    int getCategoryId() const noexcept;
    const std::string& getName() const noexcept;
    const std::optional<std::string>& getDescription() const noexcept;
    double getPrice() const noexcept;
    const std::optional<std::string>& getImageUrl() const noexcept;
    bool getIsAvailable() const noexcept;

    void setId(int id);
    void setCategoryId(int categoryId);
    void setName(const std::string& name);
    void setDescription(const std::optional<std::string>& description);
    void setPrice(double price);
    void setImageUrl(const std::optional<std::string>& imageUrl);
    void setIsAvailable(bool isAvailable);

private:
    int id_{0};
    int categoryId_{0};
    std::string name_;
    std::optional<std::string> description_;
    double price_{0.0};
    std::optional<std::string> imageUrl_;
    bool isAvailable_{true};
};

}
}