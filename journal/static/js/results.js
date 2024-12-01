function openCheckResultsModal() {
    document.getElementById('resultsModal').style.display = 'flex';
}

function closeCheckResultsModal() {
    document.getElementById('resultsModal').style.display = 'none';
}

function openAddResultModal(studentId) {
    const modal = document.getElementById("addResultModal");
    modal.style.display = "block";

    // Заполняем поле ID студента
    const studentIdInput = document.getElementById("addStudentId");
    studentIdInput.value = studentId;
}

function closeAddResultModal() {
    const modal = document.getElementById("addResultModal");
    modal.style.display = "none";

    // Очищаем сообщение об ошибке
    document.getElementById("resultErrorMessage").textContent = "";
}

document.getElementById("addResultForm").addEventListener("submit", async function (e) {
    e.preventDefault();

    const studentId = document.getElementById("studentId").value;
    const checkpointId = document.getElementById("checkpointId").value;
    const score = document.getElementById("score").value;

    try {
        // Отправка данных на сервер
        const response = await fetch("/api/results", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({ studentId, checkpointId, score }),
        });

        if (!response.ok) {
            throw new Error("Ошибка добавления результата");
        }

        // Закрываем модальное окно и обновляем данные
        closeAddResultModal();
        alert("Результат добавлен успешно!");
    } catch (error) {
        document.getElementById("resultErrorMessage").textContent = error.message;
    }
});
