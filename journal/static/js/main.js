async function loadStudents() {
    try {
        const response = await fetch('/student');
        if (!response.ok) {
            throw new Error('Failed to fetch students');
        }
        const data = await response.json();
        const tableBody = document.querySelector('#studentTable tbody');

        tableBody.innerHTML = '';

        if (data.students && data.students.length > 0) {
            data.students.forEach(student => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>${student.id}</td>
                    <td>${student.surname}</td>
                    <td>${student.name}</td>
                    <td>${student.lastname}</td>
                `;
                tableBody.appendChild(row);
            });
        } else {
            tableBody.innerHTML = '<tr><td colspan="4">No students found</td></tr>';
        }
    } catch (error) {
        console.error('Error loading students:', error);
    }
}

async function loadGroupName() {
    try {
        const response = await fetch('/group_name');
        if (!response.ok) {
            throw new Error('Failed to fetch group name');
        }
        const data = await response.json();
        const groupNameElement = document.getElementById('groupName');
        groupNameElement.textContent = `Название группы: ${data.group_name}`;
    } catch (error) {
        console.error('Error loading group name:', error);
        document.getElementById('groupName').textContent = 'Название группы не установлено';
    }
}

async function loadCheckpoints() {
    try {
        const response = await fetch('/checkpoints');
        if (!response.ok) {
            throw new Error('Failed to fetch checkpoints');
        }
        const data = await response.json();
        const tableBody = document.querySelector('#checkpointTable tbody');

        tableBody.innerHTML = '';

        if (data.checkpoints && data.checkpoints.length > 0) {
            data.checkpoints.forEach(checkpoint => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>${checkpoint.id}</td>
                    <td>${checkpoint.name}</td>
                    <td>${checkpoint.max_score}</td>
                    <td>${checkpoint.date}</td>
                    <td>${checkpoint.description}</td>
                `;
                tableBody.appendChild(row);
            });
        } else {
            tableBody.innerHTML = '<tr><td colspan="5">No checkpoints found</td></tr>';
        }
    } catch (error) {
        console.error('Error loading checkpoints:', error);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    loadStudents();
    loadGroupName();
    loadCheckpoints();
});

function addStudent() {
    console.log('Add student functionality');
}

function removeStudent() {
    console.log('Remove student functionality');
}

function addCheckpoint() {
    console.log('Add checkpoint functionality');
}

function removeCheckpoint() {
    console.log('Remove checkpoint functionality');
}

document.addEventListener('DOMContentLoaded', () => {
    loadStudents();
    loadGroupName();
    loadCheckpoints();
});

// Открытие/закрытие модальных окон
function openAddStudentModal() {
    document.getElementById('addStudentModal').style.display = 'flex';
}

function closeAddStudentModal() {
    document.getElementById('addStudentModal').style.display = 'none';
}

function closeCheckResultsModal() {
    document.getElementById('resultsModal').style.display = 'none';
}

function openRemoveStudentModal() {
    document.getElementById('removeStudentModal').style.display = 'flex';
}

function closeRemoveStudentModal() {
    document.getElementById('removeStudentModal').style.display = 'none';
}

// Загрузка таблиц
async function loadStudents() {
    try {
        const response = await fetch('/student');
        const data = await response.json();
        const tableBody = document.querySelector('#studentTable tbody');
        tableBody.innerHTML = '';

        data.students.forEach(student => {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${student.id}</td>
                <td>${student.surname}</td>
                <td>${student.name}</td>
                <td>${student.lastname}</td>
            `;
            tableBody.appendChild(row);
        });
    } catch (error) {
        console.error('Error loading students:', error);
    }
}

// Добавление студента
document.getElementById('addStudentForm').addEventListener('submit', async (event) => {
    event.preventDefault();
    const formData = new FormData(event.target);

    try {
        await fetch('/student', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(Object.fromEntries(formData)),
        });
        loadStudents();
        closeAddStudentModal();
    } catch (error) {
        console.error('Error adding student:', error);
    }
});

// Удаление студента
document.getElementById('removeStudentForm').addEventListener('submit', async (event) => {
    event.preventDefault();
    const studentId = document.getElementById('studentId').value;
    const removeStudentMessage = document.getElementById('removeStudentMessage');

    try {
        const res = await fetch(`/student/${studentId}`, { method: 'DELETE' });
        if (res.status != 200) {
            removeStudentMessage.textContent = await res.text();
            return;
        }
        loadStudents();
        closeRemoveStudentModal();
    } catch (error) {
        console.error('Error removing student:', error);
    }
});

// Modal logic
document.addEventListener('DOMContentLoaded', () => {
    const resultsModal = document.getElementById('resultsModal');
    const showResultsBtn = document.getElementById('showResultsBtn');
    const closeResultsBtn = document.querySelector('.close-btn');
    const resultsTableBody = document.querySelector('#resultsTable tbody');

    // Open modal
    showResultsBtn.addEventListener('click', async () => {
        resultsTableBody.innerHTML = '';
        try {
            const response = await fetch('/results');
            const text = await response.text();
            if (text == "null") {
                throw new Error("Результаты ещё не добавлены");
            }
            if (!response.ok) {
                throw new Error('Ошибка загрузки результатов');
            }

            const data = JSON.parse(text);

            if (data.results && data.results.length > 0) {
                data.results.forEach(result => {
                    const row = document.createElement('tr');
                    row.innerHTML = `
                        <td>${result.id}</td>
                        <td>${result.student_name}</td>
                        <td>${result.checkpoint_name}</td>
                        <td>${result.score}</td>
                    `;
                    resultsTableBody.appendChild(row);
                });
            } else {
                resultsTableBody.innerHTML = '<tr><td colspan="4">Результаты не найдены</td></tr>';
            }
        } catch (error) {
            console.error('Ошибка:', error);
            resultsTableBody.innerHTML = `<tr><td colspan="4">${error}</td></tr>`;
        }

        resultsModal.style.display = 'flex';
    });

    // Close modal
    closeResultsBtn.addEventListener('click', () => {
        resultsModal.style.display = 'none';
    });

    // Close modal on outside click
    resultsModal.addEventListener('click', (event) => {
        if (event.target === resultsModal) {
            resultsModal.style.display = 'none';
        }
    });
});