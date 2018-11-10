#ifndef RESTAURANT_H_
#define RESTAURANT_H_

#include <vector>
#include <string>
#include "Dish.h"
#include "Table.h"
#include "Action.h"


class Restaurant {
public:
    Restaurant();
    Restaurant(const std::string &configFilePath);
    void start();
    int getNumOfTables() const;
    Table* getTable(int ind);
    const std::vector<BaseAction*>& getActionsLog() const; // Return a reference to the history of actions
    std::vector<Dish>& getMenu();
    ~Restaurant();
    Restaurant& operator= (const Restaurant& other);

private:
    bool open;
    std::vector<Table*> tables;
    std::vector<Dish> menu;
    std::vector<BaseAction*> actionsLog;

    const std::vector<std::string> parseInput(std::string& str);
    void readFile(const std::string &configFilePath);
    int parseLine(const std::string &currLine, int caseNumber);
    DishType parseDishType(const std::string&);
    void insertNewDish(const std::string&);

};

#endif