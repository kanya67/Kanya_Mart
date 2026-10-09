/* ============================================================
   KANYAMART CART PAGE
   ============================================================ */
const CartPage = {
    cart: null,

    render() {
        return `<div class="cart-page"><div class="container" id="cart-content">${Components.loading('Loading cart...')}</div></div>`;
    },

    async init() {
        const res = await API.getCart();
        if (res.ok) { this.cart = res.data.data; this.renderCart(); }
        else { document.getElementById('cart-content').innerHTML = Components.emptyState('🛒', 'Unable to load cart', 'Please try again.'); }
    },

    renderCart() {
        const c = this.cart;
        const el = document.getElementById('cart-content');
        if (!c || c.items.length === 0) {
            el.innerHTML = `<h1>Shopping Cart</h1>` + Components.emptyState('🛒', 'Your cart is waiting', 'Your cart is waiting for something amazing.', 'Explore Products', "App.navigate('/shop')");
            return;
        }

        const itemsHtml = c.items.map(item => `
            <div class="cart-item animate-fade-in">
                <div class="cart-item-image">${item.image_url ? `<img src="${item.image_url}" alt="${item.name}">` : '📦'}</div>
                <div>
                    <div class="cart-item-name" style="cursor:pointer" onclick="App.navigate('/product/${item.product_id}')">${item.name}</div>
                    <div class="text-sm text-muted">${item.category} · ${item.seller_name}</div>
                </div>
                <div class="cart-item-price">₹${Number(item.price).toLocaleString('en-IN')}</div>
                ${Components.qtySelector(item.product_id, item.quantity, item.stock)}
                <div class="cart-item-subtotal">₹${Number(item.subtotal).toLocaleString('en-IN')}</div>
                <span class="cart-item-remove" onclick="CartPage.removeItem('${item.product_id}')" title="Remove">✕</span>
            </div>`).join('');

        el.innerHTML = `
            <h1>Shopping Cart</h1>
            <div class="cart-layout">
                <div>${itemsHtml}</div>
                <div class="cart-summary">
                    <h3>Order Summary</h3>
                    <div class="cart-summary-row"><span class="label">Items (${c.item_count})</span><span>₹${Number(c.total).toLocaleString('en-IN')}</span></div>
                    <div class="cart-summary-row"><span class="label">Shipping</span><span style="color:var(--accent-green)">Free</span></div>
                    <div class="cart-summary-total"><span>Total</span><span class="amount">₹${Number(c.total).toLocaleString('en-IN')}</span></div>
                    <button class="btn btn-primary btn-full btn-lg mt-lg" onclick="App.navigate('/checkout')">Proceed to Checkout</button>
                    <button class="btn btn-ghost btn-full mt-sm" onclick="App.navigate('/shop')">Continue Shopping</button>
                </div>
            </div>`;
    },

    async updateQty(productId, qty) {
        if (qty <= 0) { this.removeItem(productId); return; }
        const res = await API.updateCartItem(productId, qty);
        if (res.ok) { this.init(); App.updateCartCount(); }
        else Components.toast(res.data.error ? res.data.error.message : 'Failed to update', 'error');
    },

    async removeItem(productId) {
        const res = await API.removeCartItem(productId);
        if (res.ok) { Components.toast('Item removed', 'info'); this.init(); App.updateCartCount(); }
        else Components.toast('Failed to remove item', 'error');
    }
};
