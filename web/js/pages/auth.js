/* ============================================================
   KANYAMART AUTH PAGES (Login & Register)
   ============================================================ */
const AuthPage = {
    renderLogin() {
        return `
        <div class="auth-page">
            <div class="auth-container animate-fade-in">
                <div class="auth-visual">
                    <div class="auth-visual-logo">KanyaMart</div>
                    <p class="auth-visual-text">Welcome back! Sign in to access your personalized marketplace experience.</p>
                </div>
                <div class="auth-form-container">
                    <h2>Sign In</h2>
                    <p class="auth-subtitle">Enter your credentials to continue</p>
                    <div id="auth-message"></div>
                    <form onsubmit="AuthPage.login(event)">
                        <div class="form-group">
                            <label for="login-email">Email</label>
                            <input type="email" id="login-email" placeholder="name@example.com" required>
                        </div>
                        <div class="form-group">
                            <label for="login-password">Password</label>
                            <div class="password-toggle">
                                <input type="password" id="login-password" placeholder="••••••••" required>
                                <span class="toggle-btn" onclick="AuthPage.togglePassword('login-password')">👁</span>
                            </div>
                        </div>
                        <button type="submit" class="btn btn-primary btn-full btn-lg" id="login-btn">Sign In</button>
                    </form>
                    <div class="auth-toggle">
                        Don't have an account? <a onclick="App.navigate('/register')">Create one</a>
                    </div>
                </div>
            </div>
        </div>`;
    },

    renderRegister() {
        return `
        <div class="auth-page">
            <div class="auth-container animate-fade-in">
                <div class="auth-visual">
                    <div class="auth-visual-logo">KanyaMart</div>
                    <p class="auth-visual-text">Join the future of shopping. Create your account and start exploring.</p>
                </div>
                <div class="auth-form-container">
                    <h2>Create Account</h2>
                    <p class="auth-subtitle">Fill in your details to get started</p>
                    <div id="auth-message"></div>
                    <form onsubmit="AuthPage.register(event)">
                        <div class="form-group">
                            <label for="reg-username">Username</label>
                            <input type="text" id="reg-username" placeholder="Choose a username" required>
                        </div>
                        <div class="form-group">
                            <label for="reg-email">Email</label>
                            <input type="email" id="reg-email" placeholder="name@example.com" required>
                        </div>
                        <div class="form-group">
                            <label for="reg-password">Password</label>
                            <div class="password-toggle">
                                <input type="password" id="reg-password" placeholder="At least 6 characters" required oninput="AuthPage.checkPasswordStrength(this.value)">
                                <span class="toggle-btn" onclick="AuthPage.togglePassword('reg-password')">👁</span>
                            </div>
                            <div id="password-strength" class="password-strength"><div class="bar"></div><div class="bar"></div><div class="bar"></div><div class="bar"></div></div>
                        </div>
                        <div class="form-group">
                            <label for="reg-confirm">Confirm Password</label>
                            <div class="password-toggle">
                                <input type="password" id="reg-confirm" placeholder="••••••••" required>
                                <span class="toggle-btn" onclick="AuthPage.togglePassword('reg-confirm')">👁</span>
                            </div>
                        </div>
                        <div class="form-group">
                            <label for="reg-role">Account Type</label>
                            <select id="reg-role">
                                <option value="customer">Customer</option>
                                <option value="seller">Seller</option>
                            </select>
                        </div>
                        <button type="submit" class="btn btn-primary btn-full btn-lg" id="reg-btn">Create Account</button>
                    </form>
                    <div class="auth-toggle">
                        Already have an account? <a onclick="App.navigate('/login')">Sign in</a>
                    </div>
                </div>
            </div>
        </div>`;
    },

    async login(e) {
        e.preventDefault();
        const btn = document.getElementById('login-btn');
        btn.disabled = true; btn.textContent = 'Signing in...';

        const email = document.getElementById('login-email').value;
        const password = document.getElementById('login-password').value;

        const res = await API.login({ email, password });
        btn.disabled = false; btn.textContent = 'Sign In';

        if (res.ok) {
            Components.toast('Login successful!', 'success');
            App.user = res.data.data;
            App.updateNav();
            if (App.user.role === 'seller') App.navigate('/seller/dashboard');
            else App.navigate('/shop');
        } else {
            const msg = res.data.error ? res.data.error.message : 'Login failed';
            document.getElementById('auth-message').innerHTML = `<div class="toast error" style="position:static;animation:none;margin-bottom:var(--space-md)">${msg}</div>`;
        }
    },

    async register(e) {
        e.preventDefault();
        const btn = document.getElementById('reg-btn');
        btn.disabled = true; btn.textContent = 'Creating account...';

        const username = document.getElementById('reg-username').value;
        const email = document.getElementById('reg-email').value;
        const password = document.getElementById('reg-password').value;
        const confirm = document.getElementById('reg-confirm').value;
        const role = document.getElementById('reg-role').value;

        if (password !== confirm) {
            document.getElementById('auth-message').innerHTML = `<div class="toast error" style="position:static;animation:none;margin-bottom:var(--space-md)">Passwords do not match</div>`;
            btn.disabled = false; btn.textContent = 'Create Account';
            return;
        }

        const res = await API.register({ username, email, password, role });
        btn.disabled = false; btn.textContent = 'Create Account';

        if (res.ok) {
            Components.toast('Registration successful! Please login.', 'success');
            App.navigate('/login');
        } else {
            const msg = res.data.error ? res.data.error.message : 'Registration failed';
            document.getElementById('auth-message').innerHTML = `<div class="toast error" style="position:static;animation:none;margin-bottom:var(--space-md)">${msg}</div>`;
        }
    },

    togglePassword(id) {
        const input = document.getElementById(id);
        input.type = input.type === 'password' ? 'text' : 'password';
    },

    checkPasswordStrength(password) {
        const el = document.getElementById('password-strength');
        if (!el) return;
        let strength = '';
        if (password.length >= 8 && /[A-Z]/.test(password) && /[0-9]/.test(password) && /[^A-Za-z0-9]/.test(password)) strength = 'very-strong';
        else if (password.length >= 8 && /[A-Z]/.test(password) && /[0-9]/.test(password)) strength = 'strong';
        else if (password.length >= 6) strength = 'medium';
        else if (password.length > 0) strength = 'weak';
        el.className = 'password-strength ' + strength;
    }
};
