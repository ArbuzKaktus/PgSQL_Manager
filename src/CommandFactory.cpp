#include "CommandFactory.h"
#include "Commands.h"
#include <iostream>

std::unique_ptr<ICommand> CommandFactory::createCommand(int mode, const std::vector<std::string>& args) {
    switch (mode) {
        case 1:
            return std::make_unique<CreateTableCommand>();
            
        case 2:
            if (args.size() < 3) {
                std::cerr << "Error: Mode 2 requires 3 arguments: <full_name> <birth_date> <gender>" << std::endl;
                std::cerr << "Example: myApp 2 \"Ivanov Petr Sergeevich\" 2009-07-12 Male" << std::endl;
                return nullptr;
            }
            return std::make_unique<InsertEmployeeCommand>(args[0], args[1], args[2]);
            
        case 3:
            return std::make_unique<DisplayEmployeesCommand>();
            
        case 4:
            return std::make_unique<FillDataCommand>();
            
        case 5:
            return std::make_unique<QueryEmployeesCommand>();
            
        case 6:
            return std::make_unique<OptimizeDatabaseCommand>();
            
        default:
            std::cerr << "Error: Invalid mode. Please use mode 1-6." << std::endl;
            return nullptr;
    }
}
