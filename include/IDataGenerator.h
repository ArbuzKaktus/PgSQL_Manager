#ifndef IDATAGENERATOR_H
#define IDATAGENERATOR_H

#include "Employee.h"
#include <vector>

class IDataGenerator {
public:
    virtual ~IDataGenerator() = default;

    virtual std::vector<Employee> generateEmployees(int count) = 0;
};

class RandomDataGenerator : public IDataGenerator {
public:
    std::vector<Employee> generateEmployees(int count) override;
};

class TargetedDataGenerator : public IDataGenerator {
private:
    std::string gender;
    char startingLetter;
    
public:
    TargetedDataGenerator(const std::string& gender, char startingLetter);
    std::vector<Employee> generateEmployees(int count) override;
};

#endif // IDATAGENERATOR_H
