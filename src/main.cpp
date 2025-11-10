#include "Application.h"
#include "config.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
    
        Application app(
            Config::DB_HOST,
            Config::DB_PORT,
            Config::DB_NAME,
            Config::DB_USER,
            Config::DB_PASSWORD
        );
        
        return app.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
