#include <iostream>
#include <string>
using namespace std;
class POSController {
public:
    void checkBill(int orderId, double subtotal, bool isVip) {
        double vatTax = subtotal * 0.08;
        double discount = isVip ? subtotal * 0.10 : 0.0;
        double grandTotal = (subtotal - discount) + vatTax;
        cout << "========================================\n";
        cout << "       ITEMIZED BILL (Order #" << orderId << ") \n";
        cout << "========================================\n";
        cout << "Subtotal: $" << subtotal << "\n";
        cout << "VAT Tax (8%): $" << vatTax << "\n";
        if (isVip) {
            cout << "Membership Discount (VIP 10%): -$" << discount << "\n";
        }
        cout << "GRAND TOTAL DUE: $" << grandTotal << "\n";
        cout << "========================================\n";
    }
    bool processPayment(int orderId, double grandTotal, string paymentMethod, double cashTendered) {
        cout << "\n[Processing Payment...]\n";
        cout << "Selected Method: " << paymentMethod << "\n";

        if (paymentMethod == "Cash") {
            if (cashTendered < grandTotal) {
                cout << "Error: Cash tendered is less than Grand Total!\n";
                return false;
            }
            double changeDue = cashTendered - grandTotal;
            cout << "Payment Successful! Change Due to Customer: $" << changeDue << "\n";
        }
        else {
            cout << "Payment via " << paymentMethod << " processed successfully!\n";
        }
        generateReceipt(orderId, grandTotal, paymentMethod);
        return true;
    }
    void generateReceipt(int orderId, double amountPaid, string method) {
        cout << "\n----------------------------------------\n";
        cout << "          OFFICIAL RECEIPT              \n";
        cout << "----------------------------------------\n";
        cout << "Order ID: #" << orderId << "\n";
        cout << "Amount Paid: $" << amountPaid << "\n";
        cout << "Method: " << method << "\n";
        cout << "Status: PAID\n";
        cout << "----------------------------------------\n";
    }
    void viewRevenueReports(string timeRange) {
        cout << "\n[Manager Dashboard - Revenue Report]\n";
        cout << "Time Range: " << timeRange << "\n";
        cout << "Total Revenue: $18,450,000 VND | Total Orders: 84\n";
    }
};
int main() {
    POSController pos;
    pos.checkBill(105, 60.99, true);
    pos.processPayment(105, 59.42, "Cash", 70.0);
    pos.viewRevenueReports("This Week");
    return 0;
}