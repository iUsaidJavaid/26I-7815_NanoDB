#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

void generateCustomerData(const char* filename, int count) {
    FILE* fp = fopen(filename, "w");
    if (fp == nullptr) {
        std::cout << "[ERROR] Failed to open " << filename << std::endl;
        return;
    }

    const char* names[] = {"Alice", "Bob", "Charlie", "Diana", "Eve", "Frank", "Grace", "Hank", "Ivy", "Jack",
                          "Kate", "Leo", "Mike", "Nina", "Oscar", "Paul", "Quinn", "Rose", "Sam", "Tina"};
    const char* segments[] = {"BUILDING", "AUTOMOBILE", "MACHINERY", "HOUSEHOLD", "FURNITURE"};
    int nameCount = 20;
    int segmentCount = 5;

    for (int i = 1; i <= count; ++i) {
        int nameIdx = (i - 1) % nameCount;
        int segmentIdx = (i - 1) % segmentCount;
        int nationkey = (i % 5) + 1;
        double acctbal = 1000.0 + (rand() % 9000) + (rand() % 100) / 100.0;

        fprintf(fp, "INSERT INTO customer VALUES (%d, \"%s%d\", %.2f, \"%s\", %d)\n",
                i, names[nameIdx], i, acctbal, segments[segmentIdx], nationkey);
    }

    fclose(fp);
    std::cout << "[DataGenerator] Generated " << count << " customer records in " << filename << std::endl;
}

void generateOrdersData(const char* filename, int count, int customerCount) {
    FILE* fp = fopen(filename, "w");
    if (fp == nullptr) {
        std::cout << "[ERROR] Failed to open " << filename << std::endl;
        return;
    }

    for (int i = 1; i <= count; ++i) {
        int custkey = (i % customerCount) + 1;
        double totalprice = 50.0 + (rand() % 5000) + (rand() % 100) / 100.0;
        int year = 2024;
        int month = (i % 12) + 1;
        int day = (i % 28) + 1;

        fprintf(fp, "INSERT INTO orders VALUES (%d, %d, %.2f, \"%04d-%02d-%02d\")\n",
                100 + i, custkey, totalprice, year, month, day);
    }

    fclose(fp);
    std::cout << "[DataGenerator] Generated " << count << " orders records in " << filename << std::endl;
}

void generateLineItemData(const char* filename, int count, int ordersCount) {
    FILE* fp = fopen(filename, "w");
    if (fp == nullptr) {
        std::cout << "[ERROR] Failed to open " << filename << std::endl;
        return;
    }

    for (int i = 1; i <= count; ++i) {
        int orderkey = 100 + ((i - 1) % ordersCount) + 1;
        int linenumber = (i % 10) + 1;
        double extendedprice = 10.0 + (rand() % 500) + (rand() % 100) / 100.0;

        fprintf(fp, "INSERT INTO lineitem VALUES (%d, %d, %d, %.2f)\n",
                1000 + i, orderkey, linenumber, extendedprice);
    }

    fclose(fp);
    std::cout << "[DataGenerator] Generated " << count << " lineitem records in " << filename << std::endl;
}

void generateTPCHDataset() {
    const int CUSTOMER_COUNT = 20000;
    const int ORDERS_COUNT = 30000;
    const int LINEITEM_COUNT = 50000;

    std::cout << "[DataGenerator] Starting TPC-H-like dataset generation..." << std::endl;
    std::cout << "[DataGenerator] Target: " << (CUSTOMER_COUNT + ORDERS_COUNT + LINEITEM_COUNT)
              << " total records" << std::endl;

    generateCustomerData("data/customer_data.txt", CUSTOMER_COUNT);
    generateOrdersData("data/orders_data.txt", ORDERS_COUNT, CUSTOMER_COUNT);
    generateLineItemData("data/lineitem_data.txt", LINEITEM_COUNT, ORDERS_COUNT);

    std::cout << "[DataGenerator] Dataset generation complete." << std::endl;
}

int main() {
    srand((unsigned int)time(nullptr));
    generateTPCHDataset();
    return 0;
}
