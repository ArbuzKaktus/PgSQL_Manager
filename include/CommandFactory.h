#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include "ICommand.h"
#include <memory>
#include <string>
#include <vector>

class CommandFactory {
public:
    static std::unique_ptr<ICommand> createCommand(int mode, const std::vector<std::string>& args);
};

#endif // COMMANDFACTORY_H
