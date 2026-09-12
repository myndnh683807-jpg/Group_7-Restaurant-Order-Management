#ifndef PAYMENT_CONTROLLER_H
#define PAYMENT_CONTROLLER_H

#include <string>

class PaymentController {
public:
    void checkBill(int orderId, double subtotal, bool isVip);
    bool processPayment(int orderId, double grandTotal, std::string paymentMethod, double cashTendered);
    void generateReceipt(int orderId, double amountPaid, std::string method);
};

#endif
