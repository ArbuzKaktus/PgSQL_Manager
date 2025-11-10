#ifndef ICOMMAND_H
#define ICOMMAND_H


class DatabaseManager;

class ICommand {
public:
    virtual ~ICommand() = default;
    
    virtual void execute(DatabaseManager& dbManager) = 0;
    
    virtual const char* getDescription() const = 0;
};

#endif // ICOMMAND_H
