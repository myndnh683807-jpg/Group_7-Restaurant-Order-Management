#pragma once

#include "TableController.h"

#include <crow.h>
#include <crow/middlewares/cors.h>

namespace restaurant {
namespace controller {

class TableApiController {
public:
    explicit TableApiController(
        TableController& tableController
    );

    void registerRoutes(
        crow::App<crow::CORSHandler>& app
    );

private:
    TableController& tableController_;
};

}
}