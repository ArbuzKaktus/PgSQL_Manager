#include "Employee.h"
#include "DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

Employee::Employee(const std::string& name, const std::string& date, const std::string& gen)
    : fullName(name), birthDate(date), gender(gen) {}

std::string Employee::getFullName() const {
    return fullName;
}

std::string Employee::getBirthDate() const {
    return birthDate;
}

std::string Employee::getGender() const {
    return gender;
}

int Employee::calculateAge() const {
    std::tm birthTm = {};
    std::istringstream ss(birthDate);
    ss >> std::get_time(&birthTm, "%Y-%m-%d");
    
    if (ss.fail()) {
        std::cerr << "Failed to parse birth date: " << birthDate << std::endl;
        return 0;
    }
    
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* nowTm = std::localtime(&now_c);
    
    int age = nowTm->tm_year - birthTm.tm_year;
    
    if (nowTm->tm_mon < birthTm.tm_mon ||
        (nowTm->tm_mon == birthTm.tm_mon && nowTm->tm_mday < birthTm.tm_mday)) {
        age--;
    }
    
    return age;
}

void Employee::saveToDB(DatabaseManager& db) {
    db.insertEmployee(fullName, birthDate, gender);
}

void Employee::batchSaveToDB(DatabaseManager& db, const std::vector<Employee>& employees) {
    db.batchInsertEmployees(employees);
}

void Employee::display() const {
    std::cout << "Name: " << fullName 
              << ", Birth Date: " << birthDate 
              << ", Gender: " << gender 
              << ", Age: " << calculateAge() << " years" << std::endl;
}
