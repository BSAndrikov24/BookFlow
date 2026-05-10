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

class Library {
public:
    Library();  
   
    void addBook(const Book& b);
    bool borrowBook(int id, const std::string& borrowerName, int loanDays = 14);
    bool returnBook(int id);
    
    enum class SortBy { TITLE, AUTHOR, GENRE, AVAILABILITY };
    std::vector<Book> getSorted(SortBy by, bool ascending = true) const;
    
    std::vector<Book> searchByTitle(const std::string& query) const;
    std::vector<Book> searchByAuthor(const std::string& query) const;
    std::vector<Book> searchByGenre(const std::string& genre) const;
    std::vector<Book> getAllBooks() const { return books_; }
    
    struct FineRecord {
        int    bookId;
        std::string title;
        std::string borrower;
        int    daysOverdue;
        double fine;
    };
    std::vector<FineRecord> getOverdueFines() const;
    double getTotalFines() const;

private:
    std::vector<Book> books_;
    int nextId_ = 1;

    static bool containsCI(const std::string& haystack,
        const std::string& needle);
};