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
VALUES 
('Admin1', 'admin1@gmail.com', 'admin123', '9876543210', 'Admin'),
('Student1', 'stud1@gmail.com', 'stud123', '9123456780', 'Student');

INSERT INTO ADMIN (user_id) VALUES(1);

INSERT INTO Student (user_id) VALUES (2);

INSERT INTO Subject (subject_name, start_date, end_date, admin_id)
VALUES ('DBMS', '2026-01-01', '2026-12-13', 1);

INSERT INTO Questions (quest_txt, sub_id)
VALUES 
('What is DBMS?', 1),
('What is normalization?', 1);

INSERT INTO Options (quest_id, option_txt, is_correct)
VALUES
(1, 'Database system', TRUE),
(1, 'Operating system', FALSE),
(2, 'Removing redundancy', TRUE),
(2, 'Data duplication', FALSE);

INSERT INTO Personal_Test (student_id, sub_id)
VALUES (1, 1);

INSERT INTO Personal_Test_Qs (student_id, sub_id, quest_id)
VALUES (1, 1,1),
	   (1,1,2);
       
INSERT INTO Personal_Response (student_id, sub_id, quest_id, opt_id)
VALUES (1, 1,1,1),
	   (1,1,2,3);
       
INSERT INTO Personal_Result (student_id, sub_id, score)
VALUES (1,1,2);

Select *From Personal_Result;

