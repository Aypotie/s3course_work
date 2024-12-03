const showResultsBtn = document.getElementById('showResultsBtn');
const resultsTableBody = document.querySelector('#resultsTable tbody');
const resultsTableHead = document.querySelector('#resultsTable thead');
const resultsModal = document.getElementById('resultsModal');
const closeResultsBtn = document.querySelector('.close-btn');

let studentResults = {};

showResultsBtn.addEventListener('click', async () => {
    resultsTableBody.innerHTML = '';
    let text;
    try {
        const response = await fetch('/results');
        text = await response.text();
        if (text == "null") {
            throw new Error("Результаты ещё не добавлены");
        }
        if (!response.ok) {
            throw new Error('Ошибка загрузки результатов');
        }
    } catch (error) {
        console.error('Ошибка:', error);
        resultsTableBody.innerHTML = `<tr><td colspan="4">${error.message}</td></tr>`;
        resultsModal.style.display = 'flex';
        return;
    }

    const data = JSON.parse(text);

    // Сохраняем уникальные чекпоинты
    let checkpointNames = new Set();
    data.results.forEach(result => {
        checkpointNames.add(result.checkpoint_name);
    });
    checkpointNames = Array.from(checkpointNames); // Конвертируем Set в массив

    // Сохраняем студентов и их результаты
    studentResults = {}; // Обнуляем перед загрузкой новых данных
    data.results.forEach(result => {
        const student = result.student_name;
        if (!studentResults[student]) {
            studentResults[student] = {};
        }
        studentResults[student][result.checkpoint_name] = {"score": result.score, "id": result.id};
    });

    // Обновляем заголовки таблицы
    resultsTableHead.innerHTML = '';
    const headerRow = document.createElement('tr');
    headerRow.innerHTML = `<th>ФИО</th>`; // Первый заголовок — ФИО
    checkpointNames.forEach(name => {
        headerRow.innerHTML += `<th>${name}</th>`;
    });
    headerRow.innerHTML += `<th>Удалить оценку</th>`; // Добавляем новый столбец
    resultsTableHead.appendChild(headerRow);

    // Обновляем тело таблицы
    resultsTableBody.innerHTML = '';
    Object.keys(studentResults).forEach(studentName => {
        const row = document.createElement('tr');
        let rowHTML = `<td>${studentName}</td>`;
        checkpointNames.forEach(checkpointName => {
            let score, id;
            if (studentResults[studentName][checkpointName]) {
                score = studentResults[studentName][checkpointName]["score"] || '';
                id = studentResults[studentName][checkpointName]["id"] || '0';
            } else {
                score = '';
                id = '0';
            }
            rowHTML += `<td result-id="${id}">${score}</td>`;
        });

        // Добавляем кнопку удаления результата с выпадающим списком
        rowHTML += `<td>
                        <button class="btn btn-remove" onclick="toggleDropdown('${studentName}')">−</button>
                        <div id="dropdown-${studentName}" class="dropdown-content" style="display: none;">
                            <!-- Сюда будут динамически добавляться контрольные работы -->
                        </div>
                     </td>`;

        row.innerHTML = rowHTML;
        resultsTableBody.appendChild(row);
    });

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

function openCheckResultsModal() {
    document.getElementById('resultsModal').style.display = 'flex';
}

function closeCheckResultsModal() {
    document.getElementById('resultsModal').style.display = 'none';
}

function openAddResultModal(studentId) {
    const modal = document.getElementById("addResultModal");
    modal.style.display = "flex";

    const studentIdInput = document.getElementById("addStudentId");
    studentIdInput.value = studentId;
}

function closeAddResultModal() {
    const modal = document.getElementById("addResultModal");
    modal.style.display = "none";

    document.getElementById("resultErrorMessage").textContent = "";
}

document.getElementById("addResultForm").addEventListener("submit", async function (e) {
    e.preventDefault();

    const studentId = document.getElementById("addStudentId").value;
    const checkpointId = document.getElementById("addCheckpointId").value;
    const score = document.getElementById("addScore").value;

    try {
        const response = await fetch("/results", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({
                "student_id": studentId, 
                "checkpoint_id": checkpointId, 
                "score": score
            }),
        });

        if (!response.ok) {
            const errorText = await response.text();
            if (errorText == "Invalid score") {
                throw new Error("Неправильный балл оценки");
            }
            if (errorText == "Score bigger then max") {
                throw new Error("Оценка превышает максимальную");
            }
            if (errorText == "Checkpoint not found") {
                throw new Error("Контрольной точки с таким ID не существует");
            }
            if (errorText == "Exist result") {
                throw new Error("Результат для данного студента уже существует");
            }
            throw new Error("Ошибка добавления результата");

        }

        closeAddResultModal();
    } catch (error) {
        document.getElementById("resultErrorMessage").textContent = error.message;
    }
});
//удаление результата
async function deleteResult(studentName, checkpointName) {
    try {
        const resultId = studentResults[studentName][checkpointName].id;

        // Отправляем запрос на удаление результата
        const response = await fetch(`/results/${resultId}`, {
            method: 'DELETE',
        });

        if (!response.ok) {
            throw new Error('Ошибка удаления результата');
        }

        // Закрыть выпадающий список
        const dropdown = document.getElementById(`dropdown-${studentName}`);
        dropdown.style.display = 'none';

        // Обновить результаты
        showResultsBtn.click(); // Это вызовет повторное отображение результатов
    } catch (error) {
        alert(error.message);
    }
}

// Функция для отображения/скрытия выпадающего списка
function toggleDropdown(studentName) {
    const dropdown = document.getElementById(`dropdown-${studentName}`);
    
    // Проверяем, если выпадающий список скрыт, показываем его
    if (dropdown.style.display === "none" || dropdown.style.display === "") {
        dropdown.style.display = "block";
        populateDropdown(studentName); // Заполняем список для этого студента
    } else {
        // Если он виден, скрываем
        dropdown.style.display = "none";
    }
}

// Функция для заполнения выпадающего списка контрольными работами для студента
function populateDropdown(studentName) {
    const dropdown = document.getElementById(`dropdown-${studentName}`);
    dropdown.innerHTML = ''; // Очищаем список, чтобы не дублировать элементы

    const checkpointNames = Object.keys(studentResults[studentName]);

    checkpointNames.forEach(checkpointName => {
        const option = document.createElement('div');
        option.classList.add('dropdown-item');
        option.textContent = checkpointName;
        option.onclick = function() {
            deleteResult(studentName, checkpointName);
        };
        dropdown.appendChild(option);
    });
}

//

