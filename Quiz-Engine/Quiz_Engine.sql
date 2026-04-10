CREATE DATABASE quiz_engine;
USE quiz_engine;

CREATE TABLE USERS(
     user_id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    email_id VARCHAR(100) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    mobile_no VARCHAR(15),
    role ENUM('Admin', 'Student') NOT NULL
);

CREATE TABLE Admin (
    admin_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,
    FOREIGN KEY (user_id) REFERENCES USERS(user_id) ON DELETE CASCADE
);

CREATE TABLE Student (
    student_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,
    FOREIGN KEY (user_id) REFERENCES USERS(user_id) ON DELETE CASCADE
);
SelecT *from USERS;

CREATE TABLE Subject (
    sub_id INT PRIMARY KEY AUTO_INCREMENT,
    subject_name VARCHAR(100),
    start_date DATE,
    end_date DATE,
    admin_id INT, -- Relationship: Admin manages Subject
    FOREIGN KEY (admin_id) REFERENCES Admin(admin_id)
);

CREATE TABLE Questions (
    quest_id INT PRIMARY KEY AUTO_INCREMENT,
    quest_txt TEXT NOT NULL,
    sub_id INT, -- Relationship: Subject has Questions
    FOREIGN KEY (sub_id) REFERENCES Subject(sub_id) ON DELETE CASCADE
);


ALTER TABLE Questions ADD difficulty INT DEFAULT 1;
CREATE TABLE Options (
    opt_id INT PRIMARY KEY AUTO_INCREMENT,
    quest_id INT,
    option_txt TEXT NOT NULL,
    is_correct BOOLEAN DEFAULT FALSE,
    FOREIGN KEY (quest_id) REFERENCES Questions(quest_id) ON DELETE CASCADE
);

CREATE TABLE Personal_Test (
    test_id INT PRIMARY KEY AUTO_INCREMENT,
    student_id INT,
    sub_id INT,
    FOREIGN KEY (student_id) REFERENCES Student(student_id),
    FOREIGN KEY (sub_id) REFERENCES Subject(sub_id),
    UNIQUE(student_id, sub_id) -- Ensures a student takes a specific subject test once
);

CREATE TABLE Personal_Test (
    student_id INT,
    sub_id INT,
    question_id INT, -- Included as per your diagram's attribute list
    PRIMARY KEY (student_id, sub_id), 
    FOREIGN KEY (student_id) REFERENCES Student(student_id),
    FOREIGN KEY (sub_id) REFERENCES Subject(sub_id)
);

CREATE TABLE Personal_Result (
    attempt_id INT AUTO_INCREMENT PRIMARY KEY,
    student_id INT,
    sub_id INT,
    score INT,
    attempt_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id, sub_id) REFERENCES Personal_Test(student_id, sub_id) ON DELETE CASCADE
);



CREATE TABLE Personal_Test_Qs (
    student_id INT,
    sub_id INT,
    quest_id INT,
    PRIMARY KEY (student_id, sub_id, quest_id),
    FOREIGN KEY (student_id, sub_id) REFERENCES Personal_Test(student_id, sub_id),
    FOREIGN KEY (quest_id) REFERENCES Questions(quest_id)
);

CREATE TABLE Personal_Response (
    -- The core attributes from your diagram
    student_id INT,
    sub_id INT,
    quest_id INT,
    opt_id INT,
    
    -- Composite Primary Key: ensures one response per question per test
    PRIMARY KEY (student_id, sub_id, quest_id),
    
    -- Links back to the specific test instance
    FOREIGN KEY (student_id, sub_id) 
        REFERENCES Personal_Test(student_id, sub_id),
        
    -- Links to the question being answered
    FOREIGN KEY (quest_id) 
        REFERENCES Questions(quest_id),
        
    -- Links to the specific option the student selected
    FOREIGN KEY (opt_id) 
        REFERENCES Options(opt_id)
);

INSERT INTO USERS (name, email_id, password, mobile_no, role)
VALUES ('Admin', 'admin@gmail.com', 'admin123', '9999999999', 'Admin');

INSERT INTO Admin (user_id) VALUES (1);

INSERT INTO Subject (subject_name, start_date, end_date, admin_id)
VALUES 
('DSA', '2026-01-01', '2026-12-31', 1),
('C++', '2026-01-01', '2026-12-31', 1),
('Java', '2026-01-01', '2026-12-31', 1);

INSERT INTO Questions (quest_txt, sub_id, difficulty) VALUES
('What is a stack?', 1,1),
('What is a queue?', 1,1),
('What is recursion?', 1,2),
('Time complexity of binary search?', 1,2),
('What is a linked list?', 1,1),
('DFS stands for?', 1,1),
('BFS uses which structure?', 1,2),
('Worst case of quicksort?', 1,3),
('What is hashing?', 1,2),
('Heap is used in?', 1,2);

INSERT INTO Questions (quest_txt, sub_id, difficulty) VALUES
('What is OOP?', 2,1),
('What is a class?', 2,1),
('Constructor is?', 2,1),
('Destructor is?', 2,1),
('What is inheritance?', 2,2),
('What is polymorphism?', 2,2),
('STL stands for?', 2,1),
('Vector is?', 2,1),
('Pointer is?', 2,1),
('Reference is?', 2,2);

INSERT INTO Questions (quest_txt, sub_id, difficulty) VALUES
('What is JVM?', 3,1),
('What is JDK?', 3,1),
('What is JRE?', 3,1),
('What is inheritance?', 3,2),
('What is encapsulation?', 3,2),
('What is abstraction?', 3,2),
('What is interface?', 3,2),
('What is exception?', 3,1),
('What is thread?', 3,2),
('What is collection?', 3,1);

SET FOREIGN_KEY_CHECKS = 0;
TRUNCATE TABLE Options;
SET FOREIGN_KEY_CHECKS = 1;

INSERT INTO Options VALUES
(NULL,30,'Single object',FALSE),
(NULL,30,'Group of objects',TRUE),
(NULL,30,'Array only',FALSE),
(NULL,30,'None',FALSE);

select *from Options;