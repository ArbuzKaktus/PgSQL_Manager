#ifndef COMMANDS_H
#define COMMANDS_H

#include "ICommand.h"
#include "DatabaseManager.h"
#include "Employee.h"
#include <string>

class CreateTableCommand : public ICommand {
public:
    void execute(DatabaseManager& dbManager) override;
    const char* getDescription() const override { return "Create employee table"; }
};

class InsertEmployeeCommand : public ICommand {
private:
    std::string fullName;
    std::string birthDate;
    std::string gender;
    
public:
    InsertEmployeeCommand(const std::string& name, const std::string& date, const std::string& gender);
    void execute(DatabaseManager& dbManager) override;
    const char* getDescription() const override { return "Insert employee"; }
};

class DisplayEmployeesCommand : public ICommand {
public:
    void execute(DatabaseManager& dbManager) override;
    const char* getDescription() const override { return "Display all employees"; }
};

class FillDataCommand : public ICommand {
public:
    void execute(DatabaseManager& dbManager) override;
    const char* getDescription() const override { return "Fill database with test data"; }
};

class QueryEmployeesCommand : public ICommand {
public:
    void execute(DatabaseManager& dbManager) override;
    const char* getDescription() const override { return "Query employees by criteria"; }
};

class OptimizeDatabaseCommand : public ICommand {
public:
    void execute(DatabaseManager& dbManager) override;
    const char* getDescription() const override { return "Optimize database"; }
};

#endif // COMMANDS_H
