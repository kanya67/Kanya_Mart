/* ============================================================
   KANYAMART SELLER DASHBOARD PAGE
   ============================================================ */
const SellerDashboardPage = {
    render() {
        return `<div class="seller-page"><div class="container-wide" id="seller-dashboard-content">${Components.loading('Loading dashboard...')}</div></div>`;
    },

    async init() {
        const res = await API.getSellerDashboard();
        if (res.ok) { this.renderDashboard(res.data.data); }
        else { document.getElementById('seller-dashboard-content').innerHTML = Components.emptyState('📊', 'Unable to load dashboard', 'Please try again or check your seller permissions.'); }
    },

    renderDashboard(stats) {
        document.getElementById('seller-dashboard-content').innerHTML = `
            <div class="flex justify-between items-center mb-xl flex-wrap gap-md">
                <h1>Seller Dashboard</h1>
                <div class="flex gap-sm">
                    <button class="btn btn-secondary" onclick="App.navigate('/seller/products')">📦 Manage Products</button>
                    <button class="btn btn-secondary" onclick="App.navigate('/seller/orders')">📋 Manage Orders</button>
                </div>
            </div>

            <h3 style="font-family:var(--font-heading);margin-bottom:var(--space-md)">Overview</h3>
            <div class="stats-grid">
                <div class="stats-card stagger-1 animate-fade-in-up">
                    <div class="stat-icon">📈</div>
                    <div class="stat-value">₹${Number(stats.total_sales || 0).toLocaleString('en-IN')}</div>
                    <div class="stat-label">Total Sales</div>
                </div>
                <div class="stats-card stagger-2 animate-fade-in-up">
                    <div class="stat-icon">📋</div>
                    <div class="stat-value">${stats.total_orders || 0}</div>
                    <div class="stat-label">Total Orders</div>
                </div>
                <div class="stats-card stagger-3 animate-fade-in-up">
                    <div class="stat-icon">⏳</div>
                    <div class="stat-value" style="color:var(--accent-orange)">${stats.pending_orders || 0}</div>
                    <div class="stat-label">Pending Orders</div>
                </div>
                <div class="stats-card stagger-4 animate-fade-in-up">
                    <div class="stat-icon">📦</div>
                    <div class="stat-value">${stats.active_products || 0} <span style="font-size:14px;color:var(--text-muted)">/ ${stats.total_products || 0}</span></div>
                    <div class="stat-label">Active Products</div>
                </div>
            </div>

            ${stats.low_stock > 0 ? `
            <div class="toast error" style="position:static;animation:none;margin-bottom:var(--space-xl)">
                <span>⚠️</span><span>You have ${stats.low_stock} product(s) with low stock. Please check your inventory.</span>
                <span class="toast-close" style="visibility:hidden">×</span>
            </div>` : ''}

            <div class="glass-card animate-fade-in-up stagger-2" style="padding:var(--space-xl);text-align:center">
                <div style="font-size:48px;opacity:0.5;margin-bottom:var(--space-md)">🚀</div>
                <h3 style="font-family:var(--font-heading);margin-bottom:var(--space-sm)">Grow Your Business</h3>
                <p style="color:var(--text-secondary);max-width:500px;margin:0 auto var(--space-lg)">FaalihaMart provides the tools you need to reach more customers and manage your store efficiently.</p>
                <button class="btn btn-primary" onclick="App.navigate('/seller/products')">+ Add New Product</button>
            </div>
        `;
    }
};
