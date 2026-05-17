#include "library.h"
#include <algorithm>
#include <cctype>
#include <sstream>

static std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return out;
}

bool Library::containsCI(const std::string& haystack,
    const std::string& needle) {
    return toLower(haystack).find(toLower(needle)) != std::string::npos;
}

Library::Library() {
    auto now = std::time(nullptr);

    auto makeOverdue = [&](int id, const std::string& title,
        const std::string& author,
        const std::string& genre,
        const std::string& borrower,
        int overdueDays) {
            Book b(id, title, author, genre);
            b.isAvailable = false;
            b.borrowedBy = borrower;
            b.dueDate = now - static_cast<std::time_t>(overdueDays) * 86400;
            return b;
        };

    books_ = {
        Book(1,  "Bratq Karamazovi",       "Fjordor Dostoevski",   "Класическа "),
        Book(2,  "Pod igoto",               "Ivan Vazov",           "Исторически роман"),
        Book(3,  "Maystora i Margarita",    "Mihail Bulgakov",      "Магически реализъм"),
        Book(4,  "Dyun",                    "Frank Harburt",        "Научна Фантастика"),
        Book(5,  "1984",                    "Dzhordzh Oruell",      "Дистопия"),
        Book(6,  "Malkiyat prints",         "Antoan de Sent-Ekzyuperi", "Детска Литература"),
        Book(7,  "Sto godini samota",       "Gabriel Garcia Markes","Магически реализъм"),
        Book(8,  "Prestaplenie i nakazanie","Fyodor Dostoevski",    "Класическа Литература"),
        Book(9,  "Alhimikut",               "Paulo Koelyo",         "Философски Роман"),
        Book(10, "Voyna i mir",             "Lev Tolstoy",          "Исторически Роман"),
        makeOverdue(11, "Harry Potter i Filosofskiyat Kamak",
                        "J. K. Rowling", "фентази", "Ivan Petrov", 5),
        makeOverdue(12, "Vlastelinyat na Prustenite",
                        "J. R. R. Tolkien", "Фентази", "Maria Ivanova", 12),
        makeOverdue(13, "Protsesut",
                        "Franz Kafka", "Модернизъм", "Georgi Dimitrov", 3),
        Book(14, "Putehestviyata na Gulivar","Jonathan Swift",   "Сатира"),
        Book(15, "Portret na Dorian Grey",  "Oscar Wilde",          "Готически роман"),
    };
    nextId_ = 16;
}

void Library::addBook(const Book& b) {
    books_.push_back(b);
}

bool Library::borrowBook(int id, const std::string& borrowerName, int loanDays) {
    for (auto& b : books_) {
        if (b.id == id) {
            if (!b.isAvailable) return false;
            b.isAvailable = false;
            b.borrowedBy = borrowerName;
            b.dueDate = std::time(nullptr) + static_cast<std::time_t>(loanDays) * 86400;
            return true;
        }
    }
    return false;
}

bool Library::returnBook(int id) {
    for (auto& b : books_) {
        if (b.id == id) {
            if (b.isAvailable) return false;
            b.isAvailable = true;
            b.borrowedBy = "";
            b.dueDate = 0;
            return true;
        }
    }
    return false;
}

std::vector<Book> Library::getSorted(SortBy by, bool ascending) const {
    std::vector<Book> result = books_;
    auto cmp = [&](const Book& a, const Book& b) -> bool {
        bool less = false;
        switch (by) {
        case SortBy::TITLE:        less = a.title < b.title;  break;
        case SortBy::AUTHOR:       less = a.author < b.author; break;
        case SortBy::GENRE:        less = a.genre < b.genre;  break;
        case SortBy::AVAILABILITY: less = (a.isAvailable && !b.isAvailable); break;
        }
        return ascending ? less : !less;
        };
    std::sort(result.begin(), result.end(), cmp);
    return result;
}

std::vector<Book> Library::searchByTitle(const std::string& query) const {
    std::vector<Book> out;
    for (const auto& b : books_)
        if (containsCI(b.title, query)) out.push_back(b);
    return out;
}

std::vector<Book> Library::searchByAuthor(const std::string& query) const {
    std::vector<Book> out;
    for (const auto& b : books_)
        if (containsCI(b.author, query)) out.push_back(b);
    return out;
}

std::vector<Book> Library::searchByGenre(const std::string& genre) const {
    std::vector<Book> out;
    for (const auto& b : books_)
        if (containsCI(b.genre, genre)) out.push_back(b);
    return out;
}

std::vector<Library::FineRecord> Library::getOverdueFines() const {
    std::vector<FineRecord> records;
    for (const auto& b : books_) {
        int days = b.daysOverdue();
        if (days > 0) {
            records.push_back({
                b.id, b.title, b.borrowedBy,
                days,
                FineCalculator::calculate(days)
                });
        }
    }
    return records;
}

double Library::getTotalFines() const {
    double total = 0.0;
    for (const auto& r : getOverdueFines())
        total += r.fine;
    return total;
}