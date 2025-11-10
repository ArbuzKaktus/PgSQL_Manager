#include "Commands.h"
#include "IDataGenerator.h"
#include <iostream>
#include <chrono>
#include <iomanip>


void CreateTableCommand::execute(DatabaseManager& dbManager) {
    std::cout << "Creating employee table..." << std::endl;
    dbManager.createTable();
    std::cout << "Table created successfully!" << std::endl;
}
    
InsertEmployeeCommand::InsertEmployeeCommand(const std::string& name, const std::string& date, const std::string& gender)
    : fullName(name), birthDate(date), gender(gender) {}

void InsertEmployeeCommand::execute(DatabaseManager& dbManager) {
    Employee emp(fullName, birthDate, gender);
    emp.saveToDB(dbManager);
    std::cout << "Employee added successfully!" << std::endl;
    std::cout << "Full Name: " << fullName << std::endl;
    std::cout << "Birth Date: " << birthDate << std::endl;
    std::cout << "Gender: " << gender << std::endl;
    std::cout << "Age: " << emp.calculateAge() << " years" << std::endl;
}

void DisplayEmployeesCommand::execute(DatabaseManager& dbManager) {
    std::cout << "Displaying all employees (unique by Full Name + Birth Date, sorted by Full Name):" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    auto employees = dbManager.getAllEmployees();
    
    if (employees.empty()) {
        std::cout << "No employees found in database." << std::endl;
        return;
    }
    
    std::cout << "Total employees: " << employees.size() << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    for (const auto& emp : employees) {
        std::cout << "Full Name: " << std::get<0>(emp) << std::endl;
        std::cout << "Birth Date: " << std::get<1>(emp) << std::endl;
        std::cout << "Gender: " << std::get<2>(emp) << std::endl;
        std::cout << "Age: " << std::get<3>(emp) << " years" << std::endl;
        std::cout << std::string(100, '-') << std::endl;
    }
}

void FillDataCommand::execute(DatabaseManager& dbManager) {
    std::cout << "Filling database with test data..." << std::endl;
    
    std::cout << "Generating 1,000,000 random employees..." << std::endl;
    RandomDataGenerator randomGen;
    auto randomEmployees = randomGen.generateEmployees(1000000);
    
    std::cout << "Inserting random employees into database..." << std::endl;
    Employee::batchSaveToDB(dbManager, randomEmployees);
    std::cout << "Random employees inserted successfully!" << std::endl;
    
    std::cout << "Generating 100 targeted employees (Male, surname starts with 'F')..." << std::endl;
    TargetedDataGenerator targetedGen("Male", 'F');
    auto targetedEmployees = targetedGen.generateEmployees(100);
    
    std::cout << "Inserting targeted employees into database..." << std::endl;
    Employee::batchSaveToDB(dbManager, targetedEmployees);
    std::cout << "Targeted employees inserted successfully!" << std::endl;
    
    std::cout << "Database filled with 1,000,100 employees!" << std::endl;
}

void QueryEmployeesCommand::execute(DatabaseManager& dbManager) {
    std::cout << "Querying employees: Gender = Male, Surname starts with 'F'" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    auto employees = dbManager.getEmployeesByCriteria("Male", "F");
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Query completed in " << duration.count() << " ms" << std::endl;
    std::cout << "Found " << employees.size() << " employees matching criteria" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    int displayCount = std::min(10, static_cast<int>(employees.size()));
    if (displayCount > 0) {
        std::cout << "Displaying first " << displayCount << " results:" << std::endl;
        for (int i = 0; i < displayCount; ++i) {
            std::cout << "Full Name: " << std::get<0>(employees[i]) << std::endl;
            std::cout << "Birth Date: " << std::get<1>(employees[i]) << std::endl;
            std::cout << "Gender: " << std::get<2>(employees[i]) << std::endl;
            std::cout << "Age: " << std::get<3>(employees[i]) << " years" << std::endl;
            std::cout << std::string(100, '-') << std::endl;
        }
    }
}

void OptimizeDatabaseCommand::execute(DatabaseManager& dbManager) {
    std::cout << "Database Optimization Process" << std::endl;
    std::cout << "Query criteria: Gender = Male, Surname starts with 'F'" << std::endl;
    std::cout << std::string(100, '=') << std::endl;
    
    std::cout << "\nStep 0: Removing any existing optimization indexes..." << std::endl;
    dbManager.dropIndex();
    std::cout << std::string(100, '-') << std::endl;
    
    std::cout << "\nMeasuring query time BEFORE optimization..." << std::endl;
    auto start1 = std::chrono::high_resolution_clock::now();
    auto employees1 = dbManager.getEmployeesByCriteria("Male", "F");
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
    
    std::cout << "Time BEFORE optimization: " << duration1.count() << " ms" << std::endl;
    std::cout << "Found " << employees1.size() << " employees" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    std::cout << "\nApplying optimizations:" << std::endl;
    dbManager.createOptimizationIndex();
    std::cout << std::string(100, '-') << std::endl;
    
    std::cout << "\nMeasuring query time AFTER optimization..." << std::endl;
    auto start2 = std::chrono::high_resolution_clock::now();
    auto employees2 = dbManager.getEmployeesByCriteria("Male", "F");
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
    
    std::cout << "Time AFTER optimization: " << duration2.count() << " ms" << std::endl;
    std::cout << "Found " << employees2.size() << " employees" << std::endl;
    std::cout << std::string(100, '=') << std::endl;
    
    double improvement = 0.0;
    long long timeSaved = duration1.count() - duration2.count();
    if (duration1.count() > 0) {
        improvement = (timeSaved * 100.0) / duration1.count();
    }
    
    std::cout << "\n*** PERFORMANCE RESULTS ***" << std::endl;
    std::cout << "Time BEFORE optimization: " << duration1.count() << " ms" << std::endl;
    std::cout << "Time AFTER optimization:  " << duration2.count() << " ms" << std::endl;
    std::cout << "Performance improvement: " << std::fixed << std::setprecision(2) << improvement << "%" << std::endl;
    std::cout << "Time saved: " << timeSaved << " ms" << std::endl;
    std::cout << std::string(100, '=') << std::endl;
    
    std::cout << "\nOptimization techniques applied:" << std::endl;
    std::cout << "  1. Partial Index: Index only for Male employees with surname 'F'" << std::endl;
    std::cout << "  2. Covering Index: Includes all query columns to avoid table lookups" << std::endl;
    std::cout << "  3. VACUUM ANALYZE: Reclaimed storage and updated statistics" << std::endl;
    std::cout << "  4. Increased work_mem: Better memory for sorting operations (256MB)" << std::endl;
}

