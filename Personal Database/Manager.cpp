#include <iostream>
#include <string>
#include "Manager.h"
#include "testCode.h"


namespace fs = std::filesystem;

manager::manager() :
    currentDB("") {};

bool manager::hasOpenDatabase() const 
{
    return !currentDB.empty();
}

std::string manager::getCurrentDatabase() const 
{
    return currentDB;
}

bool manager::doesDatabaseExists(const std::string& dbName) const
{
    std::string currName;
    std::ifstream searchFile("databaseNames.csv");

    while (std::getline(searchFile, currName))
    {
        if (currName == currentDB + ",")
        {
            return true;
            break;
        }
    }
    searchFile.close();
    return false;
}



void manager::dbLogger(std::string dbName) const
{

    if (doesDatabaseExists(dbName)) 
    {
        //exportAllTablesToCSV(dbName);
        std::cout << "Database opened. \n";
        return;
	}
    else if (!doesDatabaseExists(dbName))
    {
        std::ofstream writeFile("databaseNames.csv", std::ios::app); // Append mode
		writeFile << dbName << ",\n";
        writeFile.close();
        std::cout << "Database created: " << dbName << "\n";

        try
        {
            fs::path dbFolder = fs::path("./databases") / fs::path(currentDB);
            std::filesystem::create_directory(dbFolder);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Error creating database directory: " << e.what() << "\n";
            return;
        }

    }
    else
        std::cout << "Error opening file." << "\n";
}

void manager::createDatabaseFile(std::unique_ptr<SQLCommand>& cmd)
{
    auto* cdb{ dynamic_cast<createDatabase*>(cmd.get()) };
    // Future: Check if database already opened or needs to be switched
    if (cdb && currentDB == cdb->dbName)
    {
        std::cout << "Database already opened: " << cdb->dbName << "\n";
    }

    else if (cdb)
    {
        currentDB = cdb->dbName;
        dbLogger(currentDB);
    }
    else
    {
        std::cout << "No database opened. Please create or open a database first.\n";
        return;
    }
}
void manager::createTableFile(std::unique_ptr<SQLCommand>& cmd) const
{

    auto* cdb{ dynamic_cast<createTable*>(cmd.get()) };

    if (hasOpenDatabase() == false)
    {
        std::cout << "No database opened. Please create or open a database first.\n";
        std::cout << currentDB << "\n";
        return;
    }


    fs::path dbFolder = fs::path("./databases") / fs::path(currentDB);
    std::error_code ec;
    fs::create_directories(dbFolder, ec);
    if (ec)
    {
        std::cerr << "Failed to create database directory: " << ec.message() << "\n";
        return;
    }


    fs::path binPath = dbFolder / (cdb->tableName + "Schema" + ".bin");
    if (fs::exists(binPath))
    {
        std::cerr << "Table Already Exists\n";
        return;
    }

    std::ofstream binFile(binPath, std::ios::binary);
    if (!binFile)
    {
        std::cerr << "Failed to create binary table file: " << binPath << "\n";
        return;
    }

    // Writing Column Info to BIN file

    /*
    FORMAT:

    columnCount - count, size

        column1 - length, size
        column1 - columnName tokenized to char
        column1 - datatype represented as typeInt

    */
    size_t colCount = cdb->columns.size();
    binFile.write(reinterpret_cast<const char*>(&colCount), sizeof(colCount));

    for (const auto& col : cdb->columns)
    {
        size_t nameLen = col.first.size();
        binFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        binFile.write(col.first.data(), nameLen);

        int typeInt;
        if (col.second == "INT")        typeInt = 0;
        else if (col.second == "FLOAT") typeInt = 1;
        else if (col.second == "TEXT")  typeInt = 2;
        else
        {
            std::cerr << "Unsupported column type: " << col.second << "\n";
            binFile.close();
            fs::remove(binPath); // File will be deleted if datatypes don't match
            return;
        }
        binFile.write(reinterpret_cast<const char*>(&typeInt), sizeof(typeInt));
    }

    binFile.close();
    std::cout << "Table created with binary file: " << binPath << "\n";
}
void manager::insertDataToFile(std::unique_ptr<SQLCommand>& cmd) const
{
    auto* cdb{ dynamic_cast<insertCommand*>(cmd.get()) };

    if (!hasOpenDatabase())
    {
        std::cout << "No database opened. Please create or open a database first.\n";
        return;
    }
    if (!cdb)
    {
        std::cerr << "Invalid INSERT command.\n";
        return;
    }

    fs::path binPath = fs::path("./databases") / currentDB / (cdb->tableName + "Schema" + ".bin");
    if (!fs::exists(binPath))
    {
        std::cerr << "Table not found: " << cdb->tableName << "\n";
        return;
    }

    std::ifstream binFileViewer(binPath, std::ios::binary);
    if (!binFileViewer)
    {
        std::cerr << "Failed to open binary table file: " << binPath << "\n";
        return;
    }

    size_t colCount{ 0 };
    binFileViewer.read(reinterpret_cast<char*>(&colCount), sizeof(colCount));
    std::vector<int> colTypes;

    for (size_t i = 0; i < colCount; ++i)
    {
        size_t nameLen{ 0 };
        //Reading binary files dont use const char*

        binFileViewer.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        binFileViewer.seekg(nameLen, std::ios::cur); // Moves nameLen bytes from current position

        int typeInt{ 0 };
        binFileViewer.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
        colTypes.push_back(typeInt);
    }
    binFileViewer.close();

    binPath = fs::path("./databases") / currentDB / (cdb->tableName + ".bin"); // Updates binpath to not open the schema
    std::ofstream binFileWriter(binPath, std::ios::binary | std::ios::app);
    if (!binFileWriter)
    {
        std::cerr << "Failed to open binary table file for writing: " << binPath << "\n";
        return;
    }

    for (size_t g{ 0 }; g < colCount; ++g)
    {
        if (colTypes[g] == 0 && !std::all_of(cdb->values[g].begin(), cdb->values[g].end(), ::isdigit)) // INT
        {
            std::cerr << "Error: Value '" << cdb->values[g] << "' is not a valid INT.\n";
        }
        else if (colTypes[g] == 1)
        {
            try
            {
                std::stof(cdb->values[g]);
            }
            catch (const std::invalid_argument&)
            {
                std::cerr << "Error: Value '" << cdb->values[g] << "' is not a valid FLOAT.\n";
                return;
            }
        }
    }

    // Works as VALUE -> SIZE

    for (size_t i = 0; i < colCount; ++i)
    {
        if (colTypes[i] == 0) // INT
        {
            int val{ std::stoi(cdb->values[i]) };
            binFileWriter.write(reinterpret_cast<const char*>(&val), sizeof(val));
        }

        else if (colTypes[i] == 1) // FLOAT
        {
            float val{ std::stof(cdb->values[i]) };
            binFileWriter.write(reinterpret_cast<const char*>(&val), sizeof(val));
        }

        else if (colTypes[i] == 2) // TEXT
        {
            size_t len = cdb->values[i].size();
            binFileWriter.write(reinterpret_cast<const char*>(&len), sizeof(len));
            binFileWriter.write(cdb->values[i].data(), len); // Needed since strings vary in length
        }
    }

    std::cout << "Data inserted into table: " << cdb->tableName << "\n";
}
void manager::insertNewColumn(std::unique_ptr<SQLCommand>& cmd) const 
{
    auto* cdb = dynamic_cast<createIndex*>(cmd.get());
    if (!cdb)
    {
        std::cerr << "Invalid CREATE NEW COLUMN command.\n";
        return;
    }
    if (!hasOpenDatabase())
    {
        std::cerr << "No database opened. Please create or open a database first.\n";
        return;
    }

    fs::path binFile = fs::path("./databases") / currentDB / (cdb->tableName + "Schema" + ".bin");
    if (!fs::exists(binFile))
    {
        std::cerr << "Table does not exist: Pagfdashadfhdfhadfhadfh" << cdb->tableName << "\n";
        return;
    }

    std::fstream fileSchema(binFile, std::ios::binary | std::ios::in | std::ios::out);

    int typeInt;
    if (cdb->columnData.second == "INT")          typeInt = 0;
    else if (cdb->columnData.second == "FLOAT")   typeInt = 1;
    else if (cdb->columnData.second == "TEXT")    typeInt = 2;
    else
    {
        std::cerr << "Unsupported column type: " << cdb->columnData.second << "\n";
        fileSchema.close();

        return;
    }

	// Adds new column to the schema file
    size_t nameLen{ cdb->columnData.first.size() };
    fileSchema.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    fileSchema.write(cdb->columnData.first.data(), nameLen);
    fileSchema.write(reinterpret_cast<const char*>(&typeInt), sizeof(typeInt));

    size_t colCount{ 0 };
    fileSchema.seekp(0, std::ios::beg);
    fileSchema.read(reinterpret_cast<char*>(&colCount), sizeof(colCount));
    ++colCount;
    fileSchema.write(reinterpret_cast<const char*>(&colCount), sizeof(colCount));

	// Update the column count in the schema file
	fileSchema.seekp(0, std::ios::beg);
	fileSchema.write(reinterpret_cast<const char*>(&colCount), sizeof(colCount));
    fileSchema.close();

    // Implement new file that will copy over original file
    fs::path newBinFile = fs::path("./databases") / currentDB / ("new" + cdb->tableName + ".bin");
    fs::path oldBinFile = fs::path("./databases") / currentDB / (cdb->tableName + ".bin");
    std::ofstream copyFile(newBinFile, std::ios::binary | std::ios::app);
    std::ifstream originalFile(oldBinFile, std::ios::binary);

	// Vector for data types
	std::vector<int> colTypes;
	originalFile.read(reinterpret_cast<char*>(&colCount), sizeof(colCount));
    for (size_t i = 0; i < colCount; ++i)
    {
        size_t nameLen{ 0 };
        originalFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        originalFile.seekg(nameLen, std::ios::cur); // Skip over column name
        int typeInt{ 0 };
        originalFile.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
        colTypes.push_back(typeInt);
	}

    while (originalFile)
    {
        for (size_t i = 0; i < colCount; ++i)
        {
            if (i == colCount - 1 && cdb->columnData.second != "TEXT")
            {
                size_t padding{ 0 };
                copyFile.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
			}
            else if (i == colCount - 1 && cdb->columnData.second == "TEXT")
            {
                size_t padding{ cdb->columnData.first.size() };
                copyFile.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
                copyFile.write(cdb->columnData.first.data(), padding);
			}


            else if (colTypes[i] == 0) // INT
            {
                int val{ 0 };
                originalFile.read(reinterpret_cast<char*>(&val), sizeof(val));
                copyFile.write(reinterpret_cast<const char*>(&val), sizeof(val));
            }
            else if (colTypes[i] == 1) // FLOAT
            {
                float val{ 0.0f };
                originalFile.read(reinterpret_cast<char*>(&val), sizeof(val));
                copyFile.write(reinterpret_cast<const char*>(&val), sizeof(val));
            }
            else if (colTypes[i] == 2) // TEXT
            {
                size_t len{ 0 };
                originalFile.read(reinterpret_cast<char*>(&len), sizeof(len));
                std::string text(len, '\0');
                originalFile.read(&text[0], len);
                copyFile.write(reinterpret_cast<const char*>(&len), sizeof(len));
                copyFile.write(text.data(), len);
            }
        }
    }


	std::cout << "New column added: " << cdb->columnData.first << " of type " << cdb->columnData.second << "\n";
    fs::remove(oldBinFile);
	fs::rename(newBinFile, oldBinFile);
    originalFile.close();
    copyFile.close();
}

void manager::execute(std::unique_ptr<SQLCommand> cmd) 
{
    if (!cmd) return;

    /*************************/
      /*  CREATE DB COMMAND */
    /*************************/

    if (cmd->type() == commandType::CREATE_DATABASE) 
    { 
        createDatabaseFile(cmd);
    }

    /*************************/
    /*  CREATE TABLE COMMAND */
	/*************************/
    
    if (cmd->type() == commandType::CREATE_TABLE)
    {
        createTableFile(cmd);
    }

   /*************************/
   /*  SELECT COMMAND */
   /*************************/
   
    // NOT FINISHED - IMPLEMENT WITH BTREES

    if (cmd->type() == commandType::SELECT) 
    {
        auto* cdb{ dynamic_cast<selectCommand*>(cmd.get()) };

        if (!hasOpenDatabase())
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }

        if (!cdb)
        {
            std::cout << "Invalid SELECT command.\n";
            return;
        }

        fs::path tablePath = fs::path("./databases") / currentDB / (cdb->tableName + ".csv");
        if (!fs::exists(tablePath))
        {
            std::cerr << "Table does not exist: " << cdb->tableName << "\n";
            return;
        }

        std::ifstream tableFile(tablePath);
        if (!tableFile)
        {
            std::cerr << "Failed to open table file: " << tablePath << "\n";
            return;
        }

        std::string line;
        while (std::getline(tableFile, line))
        {
            std::cout << line << "\n";
        }

        tableFile.close();
    }

    /*************************/
       /*  INSERT COMMAND */
   /*************************/

    if (cmd->type() == commandType::INSERT)
    {
        insertDataToFile(cmd);
    }

    /*************************/
    /*  NEW COLUMN COMMAND */
   /*************************/

    
    if (cmd->type() == commandType::CREATE_INDEX)  
    {
		insertNewColumn(cmd);
    }
    
    /*************************/
    /*     DROP TABLE        */
    /*************************/

    if (cmd->type() == commandType::DROP_TABLE)
    {
        auto* dropCmd = dynamic_cast<dropTable*>(cmd.get());
        if (!dropCmd) {
            std::cerr << "Invalid DROP TABLE command.\n";
            return;
        }

        if (!hasOpenDatabase()) {
            std::cerr << "No database is currently open.\n";
            return;
        }

        fs::path dbPath = fs::path("./databases") / currentDB;
        fs::path tableBin = dbPath / (dropCmd->tableName + ".bin");
        fs::path tableSchema = dbPath / (dropCmd->tableName + "Schema.bin");
        fs::path tableCSV = dbPath / (dropCmd->tableName + ".csv");
        fs::path tableMeta = dbPath / (dropCmd->tableName + ".txt");

        bool found = false;

        for (const auto& path : { tableBin, tableSchema, tableCSV, tableMeta }) {
            if (fs::exists(path)) {
                fs::remove(path);
                std::cout << "Removed: " << path.filename() << "\n";
                found = true;
            }
        }

        if (!found) {
            std::cerr << "Table '" << dropCmd->tableName << "' not found in current database.\n";
        }
        else {
            std::cout << "Table '" << dropCmd->tableName << "' successfully dropped.\n";
        }
    }

    /*************************/
    /*     DROP DATABASE     */
    /*************************/

    if (cmd->type() == commandType::DROP_DATABASE)
    {
        auto* dropDbCmd = dynamic_cast<dropDatabase*>(cmd.get());
        if (!dropDbCmd)
        {
            std::cerr << "Invalid DROP DATABASE command.\n";
            return;
        }

        std::string dbFolder = "./databases/" + dropDbCmd->dbName;

        if (!std::filesystem::exists(dbFolder))
        {
            std::cerr << "Database '" << dropDbCmd->dbName << "' does not exist.\n";
            return;
        }

        try
        {
            std::filesystem::remove_all(dbFolder);
            std::cout << "Database '" << dropDbCmd->dbName << "' dropped successfully.\n";

            if (currentDB == dropDbCmd->dbName)
                currentDB.clear();
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Error deleting database folder: " << e.what() << '\n';
        }
    }


}