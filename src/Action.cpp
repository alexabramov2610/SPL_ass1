//
// Created by alex on 11/5/18.
//

#include <iostream>
#include <vector>
#include "../include/Action.h"
#include "../include/Table.h"
#include "../include/Restaurant.h"
#include "../include/Dish.h"

extern Restaurant* backup;

// ------------------ BaseAction

BaseAction::BaseAction() :  errorMsg(""), status(ActionStatus::PENDING) {};

ActionStatus BaseAction::getStatus() const
{
    return status;
};

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
};

void BaseAction::error(std::string errorMsg)
{
    this->errorMsg = errorMsg;
    status = ActionStatus::ERROR;
};

std::string BaseAction::getErrorMsg() const
{
    if (!errorMsg.empty())
        return errorMsg;
    return "";
};



// ------------------ OpenTable

OpenTable::OpenTable(int id, std::vector<Customer *> &customersList):tableId(id),customers(customersList) {};

void OpenTable::act(Restaurant &restaurant){
    Table* tableToOpen = restaurant.getTable(tableId);
    int numOfCustomer = customers.size();
    if(tableToOpen->getCapacity() >= numOfCustomer && !tableToOpen->isOpen()) {
        tableToOpen->openTable();
        for (auto customer : customers) {
            tableToOpen->addCustomer(customer);
        }
        complete();
    }
    else{
        error("Table does not exist or is already open");
        std::cout<< getErrorMsg() << std::endl;
    }
}

std::string OpenTable::toString() const{
    std::string msg("open " + std::to_string(tableId ) + " ");
    for (auto customer: customers) {
        msg.append(customer->toString() + " ");
    }
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else if (getStatus() == ActionStatus::ERROR)
    {
        msg.append("Error: ");
        msg.append(getErrorMsg());
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
}

OpenTable::~OpenTable()
{
  for (auto customer : customers)
      delete customer;
};





// ------------------ Order

Order::Order(int id) : tableId(id) {};

void Order::act(Restaurant &restaurant)
{
    Table *tmpTable = restaurant.getTable(tableId);
    if (tmpTable != nullptr && tmpTable->isOpen())
    {
        int lastOrder = tmpTable->getOrders().size();
        tmpTable->order(restaurant.getMenu());
        std::vector<OrderPair> orders = tmpTable->getOrders();
        int numOfOrders = orders.size();
        while (lastOrder < numOfOrders)
        {
            OrderPair order = orders.at(lastOrder);
            Customer* tmpCustomer = restaurant.getTable(tableId)->getCustomer(order.first);
            std::cout << tmpCustomer->getName() << " ordered " << order.second.getName() << std::endl;
            lastOrder++;
        }
        complete();
    }
    else
    {
        this->error("Table does not exist or is not open");
        std::cout << getErrorMsg() << std::endl;
    }
};

std::string Order::toString() const
{
    std::string msg("order " + std::to_string(tableId ) + " ");

    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else if (getStatus() == ActionStatus::ERROR)
    {
        msg.append("Error: ");
        msg.append(getErrorMsg());
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};



// ------------------ MoveCustomer

MoveCustomer::MoveCustomer(int src, int dst, int customerId) : srcTable(src), dstTable(dst), id(customerId) {};

void MoveCustomer::act(Restaurant &restaurant) {
    Table* srcT = restaurant.getTable(srcTable);
    Table* dstT = restaurant.getTable(dstTable);
    Customer *c = srcT->getCustomer(id);
    if(srcT != nullptr && srcT->isOpen() && dstT != nullptr && dstT->isOpen() && c != nullptr && dstT->getCustomers().size() < dstT->getCapacity()) {
        srcT->removeCustomer(id);
        dstT->addCustomer(c);
        complete();
    }
    else{
        error("Cannot move customer") ;
        std::cout << getErrorMsg() << std::endl;
    }
};

std::string MoveCustomer::toString() const{
    std::string msg("move " + std::to_string(srcTable) + " " + std::to_string(dstTable) + " " + std::to_string(id) + " ");
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else if (getStatus() == ActionStatus::ERROR)
    {
        msg.append("Error: ");
        msg.append(getErrorMsg());
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
}



// ------------------ Close

Close::Close(int id):tableId(id) {};

void Close::act(Restaurant &restaurant) {
    Table* tableToClose = restaurant.getTable(tableId);
    if(tableToClose != nullptr && tableToClose->isOpen()){
        std::string stat = "Table " + std::to_string(tableId) + " was closed. ";
        stat.append("Bill " + std::to_string(tableToClose->getBill()) + "NIS");
        std::cout << stat << std::endl;
        tableToClose->closeTable();
        complete();
    }
    else{
        error("Table does not exist or is not open");
        std::cout<< getErrorMsg() << std::endl;
    }
};

std::string Close::toString() const{
    std::string msg("close " + std::to_string(tableId) + " ");
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else if (getStatus() == ActionStatus::ERROR)
    {
        msg.append("Error: ");
        msg.append(getErrorMsg());
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};



// ------------------ CloseAll

CloseAll::CloseAll() = default;

void CloseAll::act(Restaurant &restaurant) {
    int restCapacity = restaurant.getNumOfTables();
    if(!(restCapacity > 0))
    {
        return;
    }

    for (int i = 0; i < restCapacity; ++i) {
        Table* currTable = restaurant.getTable(i);

        if (currTable->isOpen())
        {
            std::string stat = "Table " + std::to_string(i) + " was closed.";
            stat.append(" Bill " + std::to_string(currTable->getBill()) + "NIS");
            std::cout << stat << std::endl;
            currTable->closeTable();
        }
    }
    complete();
};

std::string CloseAll::toString() const
{
    std::string msg("closeall ");
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};



// ------------------ PrintMenu

PrintMenu::PrintMenu() = default;

void PrintMenu::act(Restaurant &restaurant) {
    std::vector<Dish> menu = restaurant.getMenu();
    for (const auto &dish : menu) {
        std:: cout << dish.toString() << std::endl;
    }
    complete();
};

std::string PrintMenu::toString() const {
    std::string msg("menu " );
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};



// ------------------ PrintTableStatus

PrintTableStatus::PrintTableStatus(int id): tableId(id) {};

void PrintTableStatus::act(Restaurant &restaurant) {
    Table* tableToPrint = restaurant.getTable(tableId);
    if(tableToPrint == nullptr){
        return;
    }
    if(!tableToPrint->isOpen()){
        std::cout << "Table " << tableId << " status: " << "closed" << std::endl;
    }
    else{
        std::vector<Customer *>& tmpCustomer = tableToPrint->getCustomers();
        std::vector<OrderPair>& tmpOrders = tableToPrint->getOrders();
        std::cout << "Table " << tableId << " status: " << "open" << std::endl;
        std::cout << "Customers:" << std::endl;
        for (auto customer : tmpCustomer) {
            std::cout << customer->getId() << " " << customer->getName() << std::endl;
        }
        std::cout << "Orders:" << std::endl;
        for (auto order : tmpOrders) {
            std::cout << order.second.getName() << " " << order.second.getPrice() << "NIS " << order.first << std::endl;
        }
        std::cout << "Current Bill: " << tableToPrint->getBill() << "NIS" << std::endl;
    }
    complete();
};

std::string PrintTableStatus::toString() const
{
    std::string msg("status " + std::to_string(tableId) + " ");
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};



// ------------------ PrintActionsLog

PrintActionsLog::PrintActionsLog() = default;

void PrintActionsLog::act(Restaurant &restaurant) {
    std::vector<BaseAction*> actsLog = restaurant.getActionsLog();
    for (auto action : actsLog) {
        std::cout << action->toString() << std::endl;
    }
    complete();
};

std::string PrintActionsLog::toString() const
{
    std::string msg("log " );
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};



// ------------------ BackupRestaurant

BackupRestaurant::BackupRestaurant() = default;

void BackupRestaurant::act(Restaurant &restaurant)
{
    backup = new Restaurant(restaurant);
    complete();
};

std::string BackupRestaurant::toString() const
{
    std::string msg("backup ");
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};



// ------------------ RestoreRestaurant

RestoreResturant::RestoreResturant() = default;

void RestoreResturant::act(Restaurant &restaurant)
{
    if (backup == nullptr)
    {
        error("No backup available");
        std::cout << getErrorMsg() << std::endl;
        return;
    }
    complete();
    restaurant = *backup;

};

std::string RestoreResturant::toString() const
{
    std::string msg("restore ");
    if (getStatus() == ActionStatus::COMPLETED)
    {
        msg.append("Completed");
    }
    else if (getStatus() == ActionStatus::ERROR)
    {
        msg.append("Error: ");
        msg.append(getErrorMsg());
    }
    else
    {
        msg.append("Pending");
    }
    return msg;
};