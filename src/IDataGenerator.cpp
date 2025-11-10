#include "IDataGenerator.h"
#include <random>
#include <ctime>

namespace {
    std::string generateRandomName(char startingLetter = '\0') {
        static const char* surnames[] = {
            "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis",
            "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez", "Wilson", "Anderson",
            "Thomas", "Taylor", "Moore", "Jackson", "Martin", "Lee", "Walker", "Hall",
            "Foster", "Freeman", "Fletcher", "Fisher", "Fitzgerald", "Flanagan", "Floyd",
            "Franklin", "Francis", "Frost", "Fuller", "Ferguson", "Fields", "Flores"
        };
        
        static const char* firstNames[] = {
            "James", "John", "Robert", "Michael", "William", "David", "Richard", "Joseph",
            "Thomas", "Charles", "Mary", "Patricia", "Jennifer", "Linda", "Elizabeth",
            "Barbara", "Susan", "Jessica", "Sarah", "Karen", "Nancy", "Margaret", "Lisa",
            "Betty", "Dorothy", "Sandra", "Ashley", "Kimberly", "Donna", "Emily"
        };
        
        static const char* middleNames[] = {
            "Alan", "Andrew", "Anthony", "Benjamin", "Brian", "Christopher", "Daniel",
            "Edward", "Frank", "George", "Henry", "Ivan", "Jack", "Kevin", "Lawrence",
            "Mark", "Nathan", "Oliver", "Paul", "Peter", "Quinn", "Raymond", "Samuel",
            "Timothy", "Victor", "Walter", "Xavier", "Zachary", "Ann", "Marie"
        };
        
        static std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));
        
        std::string surname;
        if (startingLetter != '\0') {
            std::vector<std::string> matchingSurnames;
            for (const auto& s : surnames) {
                if (s[0] == startingLetter) {
                    matchingSurnames.push_back(s);
                }
            }
            if (!matchingSurnames.empty()) {
                std::uniform_int_distribution<> dist(0, matchingSurnames.size() - 1);
                surname = matchingSurnames[dist(gen)];
            } else {
                surname = "Foster";
            }
        } else {
            std::uniform_int_distribution<> surnamesDist(0, sizeof(surnames)/sizeof(surnames[0]) - 1);
            surname = surnames[surnamesDist(gen)];
        }
        
        std::uniform_int_distribution<> firstNamesDist(0, sizeof(firstNames)/sizeof(firstNames[0]) - 1);
        std::uniform_int_distribution<> middleNamesDist(0, sizeof(middleNames)/sizeof(middleNames[0]) - 1);
        
        return surname + " " + firstNames[firstNamesDist(gen)] + " " + middleNames[middleNamesDist(gen)];
    }
    
    std::string generateRandomDate() {
        static std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));
        std::uniform_int_distribution<> yearDist(1950, 2005);
        std::uniform_int_distribution<> monthDist(1, 12);
        std::uniform_int_distribution<> dayDist(1, 28);
        
        int year = yearDist(gen);
        int month = monthDist(gen);
        int day = dayDist(gen);
        
        char buffer[11];
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
        return std::string(buffer);
    }
    
    std::string generateRandomGender() {
        static std::mt19937 gen(static_cast<unsigned>(std::time(nullptr)));
        static std::uniform_int_distribution<> dist(0, 1);
        return (dist(gen) == 0) ? "Male" : "Female";
    }
}

std::vector<Employee> RandomDataGenerator::generateEmployees(int count) {
    std::vector<Employee> employees;
    employees.reserve(count);
    
    for (int i = 0; i < count; ++i) {
        employees.emplace_back(
            generateRandomName(),
            generateRandomDate(),
            generateRandomGender()
        );
    }
    
    return employees;
}

TargetedDataGenerator::TargetedDataGenerator(const std::string& gender, char startingLetter)
    : gender(gender), startingLetter(startingLetter) {}

std::vector<Employee> TargetedDataGenerator::generateEmployees(int count) {
    std::vector<Employee> employees;
    employees.reserve(count);
    
    for (int i = 0; i < count; ++i) {
        employees.emplace_back(
            generateRandomName(startingLetter),
            generateRandomDate(),
            gender
        );
    }
    
    return employees;
}
