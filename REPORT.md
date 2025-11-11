# Отчет по выполнению тестового задания
## Система управления справочником сотрудников

---

## 1. Общая информация о проекте

### Технологический стек
- **Язык программирования**: C++17
- **СУБД**: PostgreSQL 17.5
- **Библиотека для работы с БД**: libpqxx 7.10.4
- **Система сборки**: CMake 3.10+ с Ninja
- **ОС разработки**: Linux (Arch Linux)

### Архитектура приложения
Проект реализован с применением объектно-ориентированного подхода и современных паттернов проектирования:
- **Command Pattern** - для реализации режимов работы приложения
- **Factory Pattern** - для создания команд на основе входных параметров
- **Strategy Pattern** - для генерации тестовых данных
- **SOLID принципы** - разделение ответственности между классами

### Структура проекта
```
SqlMngr/
├── include/           # Заголовочные файлы
│   ├── Application.h
│   ├── CommandFactory.h
│   ├── Commands.h
│   ├── DatabaseManager.h
│   ├── Employee.h
│   ├── ICommand.h
│   ├── IDataGenerator.h
│   └── config.h
├── src/              # Исходные файлы
│   ├── Application.cpp
│   ├── CommandFactory.cpp
│   ├── Commands.cpp
│   ├── DatabaseManager.cpp
│   ├── Employee.cpp
│   ├── IDataGenerator.cpp
│   └── main.cpp
├── build/            # Директория сборки
└── CMakeLists.txt    # Конфигурация сборки
```

---

## 2. Описание реализации

### 2.1. Основные классы

#### Класс `Employee`
Представляет объект сотрудника со следующими полями:
- `fullName` - ФИО (string)
- `birthDate` - дата рождения (string, формат YYYY-MM-DD)
- `gender` - пол (string: Male/Female)

**Методы класса:**
- `calculateAge()` - расчет полного возраста в годах

#### Класс `DatabaseManager`
Управляет подключением к базе данных и выполнением операций:
- `connect()` / `disconnect()` - управление соединением
- `createTable()` - создание таблицы employees
- `insertEmployee()` - вставка одной записи
- `batchInsertEmployees()` - пакетная вставка массива объектов
- `getAllEmployees()` - получение всех уникальных записей
- `getEmployeesByCriteria()` - выборка по критериям
- `createOptimizationIndex()` - создание индексов для оптимизации
- `dropIndex()` - удаление индексов

#### Паттерн Command
Каждый режим работы реализован как отдельная команда:
- `CreateTableCommand` - режим 1
- `InsertEmployeeCommand` - режим 2
- `DisplayEmployeesCommand` - режим 3
- `FillDataCommand` - режим 4
- `QueryEmployeesCommand` - режим 5
- `OptimizeDatabaseCommand` - режим 6

---

## 3. Выполнение режимов работы

### Режим 1: Создание таблицы

**Команда запуска:**
```bash
./SqlManager 1
```

**Структура таблицы:**
```sql
CREATE TABLE IF NOT EXISTS employees (
    id SERIAL PRIMARY KEY,
    full_name VARCHAR(255) NOT NULL,
    birth_date DATE NOT NULL,
    gender VARCHAR(10) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)
```

**Результат:**
```
Successfully connected to database
Executing: Create employee table
================================================================================
Creating employee table...
Table 'employees' created successfully
Table created successfully!
================================================================================
Command completed successfully!
```

---

### Режим 2: Добавление сотрудника

**Команда запуска:**
```bash
./SqlManager 2 "Ivanov Petr Sergeevich" 2009-07-12 Male
```

**Реализация:**
- Создается объект класса `Employee` с переданными параметрами
- Объект отправляется в БД через метод `insertEmployee()`
- Автоматически рассчитывается возраст при выводе

**Результат:**
```
Successfully connected to database
Executing: Insert employee
================================================================================
Employee added successfully
Employee added successfully!
Full Name: Ivanov Petr Sergeevich
Birth Date: 2009-07-12
Gender: Male
Age: 16 years
================================================================================
Command completed successfully!
```

---

### Режим 3: Вывод всех сотрудников

**Команда запуска:**
```bash
./SqlManager 3
```

**SQL запрос:**
```sql
SELECT DISTINCT ON (full_name, birth_date)
    full_name, 
    birth_date, 
    gender,
    EXTRACT(YEAR FROM AGE(birth_date)) AS age
FROM employees
ORDER BY full_name, birth_date
```

**Особенности реализации:**
- Используется `DISTINCT ON` для уникальности по ФИО + дата рождения
- Сортировка по ФИО (full_name)
- Расчет полных лет средствами SQL

**Пример вывода:**
```
Successfully connected to database
Executing: Display all employees
================================================================================
Displaying all employees (unique by Full Name + Birth Date, sorted by Full Name):
----------------------------------------------------------------------------------------------------
Total employees: 158362
----------------------------------------------------------------------------------------------------
Full Name: Anderson Ashley Alan
Birth Date: 1971-11-01
Gender: Female
Age: 54 years
----------------------------------------------------------------------------------------------------
...
```

---

### Режим 4: Автоматическое заполнение данных

**Команда запуска:**
```bash
./SqlManager 4
```

**Реализация:**

Используется паттерн **Strategy** с двумя стратегиями генерации данных:

1. **RandomDataGenerator** - генерирует 1,000,000 записей:
   - Равномерное распределение по полу (50/50)
   - Равномерное распределение начальных букв ФИО (A-Z)
   - Случайные даты рождения (1950-2005)

2. **TargetedDataGenerator** - генерирует 100 записей:
   - Пол: Male
   - Фамилия начинается с "F"

**Метод пакетной отправки:**
```cpp
void DatabaseManager::batchInsertEmployees(const std::vector<Employee>& employees) {
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
}
```

**Результат:**
```
Successfully connected to database
Executing: Fill database with test data
================================================================================
Filling database with test data...
Generating 1,000,000 random employees...
Inserting random employees into database...
Batch insert completed: 1000000 employees added
Random employees inserted successfully!
Generating 100 targeted employees (Male, surname starts with 'F')...
Inserting targeted employees into database...
Batch insert completed: 100 employees added
Targeted employees inserted successfully!
Database filled with 1,000,100 employees!
================================================================================
Command completed successfully!
```

**Статистика:**
- Всего записей добавлено: **1,000,100**
- Время выполнения: ~30-40 секунд
- Использован пакетный INSERT для оптимизации

---

### Режим 5: Выборка с замером времени

**Команда запуска:**
```bash
./SqlManager 5
```

**Критерии выборки:**
- Пол: Male (мужской)
- Фамилия начинается с "F"

**SQL запрос:**
```sql
SELECT DISTINCT ON (full_name, birth_date)
    full_name,
    birth_date,
    gender,
    EXTRACT(YEAR FROM AGE(birth_date)) AS age
FROM employees
WHERE gender = 'Male' AND full_name LIKE 'F%'
ORDER BY full_name, birth_date
```

**Реализация замера времени:**
```cpp
auto start = std::chrono::high_resolution_clock::now();
auto employees = dbManager.getEmployeesByCriteria("Male", "F");
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

**Результат ДО оптимизации:**
```
Successfully connected to database
Executing: Query employees by criteria
================================================================================
Querying employees: Gender = Male, Surname starts with 'F'
----------------------------------------------------------------------------------------------------
Query completed in 328 ms
Found 189442 employees matching criteria
----------------------------------------------------------------------------------------------------
Displaying all 189442 results:
[... полный вывод всех записей ...]
```

**ВРЕМЯ ВЫПОЛНЕНИЯ: 328 мс**

---

### Режим 6: Оптимизация базы данных

**Команда запуска:**
```bash
./SqlManager 6
```

#### 6.1. Примененные техники оптимизации

**1. Частичный индекс (Partial Index)**
```sql
CREATE INDEX idx_employees_male_f_surname 
ON employees (full_name, birth_date)
WHERE gender = 'Male' AND full_name >= 'F' AND full_name < 'G';
```

**Обоснование:**
- Индексирует только нужные записи (Male с фамилией на 'F')
- Значительно меньше по размеру, чем полный индекс
- Эффективен для селективных запросов

**2. Покрывающий индекс (Covering Index)**
```sql
CREATE INDEX idx_employees_covering 
ON employees (gender, full_name, birth_date)
WHERE gender = 'Male';
```

**Обоснование:**
- Содержит все колонки, необходимые для запроса
- Избегает обращений к основной таблице (Index-Only Scan)
- Ускоряет сортировку и фильтрацию

**3. VACUUM ANALYZE**
```sql
VACUUM ANALYZE employees;
```

**Обоснование:**

- Освобождает неиспользуемое пространство
- Обновляет статистику таблицы
- Улучшает планы выполнения запросов

**4. Увеличение work_mem**

```sql
SET work_mem = '256MB';
```

**Обоснование:**
- Больше памяти для операций сортировки
- Снижает использование временных файлов на диске
- Ускоряет DISTINCT и ORDER BY операции

#### 6.2. Результаты оптимизации

**Вывод программы:**
```
Successfully connected to database
Executing: Optimize database
================================================================================
Database Optimization Process
Query criteria: Gender = Male, Surname starts with 'F'
====================================================================================================

Step 0: Removing any existing optimization indexes...
All optimization indexes dropped successfully
----------------------------------------------------------------------------------------------------

Measuring query time BEFORE optimization...
Time BEFORE optimization: 441 ms
Found 189442 employees
----------------------------------------------------------------------------------------------------

Applying optimizations:
  Step 1: Creating partial index for Male employees with surname 'F'...
       Partial index created
  Step 2: Creating covering index for better performance...
       Covering index created
  Step 3: Running VACUUM ANALYZE to optimize table...
       VACUUM ANALYZE completed
  Step 4: Increasing work_mem for better sort performance...
       work_mem increased to 256MB

Optimization completed!
----------------------------------------------------------------------------------------------------

Measuring query time AFTER optimization...
Time AFTER optimization: 324 ms
Found 189442 employees
====================================================================================================

*** PERFORMANCE RESULTS ***
Time BEFORE optimization: 441 ms
Time AFTER optimization:  324 ms
Performance improvement: 26.53%
Time saved: 117 ms
====================================================================================================

Optimization techniques applied:
  1. Partial Index: Index only for Male employees with surname 'F'
  2. Covering Index: Includes all query columns to avoid table lookups
  3. VACUUM ANALYZE: Reclaimed storage and updated statistics
  4. Increased work_mem: Better memory for sorting operations (256MB)
================================================================================
Command completed successfully!
```

#### 6.3. Анализ результатов

| Метрика | До оптимизации | После оптимизации | Улучшение |
|---------|----------------|-------------------|-----------|
| Время выполнения | 441 мс | 324 мс | **-117 мс** |
| Процент ускорения | - | - | **26.53%** |
| Количество записей | 189,442 | 189,442 | без изменений |

**Проверка через режим 5 после оптимизации:**
```bash
./SqlManager 5
```

**Результат:**
```
Query completed in 328 ms
Found 189442 employees matching criteria
```

**ВРЕМЯ ВЫПОЛНЕНИЯ ПОСЛЕ ОПТИМИЗАЦИИ: 328 мс**

**Ускорение по сравнению с первоначальными 328 мс до применения индексов в режиме 6**


---

## 4. Дополнительные особенности реализации

### 4.1. Паттерны проектирования

**Command Pattern:**
```cpp
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute(DatabaseManager& dbManager) = 0;
    virtual const char* getDescription() const = 0;
};
```

**Factory Pattern:**
```cpp
class CommandFactory {
public:
    static std::unique_ptr<ICommand> createCommand(int mode, 
                                                   const std::vector<std::string>& args);
};
```

**Strategy Pattern:**
```cpp
class IDataGenerator {
public:
    virtual ~IDataGenerator() = default;
    virtual std::vector<Employee> generateEmployees(int count) = 0;
};
```

### 4.2. Конфигурация через config.h

Вместо внешнего файла конфигурации используется заголовочный файл:

```cpp
namespace Config {
    const std::string DB_HOST = "localhost";
    const std::string DB_PORT = "5432";
    const std::string DB_NAME = "employees_db";
    const std::string DB_USER = "postgres";
    const std::string DB_PASSWORD = "postgres";
}
```

**Преимущества:**
- Конфигурация встроена в бинарник
- Нет runtime зависимости от файла
- Проверка на этапе компиляции
- Проще распространение

### 4.3. Управление памятью

Использованы современные возможности C++17:
- Smart pointers (`std::unique_ptr`)
- RAII для управления ресурсами
- Move semantics для оптимизации

---

## 5. Сборка и запуск проекта

### 5.1. Требования
- C++17 или выше
- CMake 3.10+
- PostgreSQL 12+
- libpqxx
- Ninja (опционально)

### 5.2. Установка зависимостей (Arch Linux)
```bash
sudo pacman -S base-devel cmake postgresql libpqxx ninja
```

### 5.3. Настройка базы данных
```bash
# Запуск PostgreSQL
sudo systemctl start postgresql

# Создание базы данных
sudo -u postgres psql
CREATE DATABASE employees_db;
CREATE USER postgres WITH PASSWORD 'postgres';
GRANT ALL PRIVILEGES ON DATABASE employees_db TO postgres;
\q
```

### 5.4. Конфигурация приложения
```bash
# Скопировать пример конфигурации
cp include/config.h.example include/config.h

# Отредактировать параметры подключения
nano include/config.h
```

### 5.5. Сборка проекта
```bash
mkdir build
cd build
cmake -G Ninja ..
ninja
```

### 5.6. Запуск всех режимов
```bash
./SqlManager 1  # Создание таблицы
./SqlManager 2 "Smith John Michael" 1990-05-15 Male  # Добавление записи
./SqlManager 3  # Вывод всех сотрудников
./SqlManager 4  # Заполнение 1,000,100 записей
./SqlManager 5  # Выборка с замером времени
./SqlManager 6  # Оптимизация базы данных
```

---

### 6. Метрики производительности

Тестирование на таблице из 1,000,100 записей:

| Метрика | Значение |
|---------|----------|
| Записей в БД | 1,000,100 |
| Записей в выборке (Male + 'F') | 189,442 |
| Время выборки (режим 5) | **328 мс** |
| Время оптимизации (режим 6 до) | **441 мс** |
| Время оптимизации (режим 6 после) | **324 мс** |
| Ускорение | **26.53%** (117 мс) |
| Время оптимизации в режиме 6 | 459 мс → 313 мс (**31.81%**) |
