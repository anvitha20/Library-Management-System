#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <string>
using namespace std;

class Book {
private:
    long bookID;
    string title;
    string author;
    static long NextBookID;

public:
    Book() {}
    Book(string title, string author) {
        NextBookID++;
        this->bookID = NextBookID;
        this->title = title;
        this->author = author;
    }

    long getBookID() { return bookID; }
    string getTitle() { return title; }
    string getAuthor() { return author; }

    static void setLastBookID(long id) { NextBookID = id; }
    static long getLastBookID() { return NextBookID; }

    friend ofstream& operator<<(ofstream& ofs, Book& book);
    friend ifstream& operator>>(ifstream& ifs, Book& book);
    friend ostream& operator<<(ostream& os, Book& book);
};

long Book::NextBookID = 0;

ofstream& operator<<(ofstream& ofs, Book& book) {
    ofs << book.bookID << endl;
    ofs << book.title << endl;
    ofs << book.author << endl;
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Book& book) {
    ifs >> book.bookID;
    ifs >> book.title;
    ifs >> book.author;
    return ifs;
}

ostream& operator<<(ostream& os, Book& book) {
    os << "Book ID: " << book.getBookID() << endl;
    os << "Title: " << book.getTitle() << endl;
    os << "Author: " << book.getAuthor() << endl;
    return os;
}

class Member {
private:
    long memberID;
    string name;
    static long NextMemberID;

public:
    Member() {}
    Member(string name) {
        NextMemberID++;
        this->memberID = NextMemberID;
        this->name = name;
    }

    long getMemberID() { return memberID; }
    string getName() { return name; }

    static void setLastMemberID(long id) { NextMemberID = id; }
    static long getLastMemberID() { return NextMemberID; }

    friend ofstream& operator<<(ofstream& ofs, Member& member);
    friend ifstream& operator>>(ifstream& ifs, Member& member);
    friend ostream& operator<<(ostream& os, Member& member);
};

long Member::NextMemberID = 0;

ofstream& operator<<(ofstream& ofs, Member& member) {
    ofs << member.memberID << endl;
    ofs << member.name << endl;
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Member& member) {
    ifs >> member.memberID;
    ifs >> member.name;
    return ifs;
}

ostream& operator<<(ostream& os, Member& member) {
    os << "Member ID: " << member.getMemberID() << endl;
    os << "Name: " << member.getName() << endl;
    return os;
}

class Library {
private:
    map<long, Book> books;
    map<long, Member> members;
    map<long, pair<long, time_t>> borrowedBooks; // bookID -> (memberID, dueDate)

    time_t getDueDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        ltm->tm_sec += 10; // Borrowing period of two months
        return mktime(ltm);
    }

public:
    Library() {
        Book book;
        ifstream infile("Library.data");
        if (infile) {
            while (infile >> book) {
                books.insert(pair<long, Book>(book.getBookID(), book));
            }
            Book::setLastBookID(book.getBookID());
            infile.close();
        }
        Member member;
        infile.open("Members.data");
        if (infile) {
            while (infile >> member) {
                members.insert(pair<long, Member>(member.getMemberID(), member));
            }
            Member::setLastMemberID(member.getMemberID());
            infile.close();
        }
    }

    Book AddBook(string title, string author) {
        ofstream outfile;
        Book book(title, author);
        books.insert(pair<long, Book>(book.getBookID(), book));

        outfile.open("Library.data", ios::trunc);
        for (auto& pair : books) {
            outfile << pair.second;
        }
        outfile.close();
        return book;
    }

    Member AddMember(string name) {
        ofstream outfile;
        Member member(name);
        members.insert(pair<long, Member>(member.getMemberID(), member));

        outfile.open("Members.data", ios::trunc);
        for (auto& pair : members) {
            outfile << pair.second;
        }
        outfile.close();
        return member;
    }

    void DisplayBooks() {
        for (auto& pair : books) {
            cout << pair.second << endl;
        }
    }

    void DisplayMembers() {
        for (auto& pair : members) {
            cout << pair.second << endl;
        }
    }

    void BorrowBook(long bookID, long memberID) {
        if (books.find(bookID) != books.end() && members.find(memberID) != members.end()) {
            if (borrowedBooks.find(bookID) == borrowedBooks.end()) {
                borrowedBooks[bookID] = make_pair(memberID, getDueDate());
                cout << "Book borrowed successfully!" << endl;
            } else {
                cout << "Book is already borrowed!" << endl;
            }
        } else {
            cout << "Invalid Book ID or Member ID!" << endl;
        }
    }

    void ReturnBook(long bookID) {
        if (borrowedBooks.find(bookID) != borrowedBooks.end()) {
            time_t now = time(0);
            tm* ltm = localtime(&now);
            if (now <= borrowedBooks[bookID].second) {
                cout << "Book returned successfully!" << endl;
            } else {
                cout << "Book returned late!" << endl;
            }
            borrowedBooks.erase(bookID);
        } else {
            cout << "Book was not borrowed!" << endl;
        }
    }

    ~Library() {
        ofstream outfile;
        outfile.open("Library.data", ios::trunc);
        for (auto& pair : books) {
            outfile << pair.second;
        }
        outfile.close();
        outfile.open("Members.data", ios::trunc);
        for (auto& pair : members) {
            outfile << pair.second;
        }
        outfile.close();
    }
};

int main() {
    Library lib;
    int choice;
    string title, author, name;
    long bookID, memberID;

    cout << "*** Library Management System ***" << endl;

    do {
        cout << "\n1. Add Book";
        cout << "\n2. Add Member";
        cout << "\n3. Display All Books";
        cout << "\n4. Display All Members";
        cout << "\n5. Borrow Book";
        cout << "\n6. Return Book";
        cout << "\n7. Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Book Title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter Author Name: ";
                getline(cin, author);
                lib.AddBook(title, author);
                break;
            case 2:
                cout << "Enter Member Name: ";
                cin.ignore();
                getline(cin, name);
                lib.AddMember(name);
                break;
            case 3:
                lib.DisplayBooks();
                break;
            case 4:
                lib.DisplayMembers();
                break;
            case 5:
                cout << "Enter Book ID to Borrow: ";
                cin >> bookID;
                cout << "Enter Member ID: ";
                cin >> memberID;
                lib.BorrowBook(bookID, memberID);
                break;
            case 6:
                cout << "Enter Book ID to Return: ";
                cin >> bookID;
                lib.ReturnBook(bookID);
                break;
            case 7:
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 7);

    return 0;
}
