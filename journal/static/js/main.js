// Modal logic
document.addEventListener('DOMContentLoaded', () => {
    loadStudents();
    loadGroupName();
    loadCheckpoints();

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

    // Close modal on outside click
    resultsModal.addEventListener('click', (event) => {
        if (event.target === resultsModal) {
            resultsModal.style.display = 'none';
        }
    });
});