#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include "library.h"

namespace Color {
    const char* RESET = "\033[0m";
    const char* BOLD = "\033[1m";
    const char* RED = "\033[31m";
    const char* GREEN = "\033[32m";
    const char* YELLOW = "\033[33m";
    const char* BLUE = "\033[34m";
    const char* MAGENTA = "\033[35m";
    const char* CYAN = "\033[36m";
    const char* WHITE = "\033[97m";
}

static void printSeparator(int width = 80) {
    std::cout << Color::BLUE;
    for (int i = 0; i < width; ++i) std::cout << '-';
    std::cout << Color::RESET << '\n';
}

static void printHeader(const std::string& title) {
    std::cout << '\n';
    printSeparator();
    std::cout << Color::BOLD << Color::CYAN
        << "  [*]  " << title << Color::RESET << '\n';
    printSeparator();
}

static void printBookRow(const Book& b, int index = -1) {
    if (index >= 0)
        std::cout << Color::YELLOW << std::setw(3) << index + 1 << ". " << Color::RESET;

    std::cout << Color::BOLD << Color::WHITE
        << std::left << std::setw(38) << b.title.substr(0, 37)
        << Color::RESET
        << std::setw(25) << b.author.substr(0, 24)
        << std::setw(22) << b.genre.substr(0, 21);

    if (b.isAvailable) {
        std::cout << Color::GREEN << "[+] Налична" << Color::RESET;
    }
    else {
        std::cout << Color::RED << "[-] Наета" << Color::RESET;
        int od = b.daysOverdue();
        if (od > 0)
            std::cout << Color::RED << "  ! " << od << " дни просрочени" << Color::RESET;
    }
    std::cout << '\n';
}

static void printBooksTable(const std::vector<Book>& books) {
    if (books.empty()) {
        std::cout << Color::YELLOW << "  Няма намерени книги.\n" << Color::RESET;
        return;
    }
    std::cout << Color::BOLD
        << "  #   " << std::left
        << std::setw(38) << "Заглавие"
        << std::setw(25) << "Автор"
        << std::setw(22) << "Жанр"
        << "Статус\n" << Color::RESET;
    printSeparator();
    for (int i = 0; i < (int)books.size(); ++i)
        printBookRow(books[i], i);
}

static int readInt(const std::string& prompt) {
    int val;
    while (true) {
        std::cout << Color::CYAN << prompt << Color::RESET;
        if (std::cin >> val) { std::cin.ignore(); return val; }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << Color::RED << "  Невалиден вход!\n" << Color::RESET;
    }
}

static std::string readLine(const std::string& prompt) {
    std::cout << Color::CYAN << prompt << Color::RESET;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static void menuSort(Library& lib) {
    printHeader("Сортиране на книги");
    std::cout << "  1. По заглавие\n"
        << "  2. По автор\n"
        << "  3. По жанр\n"
        << "  4. По наличност\n";
    int choice = readInt("  Изберете: ");
    bool asc = (readInt("  1=Нарастващ / 2=Намаляващ: ") == 1);

    Library::SortBy by;
    switch (choice) {
    case 1: by = Library::SortBy::TITLE;        break;
    case 2: by = Library::SortBy::AUTHOR;       break;
    case 3: by = Library::SortBy::GENRE;        break;
    case 4: by = Library::SortBy::AVAILABILITY; break;
    default: std::cout << Color::RED << "  Невалиден избор!\n"; return;
    }
    auto sorted = lib.getSorted(by, asc);
    printBooksTable(sorted);
}

static void menuSearch(Library& lib) {
    printHeader("Търсене на книга");
    std::cout << "  1. По заглавие\n"
        << "  2. По автор\n"
        << "  3. По жанр\n";
    int choice = readInt("  Изберете: ");
    std::string query = readLine("  Въведете текст: ");

    std::vector<Book> results;
    switch (choice) {
    case 1: results = lib.searchByTitle(query);  break;
    case 2: results = lib.searchByAuthor(query); break;
    case 3: results = lib.searchByGenre(query);  break;
    default: std::cout << Color::RED << "  Невалиден избор!\n"; return;
    }
    std::cout << '\n' << Color::MAGENTA << "  Намерени резултати: "
        << results.size() << Color::RESET << '\n';
    printBooksTable(results);
}

static void menuFines(Library& lib) {
    printHeader("Глоби за просрочие");
    auto fines = lib.getOverdueFines();
    if (fines.empty()) {
        std::cout << Color::GREEN << "  Няма просрочени книги!\n" << Color::RESET;
        return;
    }
    std::cout << Color::BOLD
        << std::left
        << std::setw(6) << "  ID"
        << std::setw(35) << "Заглавие"
        << std::setw(20) << "Наемател"
        << std::setw(12) << "Просрочие"
        << "Глоба\n" << Color::RESET;
    printSeparator();
    for (const auto& r : fines) {
        std::cout << "  " << std::setw(4) << r.bookId
            << std::setw(35) << r.title.substr(0, 34)
            << std::setw(20) << r.borrower.substr(0, 19)
            << Color::RED
            << std::setw(7) << r.daysOverdue << " дни   "
            << std::fixed << std::setprecision(2) << r.fine << " EUR"
            << Color::RESET << '\n';
    }
    printSeparator();
    std::cout << Color::BOLD << Color::YELLOW
        << "  Общо глоби: " << std::fixed << std::setprecision(2)
        << lib.getTotalFines() << " EUR"
        << Color::RESET << '\n';
    std::cout << Color::BLUE
        << "  (Изчислено рекурсивно: " << FineCalculator::FINE_PER_DAY
        << " EUR/ден, максимум " << FineCalculator::MAX_FINE << " EUR)\n"
        << Color::RESET;
}

static void menuBorrow(Library& lib) {
    printHeader("Наемане / Връщане на книга");
    std::cout << "  1. Наеми книга\n"
        << "  2. Върни книга\n";
    int action = readInt("  Изберете: ");
    printBooksTable(lib.getAllBooks());
    int id = readInt("  Въведете ID на книга: ");

    if (action == 1) {
        std::string name = readLine("  Вашето име: ");
        int days = readInt("  Дни за наемане (по подразбиране 14): ");
        if (lib.borrowBook(id, name, days))
            std::cout << Color::GREEN << "  [+] Книгата е наета успешно!\n" << Color::RESET;
        else
            std::cout << Color::RED << "  [-] Книгата не е налична или ID е грешен!\n" << Color::RESET;
    }
    else if (action == 2) {
        if (lib.returnBook(id))
            std::cout << Color::GREEN << "  [+] Книгата е върната успешно!\n" << Color::RESET;
        else
            std::cout << Color::RED << "  [-] Грешен ID или книгата вече е налична!\n" << Color::RESET;
    }
}

static void menuAdd(Library& lib) {
    printHeader("Добавяне на нова книга");
    std::string title = readLine("  Заглавие: ");
    std::string author = readLine("  Автор: ");
    std::string genre = readLine("  Жанр: ");
    auto books = lib.getAllBooks();
    int newId = books.empty() ? 1 : (books.back().id + 1);
    lib.addBook(Book(newId, title, author, genre));
    std::cout << Color::GREEN << "  [+] Книгата е добавена с ID " << newId << "!\n" << Color::RESET;
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    Library lib;

    while (true) {
        printHeader("Система за управление на библиотека");
        std::cout << "  1.  Всички книги\n"
            << "  2.  Сортиране\n"
            << "  3.  Търсене\n"
            << "  4.  Глоби за просрочие\n"
            << "  5.  Наемане / Връщане\n"
            << "  6.  Добавяне на книга\n"
            << "  0.  Изход\n";
        int choice = readInt("  Избор: ");
        switch (choice) {
        case 1: printBooksTable(lib.getAllBooks()); break;
        case 2: menuSort(lib);    break;
        case 3: menuSearch(lib);  break;
        case 4: menuFines(lib);   break;
        case 5: menuBorrow(lib);  break;
        case 6: menuAdd(lib);     break;
        case 0:
            std::cout << Color::CYAN << "\n  Довиждане!\n\n" << Color::RESET;
            return 0;
        default:
            std::cout << Color::RED << "  Невалиден избор!\n" << Color::RESET;
        }
    }
}