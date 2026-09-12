#pragma once

#include "../model/Table.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace utils {

class JsonHelper {
public:

    static nlohmann::json tableToJson(
        const model::Table& table
    );

    static nlohmann::json tablesToJson(
        const std::vector<model::Table>& tables
    );

    static nlohmann::json success(
        const std::string& message
    );

    static nlohmann::json error(
        const std::string& message
    );
};

}
}