#ifndef PAYMENT_API_CONTROLLER_H
#define PAYMENT_API_CONTROLLER_H

#include <string>

class PaymentApiController {
public:
    std::string handleCheckBill(int orderId, double subtotal, bool isVip);
    std::string handleProcessPayment(int orderId, double grandTotal, std::string method, double cash);
};

#endif
