#include "database.h"

static mongocxx::instance inst{};

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

DataBase::DataBase()
{
    connect(); // Підключення до MongoDB при створенні об'єкта
}

// Підключення до MongoDB
void DataBase::connect()
{
    try
    {
        client = new mongocxx::client(mongocxx::uri("mongodb://localhost:27017"));
        db = (*client)["ServiceCenter"];
        collection = db["Orders"];
        qDebug() << "MongoDB connected!";
    }
    catch (const std::exception& ex)
    {
        qDebug() << "MongoDB connection error:" << ex.what();
        throw;
    }
}

// Авторизація користувача
bool DataBase::loginUser(QString username, QString password)
{
    auto users = db["Users"];

    try
    {
        auto doc = users.find_one(document{} << "name" << username.toStdString() << "password" << password.toStdString() << finalize);
        if (doc)
        {
            currentUser = username;
            auto view = doc->view();
            role = QString::fromStdString(std::string(view["role"].get_string().value.data(), view["role"].get_string().value.size()));
            return true;
        }
    }
    catch (const std::exception& ex)
    {
        qDebug() << "Login error" << ex.what();
    }
    return false;
}

// Додавання нової заявки
bool DataBase::insertOrder(bsoncxx::document::value& order)
{
    auto result = collection.insert_one(order.view());
    if (result) return true;
    else return false;
}

// Оновлення заявки за ID
bool DataBase::updateOrder(QString orderId, bsoncxx::document::value& updateDoc)
{
    auto result = collection.update_one(document{} << "_id" << bsoncxx::oid(orderId.toStdString()) << finalize,
        document{} << "$set" << updateDoc.view() << finalize);
    return result && result->modified_count() > 0;
}

// Пошук однієї заявки за ID
bsoncxx::document::value DataBase::findOrder(QString orderId)
{
    auto result = collection.find_one(document{} << "_id" << bsoncxx::oid(orderId.toStdString()) <<finalize);
    if (!result)
    {
        throw std::runtime_error("Order not found");
    }
    return bsoncxx::document::value(result->view());;
}

// Пошук заявок за параметром (ID або текстові поля)
std::vector<bsoncxx::document::value> DataBase::findOrders(QString searchParam)
{
    std::vector<bsoncxx::document::value> orders;
    document filter_builder;
    auto or_array = filter_builder << "$or" << open_array;
    try
    {
        bsoncxx::oid oid(searchParam.toStdString());
        or_array << open_document << "_id" << oid << close_document;
    }
    catch (...){}

    std::vector<std::string> fields = {"state","name","phone","deviceBrand","deviceModel","providedDevices",
        "issueDescription","diagnosticInfo","worksDone","dateReceived","dateStarted","dateCompleted","receivedByUser","repairByUser"};

    for (auto& field : fields)
    {
        or_array << open_document << field << searchParam.toStdString() << close_document;
    }
    auto filter = or_array << close_array << finalize;
    auto find = collection.find(filter.view());
    for (auto&& doc : find)
    {
        orders.emplace_back(bsoncxx::document::value(doc));
    }
    if (orders.empty())
    {
        throw std::runtime_error("Orders not found");
    }
    return orders;
}

// Отримання всіх заявок
std::vector<bsoncxx::document::value> DataBase::findAllOrders()
{
    std::vector<bsoncxx::document::value> orders;
    auto find = collection.find({});
    for (auto&& doc : find)
    {
        orders.emplace_back(bsoncxx::document::value(doc));
    }
    return orders;
}

// Видалення заявки за ID
bool DataBase::deleteOrder(QString orderId)
{
    auto result = collection.delete_one(document{} << "_id" << bsoncxx::oid(orderId.toStdString()) << finalize);
    if (result->deleted_count() > 0) return true;
    else return false;
}

// Отримання загальної статистики
std::vector<double> DataBase::getTotalStatistic(const std::string& date1, const std::string& date2)
{
    std::vector<double> stats(4, 0); // [total, active, completed, earned]

    auto filterCompleted = document{};
    if(date1 == "" && date2 == "")
    {
        auto filterTotal = document{} << finalize;
        stats[0] = collection.count_documents(filterTotal.view());
        auto filterActive = document{} << "state" << open_document << "$ne" << "Issued" << close_document << finalize;
        stats[1] = collection.count_documents(filterActive.view());
        auto filterCompleted = document{} << "state" << "Issued" << finalize;
        stats[2] = collection.count_documents(filterCompleted.view());
    }
    else
    {
        auto filterTotal = document{} << "dateReceived" << open_document << "$gte" << date1 << "$lte" << date2 << close_document << finalize;
        stats[0] = collection.count_documents(filterTotal.view());
        auto filterActive = document{} << "dateReceived" << open_document << "$gte" << date1 << "$lte" << date2 << close_document
                            << "state" << open_document << "$ne" << "Issued" << close_document<< finalize;
        stats[1] = collection.count_documents(filterActive.view());
        auto filterCompleted = document{} << "dateReceived" << open_document<< "$gte" << date1 << "$lte" << date2 << close_document
                            << "state" << "Issued" << finalize;
        stats[2] = collection.count_documents(filterCompleted.view());
    }
    mongocxx::pipeline pipeline;
    pipeline.match(filterCompleted.view());

    bsoncxx::builder::stream::document group_builder;
    group_builder << "_id" << bsoncxx::types::b_null{} << "totalEarned" << open_document << "$sum" << "$priceFinal" << close_document;
    pipeline.group(group_builder.view());

    double earned = 0;
    auto find = collection.aggregate(pipeline);

    for (auto&& doc : find)
    {
        if (doc["totalEarned"].type() == bsoncxx::type::k_double)
            earned = doc["totalEarned"].get_double().value;
        else if (doc["totalEarned"].type() == bsoncxx::type::k_int64)
            earned = static_cast<double>(doc["totalEarned"].get_int64().value);
    }
    stats[3] = earned;
    return stats;
}

// Отримання кількості заявок по місяцях за вибраний період
std::map<QString, int> DataBase::getOrdersByMonth(std::string& date1, std::string& date2)
{
    std::map<QString, int> result;
    mongocxx::pipeline pipeline;

    // 1. Фільтрація заявок за датою прийняття
    bsoncxx::builder::stream::document matchBuilder;
    matchBuilder << "dateReceived" << open_document << "$gte" << date1 << "$lte" << date2 << close_document;
    pipeline.match(matchBuilder.view());

    // 2. Групування заявок за місяцем
    bsoncxx::builder::stream::document groupBuilder;
    groupBuilder << "_id" << open_document << "month" << open_document << "$substr" << bsoncxx::builder::stream::open_array
    << "$dateReceived" << 0 << 7 << bsoncxx::builder::stream::close_array << close_document << close_document
    << "count" << open_document << "$sum" << 1 << close_document;
    pipeline.group(groupBuilder.view());

    // SORT по місяцях
    pipeline.sort(document{} << "_id.month" << 1 << bsoncxx::builder::stream::finalize);

    // Виконання
    auto find = collection.aggregate(pipeline);

    for (auto&& doc : find)
    {
        std::string month = std::string(doc["_id"]["month"].get_string().value);
        int count = doc["count"].get_int32().value;
        result[QString::fromStdString(month)] = count;
    }
    return result;
}

// Отримання статистики по техніках (кількість завершених ремонтів)
std::map<QString, int> DataBase::getTechniciansStats(std::string& date1, std::string& date2)
{
    std::map<QString, int> result;
    mongocxx::pipeline pipeline;

    // 1. Обираємо лише завершені заявки з діапазону
    bsoncxx::builder::stream::document matchBuilder;
    matchBuilder << "dateCompleted" << open_document << "$gte" << date1 << "$lte" << date2 << close_document << "state" << "Issued";
    pipeline.match(matchBuilder.view());

    // 2. GROUP BY technician
    bsoncxx::builder::stream::document groupBuilder;
    groupBuilder << "_id" << "$repairByUser" << "count" << open_document << "$sum" << 1<< close_document;
    pipeline.group(groupBuilder.view());

    // 3. Сортування по кількості
    pipeline.sort(document{} << "count" << -1 << finalize);

    // 4. Виконання
    auto find = collection.aggregate(pipeline);
    for (auto&& doc : find)
    {
        std::string technician = std::string(doc["_id"].get_string().value);
        int count = doc["count"].get_int32().value;
        result[QString::fromStdString(technician)] = count;
    }
    return result;
}

// Отримання статистики по приймальниках (кількість прийнятих заявок)
std::map<QString, int> DataBase::getReceptionistsStats(std::string& date1,std::string& date2)
{
    std::map<QString, int> result;

    mongocxx::pipeline pipeline;

    // Обираємо ЗАЯВКИ, прийняті у діапазоні дат
    bsoncxx::builder::stream::document matchBuilder;
    matchBuilder << "dateReceived" << open_document << "$gte" << date1 << "$lte" << date2 << close_document;
    pipeline.match(matchBuilder.view());

    // GROUP BY receivedByUser (імена приймальників)
    bsoncxx::builder::stream::document groupBuilder;
    groupBuilder << "_id" << "$receivedByUser" << "count" << open_document << "$sum" << 1 << close_document;
    pipeline.group(groupBuilder.view());

    // Сортуємо за спаданням
    pipeline.sort(document{} << "count" << -1 << finalize);

    // Виконуємо
    auto find = collection.aggregate(pipeline);
    for (auto&& doc : find)
    {
        std::string receptionist = std::string(doc["_id"].get_string().value);
        int count = doc["count"].get_int32().value;
        result[QString::fromStdString(receptionist)] = count;
    }
    return result;
}

