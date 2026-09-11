#include <iostream>
#include <string>

using namespace std;

class ReportController {
public:
    // UC 15: View Revenue Reports (Xem báo cáo doanh thu cho Manager)
    void viewRevenueReports(string timeRange) {
        cout << "\n[Manager Dashboard - Revenue Report (UC 15)]\n";
        cout << "Time Range Selected: " << timeRange << "\n";
        cout << "Total Revenue: $18,450,000 VND\n";
        cout << "Total Orders Processed: 84 Orders\n";
        cout << "Top-performing Item: Dish name 1\n";
    }
};