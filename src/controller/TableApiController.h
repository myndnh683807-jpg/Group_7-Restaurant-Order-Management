#pragma once

#include "../repository/TableRepository.h"
#include "../repository/OrderRepository.h"

#include <crow.h>
#include <crow/middlewares/cors.h>

namespace restaurant {
namespace controller {

class TableApiController {
public:
    TableApiController(
        repository::TableRepository& tableRepository,
        repository::OrderRepository& orderRepository
    );

    void registerRoutes(
        crow::App<crow::CORSHandler>& app
    );

private:
    repository::TableRepository&
        tableRepository_;

    repository::OrderRepository&
        orderRepository_;
};

}
}