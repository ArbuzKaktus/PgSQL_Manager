#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <vector>

class DatabaseManager;

class Employee {
private:
    std::string fullName;
    std::string birthDate;
    std::string gender;

public:
    Employee(const std::string& name, const std::string& date, const std::string& gender);
    
    std::string getFullName() const;
    std::string getBirthDate() const;
    std::string getGender() const;
    
    int calculateAge() const;
    
    void saveToDB(DatabaseManager& db);
    
    static void batchSaveToDB(DatabaseManager& db, const std::vector<Employee>& employees);
    
    void display() const;
};

#endif // EMPLOYEE_H
