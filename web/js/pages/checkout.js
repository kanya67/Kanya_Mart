/* ============================================================
   KANYAMART CHECKOUT PAGE
   ============================================================ */
const CheckoutPage = {
    cart: null,

    render() {
        return `<div class="checkout-page"><div class="container" id="checkout-content">${Components.loading('Loading checkout...')}</div></div>`;
    },

    async init() {
        const res = await API.getCart();
        if (res.ok && res.data.data && res.data.data.items.length > 0) { this.cart = res.data.data; this.renderCheckout(); }
        else { document.getElementById('checkout-content').innerHTML = Components.emptyState('🛒', 'Cart is empty', 'Add some products before checking out.', 'Go Shopping', "App.navigate('/shop')"); }
    },

    renderCheckout() {
        const c = this.cart;
        const itemsHtml = c.items.map(item => `
            <div class="flex items-center justify-between" style="padding:var(--space-sm) 0;border-bottom:1px solid var(--glass-border)">
                <div><span style="font-weight:600">${item.name}</span> <span class="text-muted">× ${item.quantity}</span></div>
                <span style="font-family:var(--font-heading);font-weight:600">₹${Number(item.subtotal).toLocaleString('en-IN')}</span>
            </div>`).join('');

        document.getElementById('checkout-content').innerHTML = `
            <h1>Checkout</h1>
            <div class="cart-layout">
                <div class="glass-card" style="padding:var(--space-xl)">
                    <h3 style="font-family:var(--font-heading);margin-bottom:var(--space-lg)">Order Items</h3>
                    ${itemsHtml}
                </div>
                <div class="cart-summary">
                    <h3>Payment Summary</h3>
                    <div class="cart-summary-row"><span class="label">Subtotal</span><span>₹${Number(c.total).toLocaleString('en-IN')}</span></div>
                    <div class="cart-summary-row"><span class="label">Shipping</span><span style="color:var(--accent-green)">Free</span></div>
                    <div class="cart-summary-total"><span>Total</span><span class="amount">₹${Number(c.total).toLocaleString('en-IN')}</span></div>
                    <button class="btn btn-primary btn-full btn-lg mt-lg" id="place-order-btn" onclick="CheckoutPage.placeOrder()">
                        🔒 Place Order
                    </button>
                    <button class="btn btn-ghost btn-full mt-sm" onclick="App.navigate('/cart')">← Back to Cart</button>
                </div>
            </div>`;
    },

    async placeOrder() {
        const btn = document.getElementById('place-order-btn');
        btn.disabled = true; btn.textContent = 'Processing...';

        const res = await API.createOrder();
        if (res.ok) {
            const order = res.data.data;
            App.updateCartCount();
            document.getElementById('checkout-content').innerHTML = `
                <div class="order-success animate-fade-in">
                    <div class="icon">🎉</div>
                    <h2>ORDER CONFIRMED</h2>
                    <p class="text-muted" style="font-size:16px;margin-bottom:var(--space-md)">Your order has been placed successfully!</p>
                    <div class="glass-card" style="padding:var(--space-lg);text-align:left;min-width:300px">
                        <div class="flex justify-between mb-md"><span class="text-muted">Order ID</span><span style="font-family:var(--font-heading);font-weight:600">${order.id.substring(0,8)}...</span></div>
                        <div class="flex justify-between mb-md"><span class="text-muted">Total</span><span style="color:var(--accent-blue);font-weight:700;font-family:var(--font-heading)">₹${Number(order.total).toLocaleString('en-IN')}</span></div>
                        <div class="flex justify-between"><span class="text-muted">Status</span>${Components.statusBadge(order.status)}</div>
                    </div>
                    <div class="flex gap-md mt-lg">
                        <button class="btn btn-primary" onclick="App.navigate('/orders/${order.id}')">View Order</button>
                        <button class="btn btn-secondary" onclick="App.navigate('/shop')">Continue Shopping</button>
                    </div>
                </div>`;
        } else {
            btn.disabled = false; btn.textContent = '🔒 Place Order';
            Components.toast(res.data.error ? res.data.error.message : 'Failed to place order', 'error');
        }
    }
};
