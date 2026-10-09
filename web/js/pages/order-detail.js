/* ============================================================
   KANYAMART ORDER DETAIL PAGE
   ============================================================ */
const OrderDetailPage = {
    render(id) {
        return `<div class="order-detail-page"><div class="container" id="order-detail-content">${Components.loading('Loading order...')}</div></div>`;
    },

    async init(id) {
        const res = await API.getOrder(id);
        if (res.ok) { this.renderOrder(res.data.data); }
        else { document.getElementById('order-detail-content').innerHTML = Components.emptyState('📦', 'Order not found', 'This order may not exist.', 'Back to Orders', "App.navigate('/orders')"); }
    },

    renderOrder(order) {
        const itemsHtml = (order.items || []).map(item => `
            <div class="flex items-center justify-between" style="padding:var(--space-md) 0;border-bottom:1px solid var(--glass-border)">
                <div class="flex items-center gap-md">
                    <div style="width:50px;height:50px;border-radius:var(--radius-md);background:var(--bg-tertiary);display:flex;align-items:center;justify-content:center;font-size:20px">📦</div>
                    <div>
                        <div style="font-weight:600;cursor:pointer" onclick="App.navigate('/product/${item.product_id}')">${item.product_name}</div>
                        <div class="text-sm text-muted">${item.seller_name || 'Seller'} · Qty: ${item.quantity}</div>
                    </div>
                </div>
                <div style="text-align:right">
                    <div class="text-sm text-muted">₹${Number(item.unit_price).toLocaleString('en-IN')} × ${item.quantity}</div>
                    <div style="font-weight:700;font-family:var(--font-heading);color:var(--accent-blue)">₹${Number(item.subtotal).toLocaleString('en-IN')}</div>
                </div>
            </div>`).join('');

        document.getElementById('order-detail-content').innerHTML = `
            <button class="btn btn-ghost btn-sm mb-lg" onclick="App.navigate('/orders')">← Back to Orders</button>
            <div class="flex justify-between items-center mb-lg flex-wrap gap-md">
                <h1 style="font-family:var(--font-heading);font-size:28px">Order #${order.id.substring(0,8)}</h1>
                ${Components.statusBadge(order.status)}
            </div>
            ${Components.orderTimeline(order.status)}
            <div class="cart-layout mt-xl">
                <div class="glass-card" style="padding:var(--space-xl)">
                    <h3 style="font-family:var(--font-heading);margin-bottom:var(--space-md)">Order Items</h3>
                    ${itemsHtml}
                </div>
                <div class="cart-summary">
                    <h3>Order Summary</h3>
                    <div class="cart-summary-row"><span class="label">Order ID</span><span style="font-size:12px">${order.id.substring(0,13)}...</span></div>
                    <div class="cart-summary-row"><span class="label">Date</span><span>${order.created_at}</span></div>
                    <div class="cart-summary-row"><span class="label">Status</span>${Components.statusBadge(order.status)}</div>
                    <div class="cart-summary-total"><span>Total</span><span class="amount">₹${Number(order.total).toLocaleString('en-IN')}</span></div>
                </div>
            </div>`;
    }
};
