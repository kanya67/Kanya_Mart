/* ============================================================
   KANYAMART ORDER HISTORY PAGE
   ============================================================ */
const OrdersPage = {
    render() {
        return `<div class="orders-page"><div class="container" id="orders-content">${Components.loading('Loading orders...')}</div></div>`;
    },

    async init() {
        const res = await API.getOrders();
        if (res.ok) { this.renderOrders(res.data.data); }
        else { document.getElementById('orders-content').innerHTML = Components.emptyState('📦', 'Unable to load orders', 'Please try again.'); }
    },

    renderOrders(orders) {
        const el = document.getElementById('orders-content');
        if (!orders || orders.length === 0) {
            el.innerHTML = `<h1>My Orders</h1>` + Components.emptyState('📦', "You haven't placed any orders yet", 'Start shopping to see your orders here.', 'Shop Now', "App.navigate('/shop')");
            return;
        }

        const cardsHtml = orders.map(o => `
            <div class="order-card animate-fade-in" onclick="App.navigate('/orders/${o.id}')">
                <div class="order-card-header">
                    <span class="order-card-id">Order #${o.id.substring(0,8)}</span>
                    ${Components.statusBadge(o.status)}
                </div>
                <div class="order-card-details">
                    <span>${o.created_at} · ${o.item_count} item${o.item_count !== 1 ? 's' : ''}</span>
                    <span class="order-card-total">₹${Number(o.total).toLocaleString('en-IN')}</span>
                </div>
            </div>`).join('');

        el.innerHTML = `<h1>My Orders</h1>${cardsHtml}`;
    }
};
