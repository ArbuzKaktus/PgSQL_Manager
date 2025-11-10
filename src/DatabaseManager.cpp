#include "DatabaseManager.h"
#include "Employee.h"
#include <iostream>
#include <sstream>

DatabaseManager::DatabaseManager(const std::string& host, const std::string& port,
                               const std::string& dbname, const std::string& user,
                               const std::string& password) : conn(nullptr) {
    std::ostringstream oss;
    oss << "host=" << host 
        << " port=" << port 
        << " dbname=" << dbname 
        << " user=" << user 
        << " password=" << password;
    connectionString = oss.str();
}

DatabaseManager::~DatabaseManager() {
    // Note: libpqxx 7.x may trigger false positive "double free" warnings
    // This is a known issue (see libpqxx #932 in libpqxx github) and can be safely ignored
    // The warning comes from pqxx::internal::demangle_type_name cleanup
    if (conn) {
        delete conn;
        conn = nullptr;
    }
}

void DatabaseManager::connect() {
    try {
        conn = new pqxx::connection(connectionString);
        if (conn->is_open()) {
            std::cout << "Successfully connected to database" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::disconnect() {
    if (conn) {
        delete conn;
        conn = nullptr;
    }
}

void DatabaseManager::createTable() {
    try {
        pqxx::work txn(*conn);
        
        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS employees (
                id SERIAL PRIMARY KEY,
                full_name VARCHAR(255) NOT NULL,
                birth_date DATE NOT NULL,
                gender VARCHAR(10) NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");
        
        txn.commit();
        std::cout << "Table 'employees' created successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error creating table: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::insertEmployee(const std::string& fullName, 
                                    const std::string& birthDate,
                                    const std::string& gender) {
    try {
        pqxx::work txn(*conn);
        
        std::ostringstream query;
        query << "INSERT INTO employees (full_name, birth_date, gender) VALUES ("
              << txn.quote(fullName) << ", "
              << txn.quote(birthDate) << ", "
              << txn.quote(gender) << ")";
        
        txn.exec(query.str());
        txn.commit();
        
        std::cout << "Employee added successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error inserting employee: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::batchInsertEmployees(const std::vector<Employee>& employees) {
    try {
        pqxx::work txn(*conn);
        
        std::ostringstream query;
        query << "INSERT INTO employees (full_name, birth_date, gender) VALUES ";
        
        for (size_t i = 0; i < employees.size(); ++i) {
            if (i > 0) query << ", ";
            query << "("
                  << txn.quote(employees[i].getFullName()) << ", "
                  << txn.quote(employees[i].getBirthDate()) << ", "
                  << txn.quote(employees[i].getGender()) << ")";
        }
        
        txn.exec(query.str());
        txn.commit();
        
        std::cout << "Batch insert completed: " << employees.size() << " employees added" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error in batch insert: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::tuple<std::string, std::string, std::string, int>> 
DatabaseManager::getAllEmployees() {
    std::vector<std::tuple<std::string, std::string, std::string, int>> result;
    
    try {
        pqxx::work txn(*conn);
        
        pqxx::result res = txn.exec(R"(
            SELECT DISTINCT ON (full_name, birth_date) 
                full_name, birth_date, gender,
                EXTRACT(YEAR FROM AGE(birth_date)) as age
            FROM employees
            ORDER BY full_name, birth_date
        )");
        
        for (const auto& row : res) {
            std::string fullName = row[0].as<std::string>();
            std::string birthDate = row[1].as<std::string>();
            std::string gender = row[2].as<std::string>();
            int age = row[3].as<int>();
            
            result.emplace_back(fullName, birthDate, gender, age);
        }
        
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving employees: " << e.what() << std::endl;
        throw;
    }
    
    return result;
}

std::vector<std::tuple<std::string, std::string, std::string, int>> 
DatabaseManager::getEmployeesByCriteria(const std::string& gender, 
                                        const std::string& lastNameStartsWith) {
    std::vector<std::tuple<std::string, std::string, std::string, int>> result;
    
    try {
        pqxx::work txn(*conn);
        
        std::ostringstream query;
        query << R"(
            SELECT full_name, birth_date, gender,
                   EXTRACT(YEAR FROM AGE(birth_date)) as age
            FROM employees
            WHERE gender = )" << txn.quote(gender) << R"(
              AND full_name LIKE )" << txn.quote(lastNameStartsWith + "%") << R"(
            ORDER BY full_name
        )";
        
        pqxx::result res = txn.exec(query.str());
        
        for (const auto& row : res) {
            std::string fullName = row[0].as<std::string>();
            std::string birthDate = row[1].as<std::string>();
            std::string gender = row[2].as<std::string>();
            int age = row[3].as<int>();
            
            result.emplace_back(fullName, birthDate, gender, age);
        }
        
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving employees by criteria: " << e.what() << std::endl;
        throw;
    }
    
    return result;
}

void DatabaseManager::createOptimizationIndex() {
    try {
        std::cout << "  Step 1: Creating partial index for Male employees with surname 'F'..." << std::endl;
        {
            pqxx::work txn(*conn);
            txn.exec(R"(
                CREATE INDEX IF NOT EXISTS idx_employees_male_f_surname 
                ON employees (full_name, birth_date)
                WHERE gender = 'Male' AND full_name >= 'F' AND full_name < 'G'
            )");
            txn.commit();
        }
        std::cout << "       Partial index created" << std::endl;
        
        std::cout << "  Step 2: Creating covering index for better performance..." << std::endl;
        {
            pqxx::work txn(*conn);
            txn.exec(R"(
                CREATE INDEX IF NOT EXISTS idx_employees_covering 
                ON employees (gender, full_name, birth_date)
                WHERE gender = 'Male'
            )");
            txn.commit();
        }
        std::cout << "       Covering index created" << std::endl;
        
        std::cout << "  Step 3: Running VACUUM ANALYZE to optimize table..." << std::endl;
        {
            pqxx::nontransaction ntxn(*conn);
            ntxn.exec("VACUUM ANALYZE employees");
        }
        std::cout << "       VACUUM ANALYZE completed" << std::endl;
        
        std::cout << "  Step 4: Increasing work_mem for better sort performance..." << std::endl;
        {
            pqxx::work txn(*conn);
            txn.exec("SET work_mem = '256MB'");
            txn.commit();
        }
        std::cout << "       work_mem increased to 256MB" << std::endl;
        
        std::cout << "\nOptimization completed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during optimization: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::dropIndex() {
    try {
        pqxx::nontransaction txn(*conn);
        
        txn.exec("DROP INDEX IF EXISTS idx_employees_male_f_surname");
        txn.exec("DROP INDEX IF EXISTS idx_employees_covering");
        txn.exec("DROP INDEX IF EXISTS idx_employees_gender_name");
        
        std::cout << "All optimization indexes dropped successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error dropping indexes: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::clearCache() {
    try {
        pqxx::work txn(*conn);
        txn.exec("DISCARD ALL");
        txn.commit();
    } catch (const std::exception& e) { }
}

void DatabaseManager::explainQuery(const std::string& gender, const std::string& lastNameStartsWith) {
    try {
        pqxx::work txn(*conn);
        
        std::ostringstream query;
        query << "EXPLAIN ANALYZE SELECT DISTINCT ON (full_name, birth_date) "
              << "full_name, birth_date, gender, "
              << "EXTRACT(YEAR FROM AGE(birth_date)) AS age "
              << "FROM employees "
              << "WHERE gender = " << txn.quote(gender) << " "
              << "AND full_name LIKE " << txn.quote(lastNameStartsWith + "%") << " "
              << "ORDER BY full_name, birth_date";
        
        pqxx::result res = txn.exec(query.str());
        
        std::cout << "\n";
        for (auto row : res) {
            std::cout << "  " << row[0].c_str() << std::endl;
        }
        std::cout << "\n";
        
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "Error explaining query: " << e.what() << std::endl;
        throw;
    }
}

pqxx::connection* DatabaseManager::getConnection() {
    return conn;
}
