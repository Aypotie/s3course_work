/*document.addEventListener('DOMContentLoaded', () => {
    loadStudents();
    loadGroupName();
    loadCheckpoints();
});
*/
authSubmitBtn.addEventListener('click', submitAuth);
authPassword.addEventListener('keypress', (event) => {
    if (event.key === 'Enter') {
        submitAuth();
    }
});

async function submitAuth() {
    try {
        const response = await fetch('/api/auth', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ password: authPassword.value }),
        });

        const result = await response.json();

        if (result.success) {
            authModal.style.display = 'none'; // Закрыть окно авторизации
        } else {
            authError.textContent = 'Неверный пароль';
            authError.style.display = 'block';
        }
    } catch (error) {
        authError.textContent = 'Ошибка';
        authError.style.display = 'block';
    }
}

document.addEventListener("DOMContentLoaded", () => {
    loadStudents();
    loadCheckpoints();

    fetch("/group_name")
        .then(response => {
            if (response.status === 204) {
                // Если файл пустой, открываем модальное окно
                openGroupNameModal();
            } else if (response.status === 200) {
                return response.json();
            } else {
                throw new Error("Не удалось загрузить название группы");
            }
        })
        .then(data => {
            if (data && data.group_name) {
                document.getElementById("groupName").textContent = `Название группы: ${data.group_name}`;
            }
        })
        .catch(error => console.error(error));
});

// Открыть модальное окно для ввода названия группы
function openGroupNameModal() {
    const modal = document.getElementById("groupNameModal");
    modal.style.display = "flex";
}

// Закрыть модальное окно
function closeGroupNameModal() {
    const modal = document.getElementById("groupNameModal");
    modal.style.display = "none";
}

// Сохранить название группы
function saveGroupName() {
    const groupNameInput = document.getElementById("groupNameInput").value.trim();

    if (groupNameInput === "") {
        alert("Название группы не может быть пустым");
        return;
    }

    fetch("/group_name", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: groupNameInput })
    })
    .then(response => {
        if (response.status === 200) {
            document.getElementById("groupName").textContent = `Название группы: ${groupNameInput}`;
            closeGroupNameModal();
        } else {
            throw new Error("Не удалось сохранить название группы");
        }
    })
    .catch(error => console.error(error));
}

