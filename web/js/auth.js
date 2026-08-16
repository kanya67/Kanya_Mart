function showMessage(msg, type) {
    const box = document.getElementById('messageBox');
    box.textContent = msg;
    box.className = 'message ' + type;
    box.style.display = 'block';
}

function showForm(formId) {
    if (formId === 'login') {
        document.getElementById('registerForm').style.display = 'none';
        document.getElementById('loginForm').style.display = 'block';
    } else {
        document.getElementById('loginForm').style.display = 'none';
        document.getElementById('registerForm').style.display = 'block';
    }
}

async function register() {
    const username = document.getElementById('regUsername').value;
    const email = document.getElementById('regEmail').value;
    const password = document.getElementById('regPassword').value;

    if (!username || !email || !password) {
        showMessage('Please fill in all fields', 'error');
        return;
    }

    try {
        const response = await fetch(
            '/api/v1/auth/register',
            {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    username,
                    email,
                    password
                })
            }
        );

        const data = await response.json();

        if (response.ok) {
            showMessage(
                'Registration successful! Please login.',
                'success'
            );

            showForm('login');
        } else {
            showMessage(
                data.error || 'Registration failed',
                'error'
            );
        }
    } catch (error) {
        showMessage(
            'Cannot connect to server. Is it running?',
            'error'
        );
    }
}
