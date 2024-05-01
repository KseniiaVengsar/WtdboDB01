#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <Windows.h>

#pragma execution_character_set("utf-8")

class Publisher;
class Book;
class Stock;
class Sale;
class Shop;

using namespace std;
using namespace Wt::Dbo;

class Publisher {
public:
    string name;
    collection<ptr<Book>> books;

    template <typename Action>
    void persist(Action& a) {
        field(a, name, "name");
        hasMany(a, books, ManyToOne, "publisher");
    }
};

class Book {
public:
    string title;
    ptr<Publisher> publisher;
    collection<ptr<Stock>> stocks;

    template <class Action>
    void persist(Action& a) {
        field(a, title, "title");
        belongsTo(a, publisher, "publisher");
        hasMany(a, stocks, ManyToOne, "book");
    }
};

class Stock {
public:
    int count;
    ptr<Book> book;
    ptr<Shop> shop;
    collection<ptr<Sale>> sales;

    template <typename Action>
    void persist(Action& a) {
        field(a, count, "count");
        belongsTo(a, book, "book");
        belongsTo(a, shop, "shop");
        hasMany(a, sales, ManyToOne, "stock");
    }
};

class Sale {
public:
    int count;
    int price;
    string date_sale;
    ptr<Stock> stock;

    template <typename Action>
    void persist(Action& a) {
        field(a, count, "count");
        field(a, price, "price");
        field(a, date_sale, "date_sale");
        belongsTo(a, stock, "stock");
    }
};

class Shop {
public:
    string name;
    collection<ptr<Stock>> stocks;

    template <typename Action>
    void persist(Action& a) {
        field(a, name, "name");
        hasMany(a, stocks, ManyToOne, "shop");
    }
};

class DBManager {
private:
    Session session;

public:
    DBManager() {
        string connectionString =
            "host=localhost "
            "port=5432 "
            "user=postgres "
            "password=Student2000 "
            "client_encoding=UTF8 ";

        unique_ptr<backend::Postgres> connection{ new backend::Postgres(connectionString) };
        session.setConnection(move(connection));

        session.mapClass<Book>("book");
        session.mapClass<Publisher>("Publisher");
        session.mapClass<Shop>("Shop");
        session.mapClass<Stock>("Stock");
        session.mapClass<Sale>("Sale");

        try {
            session.createTables();
        }
        catch (...) {
        }
    }

    void addRelationships() {
        Transaction transaction{ session };

        auto publisher = make_unique<Publisher>();
        publisher->name = "Tolik";
        session.add(move(publisher));

        auto shop1 = make_unique<Shop>();
        shop1->name = "Магазин 1";
        session.add(move(shop1));

        auto shop2 = make_unique<Shop>();
        shop2->name = "Магазин 2";
        session.add(move(shop2));

        auto book1 = make_unique<Book>();
        book1->title = "Книга 1";
        book1->publisher = session.find<Publisher>().where("name = ?").bind("Tolik");
        session.add(move(book1));

        auto book2 = make_unique<Book>();
        book2->title = "Книга 2";
        book2->publisher = session.find<Publisher>().where("name = ?").bind("Tolik");
        session.add(move(book2));

        transaction.commit();
    }

    Session& getSession() {
        return session;
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);

    bool createTablesOnly = true; 

    try {
        DBManager dbManager;

        if (!createTablesOnly) {
            dbManager.addRelationships();
        }

        Session& session = dbManager.getSession();

       
        unique_ptr<Stock> stock1 = make_unique<Stock>();
       // stock1->book = book1_added;
       // stock1->shop = shop1_added;
       // session.add(move(stock1));

        //unique_ptr<Stock> stock2 = make_unique<Stock>();
       // stock2->book = book2_added;
       // stock2->shop = shop2_added;
       // session.add(move(stock2));
    }
    catch (const std::exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}
