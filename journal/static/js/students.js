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

            data.students.sort((a, b) => {
                if (a.surname.toLowerCase() < b.surname.toLowerCase()) return -1;
                if (a.surname.toLowerCase() > b.surname.toLowerCase()) return 1;
                return 0;
            });

            data.students.forEach(student => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>${student.id}</td>
                    <td>${student.surname}</td>
                    <td>${student.name}</td>
                    <td>${student.lastname}</td>
                    <td>
                        <button class="btn btn-add" onclick="openAddResultModal(${student.id})">+</button>
                    </td>
                `;
                tableBody.appendChild(row);
            });
        } else {
            tableBody.innerHTML = '<tr><td colspan="5">No students found</td></tr>';
        }
    } catch (error) {
        console.error('Error loading students:', error);
    }
}

function openAddStudentModal() {
    document.getElementById('addStudentModal').style.display = 'flex';
}

function closeAddStudentModal() {
    document.getElementById('addStudentModal').style.display = 'none';
}

function openRemoveStudentModal() {
    document.getElementById('removeStudentModal').style.display = 'flex';
}

function closeRemoveStudentModal() {
    document.getElementById('removeStudentModal').style.display = 'none';
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
            const errorMessage = await res.text();
            if (errorMessage == "Student not found") {
                removeStudentMessage.textContent = "Студент не найден";
            } else {
                removeStudentMessage.textContent = await res.text();
            }
            return;
        }
        loadStudents();
        closeRemoveStudentModal();
    } catch (error) {
        console.error('Error removing student:', error);
    }
});