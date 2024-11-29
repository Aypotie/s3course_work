-- Create student_group table
CREATE TABLE IF NOT EXISTS student_group (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    surname VARCHAR(255) NOT NULL,
    lastname VARCHAR(255) NOT NULL
);

-- Create checkpoints table
CREATE TABLE IF NOT EXISTS checkpoints (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    max_score INT NOT NULL,
    date VARCHAR(32),
    description TEXT
);

-- Create results table
CREATE TABLE IF NOT EXISTS results (
    id SERIAL PRIMARY KEY,
    student_id INT NOT NULL,
    checkpoint_id INT NOT NULL,
    score INT NOT NULL
);

-- Insert user
INSERT INTO student_group (name, surname, lastname) VALUES ($1, $2, $3);

-- Insert checkpoint
INSERT INTO checkpoints (name, max_score, date, description) VALUES ($1, $2, $3, $4);

-- Insert result
INSERT INTO results (student_id, checkpoint_id, score) VALUES ($1, $2, $3);

-- Select users
SELECT id, name, surname, lastname FROM student_group;

-- Select checkpoints
SELECT id, name, max_score, date, description FROM checkpoints;

-- Select results
SELECT results.id, student_group.lastname, student_group.surname, student_group.name, 
       checkpoints.name AS checkpoint_name, results.score
FROM results
JOIN student_group ON results.student_id = student_group.id
JOIN checkpoints ON results.checkpoint_id = checkpoints.id;

-- Delete user
DELETE FROM student_group WHERE id = $1;

-- Delete checkpoint
DELETE FROM checkpoints WHERE id = $1;

-- Delete result
DELETE FROM results WHERE id = $1;
