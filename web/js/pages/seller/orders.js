/* ============================================================
   KANYAMART SELLER ORDERS PAGE
   ============================================================ */
const SellerOrdersPage = {
    orders: [],

    render() {
        return `<div class="seller-page"><div class="container-wide" id="seller-orders-content">${Components.loading('Loading orders...')}</div></div>`;
    },

    async init() {
        const res = await API.getSellerOrders();
        if (res.ok) { this.orders = res.data.data; this.renderOrders(); }
        else { document.getElementById('seller-orders-content').innerHTML = Components.emptyState('📋', 'Unable to load orders', 'Please try again.'); }
    },

    renderOrders() {
        let tableBody = '';
        if (this.orders.length === 0) {
            tableBody = `<tr><td colspan="6" style="text-align:center;padding:var(--space-xl)">No orders found.</td></tr>`;
        } else {
            tableBody = this.orders.map(o => `
                <tr>
                    <td style="font-family:var(--font-heading);font-weight:600">#${o.id.substring(0,8)}</td>
                    <td>${o.customer_name}</td>
                    <td>${o.created_at}</td>
                    <td>${Components.statusBadge(o.status)}</td>
                    <td>
                        <select class="status-select" onchange="SellerOrdersPage.updateStatus('${o.id}', this.value)" style="padding:4px 8px;font-size:12px;width:130px;background:var(--bg-tertiary)">
                            <option value="">Update Status...</option>
                            <option value="CONFIRMED" ${['CANCELLED','CONFIRMED','PROCESSING','SHIPPED','OUT_FOR_DELIVERY','DELIVERED'].includes(o.status) ? 'disabled' : ''}>Confirmed</option>
                            <option value="PROCESSING" ${['CANCELLED','PROCESSING','SHIPPED','OUT_FOR_DELIVERY','DELIVERED'].includes(o.status) ? 'disabled' : ''}>Processing</option>
                            <option value="SHIPPED" ${['CANCELLED','SHIPPED','OUT_FOR_DELIVERY','DELIVERED'].includes(o.status) ? 'disabled' : ''}>Shipped</option>
                            <option value="OUT_FOR_DELIVERY" ${['CANCELLED','OUT_FOR_DELIVERY','DELIVERED'].includes(o.status) ? 'disabled' : ''}>Out for Delivery</option>
                            <option value="DELIVERED" ${['CANCELLED','DELIVERED'].includes(o.status) ? 'disabled' : ''}>Delivered</option>
                            <option value="CANCELLED" ${['CANCELLED','DELIVERED'].includes(o.status) ? 'disabled' : ''}>Cancelled</option>
                        </select>
                    </td>
                    <td><button class="btn btn-ghost btn-sm" onclick="App.navigate('/orders/${o.id}')">View</button></td>
                </tr>
            `).join('');
        }

        document.getElementById('seller-orders-content').innerHTML = `
            <div class="flex justify-between items-center mb-xl flex-wrap gap-md">
                <div>
                    <button class="btn btn-ghost btn-sm mb-sm" onclick="App.navigate('/seller/dashboard')">← Dashboard</button>
                    <h1>Orders</h1>
                </div>
            </div>
            <div class="glass-card" style="overflow-x:auto">
                <table class="seller-table">
                    <thead>
                        <tr>
                            <th>Order ID</th>
                            <th>Customer</th>
                            <th>Date</th>
                            <th>Status</th>
                            <th>Update Status</th>
                            <th>Details</th>
                        </tr>
                    </thead>
                    <tbody>${tableBody}</tbody>
                </table>
            </div>`;
    },

    async updateStatus(orderId, newStatus) {
        if (!newStatus) return;
        const res = await API.updateOrderStatus(orderId, newStatus);
        if (res.ok) {
            Components.toast('Order status updated', 'success');
            this.init();
        } else {
            Components.toast(res.data.error ? res.data.error.message : 'Failed to update status', 'error');
            this.init(); // Reset dropdown
        }
    }
};
