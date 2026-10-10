/* ============================================================
   KANYAMART API CLIENT
   Centralized fetch wrapper for all API calls
   ============================================================ */

const API = {
    BASE: 'https://faalihamart-secure-api.loca.lt/api/v1',

    async request(method, path, body = null) {
        const opts = {
            method,
            headers: { 
                'Content-Type': 'application/json',
                'Bypass-Tunnel-Reminder': 'true'
            },
            credentials: 'include'
        };
        if (body) opts.body = JSON.stringify(body);

        try {
            const res = await fetch(this.BASE + path, opts);
            const data = await res.json();
            return { status: res.status, ok: res.ok, data };
        } catch (err) {
            return { status: 0, ok: false, data: { success: false, error: { code: 'NETWORK_ERROR', message: 'Cannot connect to server' } } };
        }
    },

    get(path) { return this.request('GET', path); },
    post(path, body) { return this.request('POST', path, body); },
    put(path, body) { return this.request('PUT', path, body); },
    del(path) { return this.request('DELETE', path); },

    // Auth
    register(data) { return this.post('/auth/register', data); },
    login(data) { return this.post('/auth/login', data); },
    logout() { return this.post('/auth/logout'); },
    getMe() { return this.get('/me'); },

    // Products
    getProducts(params = {}) {
        const q = new URLSearchParams();
        Object.entries(params).forEach(([k, v]) => { if (v !== '' && v !== undefined && v !== null) q.set(k, v); });
        return this.get('/products?' + q.toString());
    },
    getProduct(id) { return this.get('/products/' + id); },

    // Cart
    getCart() { return this.get('/cart'); },
    addToCart(productId, quantity = 1) { return this.post('/cart/items', { product_id: productId, quantity }); },
    updateCartItem(productId, quantity) { return this.put('/cart/items/' + productId, { quantity }); },
    removeCartItem(productId) { return this.del('/cart/items/' + productId); },

    // Orders
    createOrder() { return this.post('/orders'); },
    getOrders() { return this.get('/orders'); },
    getOrder(id) { return this.get('/orders/' + id); },

    // Seller
    getSellerDashboard() { return this.get('/seller/dashboard'); },
    getSellerProducts() { return this.get('/seller/products'); },
    createSellerProduct(data) { return this.post('/seller/products', data); },
    updateSellerProduct(id, data) { return this.put('/seller/products/' + id, data); },
    deleteSellerProduct(id) { return this.del('/seller/products/' + id); },
    getSellerOrders() { return this.get('/seller/orders'); },
    updateOrderStatus(id, status) { return this.put('/seller/orders/' + id + '/status', { status }); },

    // Reviews
    createReview(productId, data) { return this.post('/products/' + productId + '/reviews', data); },
    updateReview(id, data) { return this.put('/reviews/' + id, data); },
    deleteReview(id) { return this.del('/reviews/' + id); },

    // Health
    health() { return this.get('/health'); }
};
