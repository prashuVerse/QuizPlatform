Here is a complete, professional `README.md` file for your project. I have strictly removed all emojis and kept the formatting clean, academic, and ready for your GitHub repository. 

***

# Adaptive Quiz Engine

## Overview
The Adaptive Quiz Engine is a robust, console-based application developed in C++ with a MySQL relational database backend. Unlike traditional static assessments, this system dynamically adjusts the difficulty of questions in real-time based on the user's ongoing performance. The project utilizes a strict 3-tier architecture to ensure high maintainability, secure data persistence, and algorithmic efficiency.

## Key Features
* **Dynamic Adaptability:** Implements a real-time state-routing algorithm that promotes or demotes a user's difficulty tier (Easy, Medium, Hard) based on immediate past performance.
* **Database Persistence:** Securely logs user credentials, individual question responses, historical quiz attempts, and scores within a structured MySQL database.
* **Algorithmic Randomization:** Utilizes the Fisher-Yates shuffle algorithm to randomize both the questions presented and the order of multiple-choice options, preventing predictability.
* **Role-Based Access Control:** Features dedicated login systems and dashboards for Administrators (to manage subjects and question banks) and Students (to take quizzes and view histories).
* **Enterprise-Level Security:** Utilizes `sql::PreparedStatement` for all database interactions, ensuring complete immunity against SQL Injection (SQLi) attacks.

## System Architecture
The application is built using a clean 3-Tier Architecture to separate concerns:
1. **Core Layer (Data Models):** Contains pure object-oriented data structures (`Question.cpp`, `Attempt.cpp`, `User.cpp`) utilizing strict data encapsulation.
2. **Services Layer (Business Logic):** Functions as the central processing unit (`QuizEngine.cpp`, `AdminService.cpp`, `StudentService.cpp`), handling control flow, the Mersenne Twister random number generator, and the O(1) adaptive routing logic.
3. **Persistence Layer (Data Access):** Manages all input/output operations regarding long-term data storage (`DatabaseStorage.cpp`), acting as the exclusive gateway between the C++ application and MySQL.

## Algorithm Complexity
* **Adaptive Routing Engine:** Operates in **O(1)** constant time to determine the next difficulty state.
* **Randomized Question Pooling:** Utilizes the Fisher-Yates algorithm, operating in **O(N)** linear time and **O(1)** space.
* **Overall Space Complexity:** Highly optimized at **O(n)**, allocating memory exclusively for the specific subject's question bank currently being tested.

## Prerequisites
To compile and run this project natively on Windows, you will need:
* Microsoft Visual Studio (MSVC) Native C++ Compiler (`cl.exe`)
* MySQL Server (running locally or remotely)
* MySQL Connector/C++ (Version 8.0 or 9.x)

## Installation and Setup

### 1. Database Configuration
Before running the application, ensure your MySQL server is running and the database schema is initialized. You must create the required tables: `USERS`, `Student`, `Questions`, `Options`, `Subjects`, and `Attempts`. 

### 2. Compilation (Windows MSVC)
Open the **Developer Command Prompt for Visual Studio**, navigate to the project directory, and execute the following command to compile the 3-tier architecture:

```cmd
cl /EHsc main.cpp core\*.cpp persistence\*.cpp services\*.cpp /I"C:\Program Files\MySQL\MySQL Connector C++ 9.6\include" /link /LIBPATH:"C:\Program Files\MySQL\MySQL Connector C++ 9.6\lib64" /LIBPATH:"C:\Program Files\MySQL\MySQL Connector C++ 9.6\lib64\vs14" mysqlcppconn.lib /out:quiz_app.exe
```
*(Note: Adjust the file paths if your MySQL Connector is installed in a different directory or uses a different version number).*

### 3. Execution
To run the application successfully, the MySQL dynamic link library (.dll) must be present in the same directory as the executable.

```cmd
copy "C:\Program Files\MySQL\MySQL Connector C++ 9.6\lib64\vs14\*.dll" .
quiz_app.exe
```

## Usage
Upon launching `quiz_app.exe`, users are presented with a main menu.
* **Students:** Can log in, select a subject, and take an adaptive quiz. Upon completion, the final score is calculated and saved to the database. Students can also view their past attempt history.
* **Administrators:** Can log in to add new subjects, insert new questions (defining text, difficulty, and multiple-choice options), and manage the backend data safely.

## Future Scope
* Migration from a CLI interface to a modern Graphical User Interface (GUI) using frameworks like Qt.
* Implementation of data visualization dashboards for advanced student analytics.
* Expansion of the data models to support alternative question formats such as fill-in-the-blanks or true/false statements.