#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdexcept>

struct Book {
    int id;
    std::string title;
    std::string author;
    std::string genre;
    bool isAvailable;
    std::string borrowedBy;   
    std::time_t dueDate;      

    Book(int id, const std::string& title, const std::string& author,
        const std::string& genre)
        : id(id), title(title), author(author), genre(genre),
        isAvailable(true), dueDate(0) {
    }

    int daysOverdue() const {
        if (isAvailable || dueDate == 0) return 0;
        std::time_t now = std::time(nullptr);
        if (now <= dueDate) return 0;
        return static_cast<int>((now - dueDate) / 86400);
    }
};

class FineCalculator {
public:
    static constexpr double FINE_PER_DAY = 0.50; 
    static constexpr double MAX_FINE = 20.00; 

    static double calculate(int daysOverdue, double accumulated = 0.0) {
        if (daysOverdue <= 0) return accumulated;
        double newTotal = accumulated + FINE_PER_DAY;
        if (newTotal >= MAX_FINE) return MAX_FINE;
        return calculate(daysOverdue - 1, newTotal);
    }
};