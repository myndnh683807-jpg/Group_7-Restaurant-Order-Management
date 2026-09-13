#include "MenuItem.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace restaurant {
namespace model {

MenuItem::MenuItem(int categoryId, std::string name,
                   std::optional<std::string> description, double price,
                   std::optional<std::string> imageUrl, bool isAvailable) {
    setCategoryId(categoryId);
    setName(name);
    setDescription(description);
    setPrice(price);
    setImageUrl(imageUrl);
    setIsAvailable(isAvailable);
}

MenuItem::MenuItem(int id, int categoryId, std::string name,
                   std::optional<std::string> description, double price,
                   std::optional<std::string> imageUrl, bool isAvailable)
    : MenuItem(categoryId, std::move(name), std::move(description),
               price, std::move(imageUrl), isAvailable) {
    setId(id);
}

int MenuItem::getId() const noexcept {
    return id_;
}

int MenuItem::getCategoryId() const noexcept {
    return categoryId_;
}

const std::string& MenuItem::getName() const noexcept {
    return name_;
}

const std::optional<std::string>& MenuItem::getDescription() const noexcept {
    return description_;
}

double MenuItem::getPrice() const noexcept {
    return price_;
}

const std::optional<std::string>& MenuItem::getImageUrl() const noexcept {
    return imageUrl_;
}

bool MenuItem::getIsAvailable() const noexcept {
    return isAvailable_;
}

void MenuItem::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument("MenuItem ID cannot be negative.");
    }

    id_ = id;
}

void MenuItem::setCategoryId(int categoryId) {
    if (categoryId <= 0) {
        throw std::invalid_argument("Category ID must be positive.");
    }

    categoryId_ = categoryId;
}

void MenuItem::setName(const std::string& name) {
    if (name.empty() || name.size() > 100) {
        throw std::invalid_argument(
            "Item name must contain 1 to 100 bytes."
        );
    }

    name_ = name;
}

void MenuItem::setDescription(
    const std::optional<std::string>& description
) {
    description_ = description;
}

void MenuItem::setPrice(double price) {
    if (!std::isfinite(price) ||
        price < 0.0 ||
        price > 99999999.99) {

        throw std::invalid_argument(
            "Price must be within DECIMAL(10,2) range."
        );
    }

    price_ = price;
}

void MenuItem::setImageUrl(
    const std::optional<std::string>& imageUrl
) {
    if (imageUrl && imageUrl->size() > 255) {
        throw std::invalid_argument(
            "Image URL must not exceed 255 bytes."
        );
    }

    imageUrl_ = imageUrl;
}

void MenuItem::setIsAvailable(bool isAvailable) {
    isAvailable_ = isAvailable;
}

}
}