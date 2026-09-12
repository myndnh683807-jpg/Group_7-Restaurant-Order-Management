#include "JsonHelper.h"

namespace restaurant {
namespace utils {

nlohmann::json JsonHelper::tableToJson(
    const model::Table& table
) {
    return {
        {"id", table.getId()},
        {"tableNumber", table.getTableNumber()},
        {"capacity", table.getCapacity()},
        {"status", table.getStatus()}
    };
}


nlohmann::json JsonHelper::tablesToJson(
    const std::vector<model::Table>& tables
) {
    nlohmann::json result =
        nlohmann::json::array();

    for (const auto& table : tables) {
        result.push_back(
            tableToJson(table)
        );
    }

    return result;
}


nlohmann::json JsonHelper::success(
    const std::string& message
) {
    return {
        {"success", true},
        {"message", message}
    };
}


nlohmann::json JsonHelper::error(
    const std::string& message
) {
    return {
        {"success", false},
        {"message", message}
    };
}

}
}