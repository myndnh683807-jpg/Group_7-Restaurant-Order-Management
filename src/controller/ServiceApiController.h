#pragma once

#include "ServiceController.h"

#include <crow.h>
#include <crow/middlewares/cors.h>

namespace restaurant {
namespace controller {

class ServiceApiController {
public:
    explicit ServiceApiController(
        ServiceController& serviceController
    );

    void registerRoutes(
        crow::App<crow::CORSHandler>& app
    );

private:
    ServiceController& serviceController_;
};

}
}