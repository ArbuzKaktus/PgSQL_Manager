#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <vector>

class DatabaseManager;
class ICommand;

class Application {
private:
    std::string host, port, dbname, user, password;
    
    void displayUsage() const;

    bool parseArguments(int argc, char* argv[], int& mode, std::vector<std::string>& args) const;

public:
    Application(const std::string& host, const std::string& port,
                const std::string& dbname, const std::string& user,
                const std::string& password);

    int run(int argc, char* argv[]);
};

#endif // APPLICATION_H
