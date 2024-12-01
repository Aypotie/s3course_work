function openCheckResultsModal() {
    document.getElementById('resultsModal').style.display = 'flex';
}

function closeCheckResultsModal() {
    document.getElementById('resultsModal').style.display = 'none';
}

function openAddResultModal(studentId) {
    const modal = document.getElementById("addResultModal");
    modal.style.display = "flex";

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

    const studentId = document.getElementById("addStudentId").value;
    const checkpointId = document.getElementById("addCheckpointId").value;
    const score = document.getElementById("addScore").value;

    try {
        // Отправка данных на сервер
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
            throw new Error("Ошибка добавления результата");
        }

        closeAddResultModal();
    } catch (error) {
        document.getElementById("resultErrorMessage").textContent = error.message;
    }
});
