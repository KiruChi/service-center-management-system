#ifndef DATABASE_H
#define DATABASE_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <QDebug>
#include <QString>

class DataBase {
public:
    DataBase();
    void connect();

    // ---- User ----
    bool loginUser(QString username, QString password);
    QString getUserName() const {return currentUser;}
    QString getUserRole() const {return role;}

    // ---- Orders ----
    bsoncxx::document::value findOrder(QString orderId);
    std::vector<bsoncxx::document::value> findOrders(QString searchParam);
    std::vector<bsoncxx::document::value> findAllOrders();
    bool deleteOrder(QString orderId);
    bool insertOrder(bsoncxx::document::value& order);
    bool updateOrder(QString id, bsoncxx::document::value& updateDoc);
    std::vector<double> getTotalStatistic(const std::string& date1 = "", const std::string& date2 = "");
    std::map<QString, int> getOrdersByMonth(std::string& date1, std::string& date2);
    std::map<QString, int> getTechniciansStats(std::string& date1, std::string& date2);
    std::map<QString, int> getReceptionistsStats(std::string& date1, std::string& date2);

private:
    mongocxx::client* client;
    mongocxx::database db;
    mongocxx::collection collection;

    QString currentUser;
    QString role;
};

#endif // DATABASE_H
