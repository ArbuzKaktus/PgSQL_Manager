#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <vector>
#include <pqxx/pqxx>

class Employee;

class DatabaseManager {
private:
    pqxx::connection* conn;
    std::string connectionString;

public:
    DatabaseManager(const std::string& host, const std::string& port, 
                   const std::string& dbname, const std::string& user, 
                   const std::string& password);
    
    ~DatabaseManager();
    
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    DatabaseManager(DatabaseManager&&) = delete;
    DatabaseManager& operator=(DatabaseManager&&) = delete;
    
    void connect();
    void disconnect();
    
    void createTable();
    
    void insertEmployee(const std::string& fullName, const std::string& birthDate, 
                       const std::string& gender);
    
    void batchInsertEmployees(const std::vector<Employee>& employees);
    
    std::vector<std::tuple<std::string, std::string, std::string, int>> getAllEmployees();
    
    std::vector<std::tuple<std::string, std::string, std::string, int>> 
        getEmployeesByCriteria(const std::string& gender, const std::string& lastNameStartsWith);
    
    void createOptimizationIndex();
    
    void dropIndex();
    
    void clearCache();
    
    void explainQuery(const std::string& gender, const std::string& lastNameStartsWith);
    
    pqxx::connection* getConnection();
};

#endif // DATABASEMANAGER_H
