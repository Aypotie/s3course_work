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
        tableBody.innerHTML = ''; // Очистка таблицы

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

function openAddCheckpointModal() {
    document.getElementById('addCheckpointModal').style.display = 'flex';
}

function closeAddCheckpointModal() {
    document.getElementById('addCheckpointModal').style.display = 'none';
}

function openRemoveCheckpointModal() {
    document.getElementById('removeCheckpointModal').style.display = 'flex';
}

function closeRemoveCheckpointModal() {
    document.getElementById('removeCheckpointModal').style.display = 'none';
}

//добавление чекпоинта
document.getElementById('addCheckpointForm').addEventListener('submit', async (event) => {
    event.preventDefault();
    const formData = new FormData(event.target);
    const checkpointData = {
        name: formData.get('name'),
        max_score: parseInt(formData.get('score'), 10),
        date: formData.get('date'),
        description: formData.get('description'),
    };

    const errorMessageElement = document.getElementById('checkpointErrorMessage');  // Элемент для вывода ошибки

    try {
        const response = await fetch('/checkpoints', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(checkpointData),
        });

        if (!response.ok) {
            const errorMessage = await response.text();  // Получаем сообщение об ошибке с сервера
            throw new Error(errorMessage);  // Бросаем ошибку с этим сообщением
            
        }

        loadCheckpoints(); // Перезагрузка таблицы чекпоинтов
        closeAddCheckpointModal(); // Закрытие модального окна

    } catch (error) {
        console.error('Ошибка добавления чекпоинта:', error);
        if (error.message == "Incorrect date") {
            errorMessageElement.textContent = "Неправильная запись даты";
        } else if (error.message == "Exist checkpoint") {
            errorMessageElement.textContent = "Такая работа уже существует";
        } else {
            errorMessageElement.textContent = error.message;  // Отображаем сообщение об ошибке на странице
        }
    }
});

//удаление чекпоинта
document.getElementById('removeCheckpointForm').addEventListener('submit', async (event) => {
    event.preventDefault();
    const checkpointId = document.getElementById('checkpointId').value;
    const removeCheckpointMessage = document.getElementById('removeCheckpointMessage');

    try {
        const res = await fetch(`/checkpoints/${checkpointId}`, { method: 'DELETE' });

        if (!res.ok) { // Используем res вместо response
            const errorMessage = await res.text();
            throw new Error(errorMessage);
        }

        loadCheckpoints(); // Перезагрузка таблицы чекпоинтов
        closeRemoveCheckpointModal(); // Закрытие модального окна
    } catch (error) {
        console.error('Ошибка удаления чекпоинта:', error);
        if (error.message == "Checkpoint not found") {
            removeCheckpointMessage.textContent = "Контрольная точка не найдена";
        } else {
            removeCheckpointMessage.textContent = error.message;
        }
    }
});