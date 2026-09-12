#pragma once

#include "../model/Invoice.h"

#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace repository {

class InvoiceRepository {
public:
    InvoiceRepository() = default;
    ~InvoiceRepository() = default;

    std::vector<model::Invoice> getAll();

    std::optional<model::Invoice> getById(
        int invoiceId
    );

    std::optional<model::Invoice> getByOrderId(
        int orderId
    );

    model::Invoice add(
        const model::Invoice& invoice
    );

    bool update(
        const model::Invoice& invoice
    );

    bool remove(
        int invoiceId
    );

    bool updatePaymentStatus(
        int invoiceId,
        const std::string& status
    );
};

}
}