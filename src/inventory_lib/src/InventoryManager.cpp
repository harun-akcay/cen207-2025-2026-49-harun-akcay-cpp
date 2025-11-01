#include "../header/InventoryManager.h"
#include <stdexcept>

using namespace Coruh::Inventory;

double Inventory::add(double a, double b) {
    return a + b;
}

double Inventory::subtract(double a, double b) {
    return a - b;
}

double Inventory::multiply(double a, double b) {
    return a * b;
}

double Inventory::divide(double a, double b) {
    if (b == 0) {
        throw std::invalid_argument("Division by zero is not allowed.");
    }
    return a / b;
}