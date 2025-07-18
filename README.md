# 🚀 QuickDB: A Lightweight C++ Database Engine

**QuickDB** is a minimalist, file-based database engine written in modern C++. It offers a simple SQL-like command interface for creating databases, tables, inserting/selecting data, and now even adding new columns dynamically. This project is perfect for students and developers looking to understand how databases work behind the scenes.

---

## ✨ Features

- 📁 **File-Based Storage**: Data is stored in CSV files, with corresponding metadata.
- 💡 **SQL-like Commands**:
  - `CREATE DATABASE dbname`
  - `USE dbname`
  - `CREATE TABLE tablename (column TYPE, ...)`
  - `INSERT INTO tablename VALUES (val1, val2, ...)`
  - `SELECT * FROM tablename`
  - `CREATE NEWCOLUMN tablename (column TYPE)`
- 🧠 **Custom SQL Parser**: Built from scratch with token streams and command classes.
- ⚙️ **Modular Design**: Easily extendable command execution and parsing logic.
- 📚 **Column Type Awareness**: Each column's type is saved and utilized during operations.


---


## 🧪 Sample Usage

```sql
CREATE DATABASE school;
USE school;

CREATE TABLE students (id INT, name TEXT);
INSERT INTO students VALUES (1, Alice);
INSERT INTO students VALUES (2, Bob);

SELECT * FROM students;

CREATE NEWCOLUMN students (email TEXT);
