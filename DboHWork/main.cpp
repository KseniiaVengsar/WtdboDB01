#include <iostream>
#include <wt/dbo/dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <Windows.h>

#pragma execution_character_set ("utf-8")


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
    //у него есть коллекция из книг
    collection<ptr<Book>>books;
    //Объявление ORM-класса/создание таблитцы
    template<typename Action>
    void persist(Action& a)
    {
        field(a, name, "name");
        hasMany(a, books, ManyToOne, "publisher");
    }
};

class Book {
public:
    string title;
    //ссылка на паблишер
    ptr<Publisher> publisher;
    collection<ptr<Stock >> stocks;

    template<class Action>
    void persist(Action& a)
    {
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

    template<typename Action>
    void persist(Action& a)
    {
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

    template<typename Action>
    void persist(Action& a)
    {
        field(a, count, "count");
        field(a, price, "price");
        field(a, date_sale, "date_sale");
        belongsTo(a, stock, "stock");
    }
};

class Shop {
public:
    string name;
    collection<ptr<Stock >> stocks;

    template<typename Action>
    void persist(Action& a)
    {
        field(a, name, "name");
        hasMany(a, stocks, ManyToOne, "shop");
    }
};
int main() {

    setlocale(LC_ALL, "Russian");

    try {

        string connectionString =
            //не забывай пробелы
            "host=localhost "
            "port=5432 "
            "user=postgres "
            "password=Pizda1992 "
            "client_encoding=UTF8 ";


        unique_ptr<backend::Postgres>connection{ new backend::Postgres(connectionString) };
        //auto connection = std::make_unique<Wt::Dbo::backend::Postgres>(connectionString);
        //auto postgres = std::make_unique<backend::Postgres>(connectionString);


        Session session;

        session.setConnection(move(connection));

        //каждый класс нужно обязательно зарегистрировать
        session.mapClass<Book>("book");
        session.mapClass<Publisher>("Publisher");//название в БД
        session.mapClass<Shop>("Shop");
        session.mapClass<Stock>("Stock");
        session.mapClass<Sale>("Sale");
        session.mapClass<Shop>("Shop");



        //добавляем метод который будет созд таблицу
        session.createTables();

        Transaction transaction{ session };

        std::unique_ptr<Publisher> publisher{ new Publisher() };
        publisher->name = "Joe";
        session.add(std::move(publisher));
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }


    return 0;

}