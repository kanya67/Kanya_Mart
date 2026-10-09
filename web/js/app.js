/* ============================================================
   KANYAMART SPA ROUTER & APP LOGIC
   ============================================================ */

const App = {
    user: null,
    currentPath: '',
    routes: {
        '/': LandingPage,
        '/login': AuthPage,
        '/register': AuthPage,
        '/shop': ShopPage,
        '/product': ProductPage,
        '/cart': CartPage,
        '/checkout': CheckoutPage,
        '/orders': OrdersPage,
        '/seller/dashboard': SellerDashboardPage,
        '/seller/products': SellerProductsPage,
        '/seller/orders': SellerOrdersPage
    },

    async init() {
        // Initialize 3D scene first (hidden by default unless on landing)
        try { ThreeScene.init(); } catch (e) { console.error('ThreeJS init failed', e); }

        // Check authentication
        const res = await API.getMe();
        if (res.ok && res.data.data) {
            this.user = res.data.data;
            this.updateCartCount();
        }

        // Setup routing
        window.addEventListener('popstate', () => this.handleRoute());
        this.handleRoute(window.location.pathname);
    },

    async handleRoute(path = window.location.pathname) {
        // Normalize path
        if (path !== '/' && path.endsWith('/')) path = path.slice(0, -1);
        if (path === '') path = '/';
        this.currentPath = path;

        const contentDiv = document.getElementById('app-content');
        const navbar = document.getElementById('navbar');
        const footer = document.getElementById('footer');

        // Close mobile menu if open
        this.closeMobileMenu();

        // Update nav state
        this.updateNav();

        // Route matching
        let page = null;
        let id = null;

        // Static routes
        if (this.routes[path]) {
            page = this.routes[path];
        }
        // Dynamic routes (/product/:id or /orders/:id)
        else if (path.startsWith('/product/')) {
            page = ProductPage;
            id = path.split('/')[2];
        }
        else if (path.startsWith('/orders/')) {
            page = OrderDetailPage;
            id = path.split('/')[2];
        }
        else {
            path = '/';
            page = LandingPage;
            window.history.replaceState(null, '', '/');
        }

        // Auth Guards
        const requiresAuth = ['/cart', '/checkout', '/orders', '/seller'];
        const isAuthRoute = requiresAuth.some(r => path.startsWith(r));
        if (isAuthRoute && !this.user) {
            this.navigate('/login');
            Components.toast('Please log in to access this page', 'info');
            return;
        }

        const requiresSeller = ['/seller'];
        const isSellerRoute = requiresSeller.some(r => path.startsWith(r));
        if (isSellerRoute && this.user && this.user.role !== 'seller' && this.user.role !== 'admin') {
            this.navigate('/shop');
            Components.toast('Seller access required', 'error');
            return;
        }

        // Layout visibility
        if (path === '/' || path === '/login' || path === '/register') {
            navbar.classList.add('hidden');
            footer.classList.add('hidden');
            if (path === '/') ThreeScene.show();
            else ThreeScene.hide();
        } else {
            navbar.classList.remove('hidden');
            footer.classList.remove('hidden');
            ThreeScene.hide();
        }

        // Render page
        if (path === '/login') contentDiv.innerHTML = AuthPage.renderLogin();
        else if (path === '/register') contentDiv.innerHTML = AuthPage.renderRegister();
        else contentDiv.innerHTML = page.render(id);

        // Initialize page logic
        if (page.init) {
            try {
                await page.init(id);
            } catch (e) {
                console.error('Page init error:', e);
            }
        }
        
        // Scroll to top
        window.scrollTo(0, 0);
    },

    navigate(path) {
        if (this.currentPath === path) return;
        window.history.pushState(null, '', path);
        this.handleRoute(path);
    },

    updateNav() {
        const links = document.querySelectorAll('.nav-link[data-page]');
        links.forEach(l => {
            const pagePath = l.getAttribute('data-page');
            if (this.currentPath.includes(pagePath)) l.classList.add('active');
            else l.classList.remove('active');
        });

        // User info & links
        const userInfo = document.getElementById('nav-user-info');
        const username = document.getElementById('nav-username');
        const sellerNav = document.getElementById('nav-seller');
        const mobileSellerNav = document.getElementById('mobile-seller-link');
        
        if (this.user) {
            username.innerHTML = `<span style="font-weight:600">${this.user.username}</span> <span style="opacity:0.5;font-size:12px">(${this.user.role})</span>`;
            if (this.user.role === 'seller' || this.user.role === 'admin') {
                if (sellerNav) sellerNav.classList.remove('hidden');
                if (mobileSellerNav) mobileSellerNav.classList.remove('hidden');
            } else {
                if (sellerNav) sellerNav.classList.add('hidden');
                if (mobileSellerNav) mobileSellerNav.classList.add('hidden');
            }
        }
    },

    async updateCartCount() {
        if (!this.user) return;
        const res = await API.getCart();
        if (res.ok && res.data.data) {
            const count = res.data.data.item_count || 0;
            const badge = document.getElementById('cart-count');
            if (badge) {
                if (count > 0) {
                    badge.textContent = count > 99 ? '99+' : count;
                    badge.classList.remove('hidden');
                } else {
                    badge.classList.add('hidden');
                }
            }
        }
    },

    async logout() {
        await API.logout();
        this.user = null;
        const badge = document.getElementById('cart-count');
        if (badge) badge.classList.add('hidden');
        this.navigate('/login');
        Components.toast('Logged out successfully', 'info');
    },
    
    // Add to cart helper (used globally from product cards)
    async addToCart(productId) {
        if (!this.user) {
            this.navigate('/login');
            Components.toast('Please log in to add items to cart', 'info');
            return;
        }
        const res = await API.addToCart(productId, 1);
        if (res.ok) {
            Components.toast('Added to cart!', 'success');
            this.updateCartCount();
        } else {
            Components.toast(res.data.error ? res.data.error.message : 'Failed to add to cart', 'error');
        }
    },

    toggleMobileMenu() {
        const nav = document.getElementById('mobile-nav');
        if (nav) nav.classList.toggle('open');
    },
    
    closeMobileMenu() {
        const nav = document.getElementById('mobile-nav');
        if (nav) nav.classList.remove('open');
    }
};

// Navbar scroll effect
window.addEventListener('scroll', () => {
    const nav = document.getElementById('navbar');
    if (nav) {
        if (window.scrollY > 20) nav.classList.add('scrolled');
        else nav.classList.remove('scrolled');
    }
});

// Start app
document.addEventListener('DOMContentLoaded', () => App.init());
