#include "Application.h"
#include "DatabaseManager.h"
#include "CommandFactory.h"
#include "ICommand.h"

#include <iostream>
#include <string>
#include <vector>

Application::Application(const std::string& host_, const std::string& port_,
                         const std::string& dbname_, const std::string& user_,
                         const std::string& password_)
    : host(host_), port(port_), dbname(dbname_), user(user_), password(password_) {}

void Application::displayUsage() const {
    std::cout << "Employee Management System - Usage:" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  1 - Create employee table" << std::endl;
    std::cout << "      Example: ./myApp 1" << std::endl;
    std::cout << std::endl;
    std::cout << "  2 - Insert employee" << std::endl;
    std::cout << "      Example: ./myApp 2 \"Ivanov Petr Sergeevich\" 2009-07-12 Male" << std::endl;
    std::cout << std::endl;
    std::cout << "  3 - Display all employees (unique by name+date, sorted)" << std::endl;
    std::cout << "      Example: ./myApp 3" << std::endl;
    std::cout << std::endl;
    std::cout << "  4 - Fill database with 1,000,100 test records" << std::endl;
    std::cout << "      Example: ./myApp 4" << std::endl;
    std::cout << std::endl;
    std::cout << "  5 - Query males with last name starting with 'F' (with timing)" << std::endl;
    std::cout << "      Example: ./myApp 5" << std::endl;
    std::cout << std::endl;
    std::cout << "  6 - Optimize database and measure improvement" << std::endl;
    std::cout << "      Example: ./myApp 6" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

bool Application::parseArguments(int argc, char* argv[], int& mode, std::vector<std::string>& args) const {
    if (argc < 2) {
        return false;
    }
    
    try {
        mode = std::stoi(argv[1]);
    } catch (...) {
        std::cerr << "Error: Invalid mode number" << std::endl;
        return false;
    }
    
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    return true;
}

int Application::run(int argc, char* argv[]) {
    int mode;
    std::vector<std::string> args;
    
    if (!parseArguments(argc, argv, mode, args)) {
        displayUsage();
        return 1;
    }
    
    try {
        DatabaseManager db(host, port, dbname, user, password);
        db.connect();
        
        auto command = CommandFactory::createCommand(mode, args);
        
        if (!command) {
            std::cerr << "Failed to create command for mode " << mode << std::endl;
            displayUsage();
            return 1;
        }
        
        std::cout << "Executing: " << command->getDescription() << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        command->execute(db);
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Command completed successfully!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}


