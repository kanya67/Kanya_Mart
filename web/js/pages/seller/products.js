/* ============================================================
   KANYAMART SELLER PRODUCTS PAGE
   ============================================================ */
const SellerProductsPage = {
    products: [],

    render() {
        return `<div class="seller-page"><div class="container-wide" id="seller-products-content">${Components.loading('Loading products...')}</div></div>`;
    },

    async init() {
        const res = await API.getSellerProducts();
        if (res.ok) { this.products = res.data.data; this.renderProducts(); }
        else { document.getElementById('seller-products-content').innerHTML = Components.emptyState('📦', 'Unable to load products', 'Please try again.'); }
    },

    renderProducts() {
        let tableBody = '';
        if (this.products.length === 0) {
            tableBody = `<tr><td colspan="7" style="text-align:center;padding:var(--space-xl)">No products found. Start by creating one.</td></tr>`;
        } else {
            tableBody = this.products.map(p => `
                <tr>
                    <td>
                        <div class="flex items-center gap-sm">
                            <div style="width:40px;height:40px;border-radius:var(--radius-sm);background:var(--bg-tertiary);overflow:hidden;display:flex;align-items:center;justify-content:center">
                                ${p.image_url ? `<img src="${p.image_url}" style="width:100%;height:100%;object-fit:cover">` : '📦'}
                            </div>
                            <div style="font-weight:600;max-width:150px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis" title="${p.name}">${p.name}</div>
                        </div>
                    </td>
                    <td>${p.category}</td>
                    <td style="font-family:var(--font-heading);font-weight:600">₹${Number(p.price).toLocaleString('en-IN')}</td>
                    <td><span style="color:${p.stock <= 0 ? 'var(--accent-red)' : p.stock < 10 ? 'var(--accent-orange)' : 'var(--accent-green)'}">${p.stock}</span></td>
                    <td>${p.is_active ? '<span class="badge badge-delivered">Active</span>' : '<span class="badge badge-cancelled">Inactive</span>'}</td>
                    <td>${p.avg_rating > 0 ? `${parseFloat(p.avg_rating).toFixed(1)}★ (${p.review_count})` : '-'}</td>
                    <td>
                        <div class="actions">
                            <button class="btn btn-ghost btn-sm" onclick="SellerProductsPage.openModal('${p.id}')">Edit</button>
                            ${p.is_active ? `<button class="btn btn-danger btn-sm" onclick="SellerProductsPage.deleteProduct('${p.id}')">Deactivate</button>` : ''}
                        </div>
                    </td>
                </tr>
            `).join('');
        }

        document.getElementById('seller-products-content').innerHTML = `
            <div class="flex justify-between items-center mb-xl flex-wrap gap-md">
                <div>
                    <button class="btn btn-ghost btn-sm mb-sm" onclick="App.navigate('/seller/dashboard')">← Dashboard</button>
                    <h1>Products</h1>
                </div>
                <button class="btn btn-primary" onclick="SellerProductsPage.openModal()">+ Add Product</button>
            </div>
            <div class="glass-card" style="overflow-x:auto">
                <table class="seller-table">
                    <thead>
                        <tr>
                            <th>Product</th>
                            <th>Category</th>
                            <th>Price</th>
                            <th>Stock</th>
                            <th>Status</th>
                            <th>Rating</th>
                            <th>Actions</th>
                        </tr>
                    </thead>
                    <tbody>${tableBody}</tbody>
                </table>
            </div>`;
    },

    openModal(id = null) {
        let p = null;
        if (id) {
            p = this.products.find(x => x.id === id);
            if (!p) return;
        }

        const title = p ? 'Edit Product' : 'Add New Product';
        const cats = ['Electronics','Fashion','Home','Beauty','Sports','Accessories','Books','Gaming'];
        const content = `
            <form id="product-form" onsubmit="SellerProductsPage.saveProduct(event, '${id || ''}')">
                <div class="form-group">
                    <label>Name</label>
                    <input type="text" id="p-name" value="${p ? p.name : ''}" required>
                </div>
                <div class="form-group">
                    <label>Description</label>
                    <textarea id="p-desc" rows="3">${p ? (p.description || '') : ''}</textarea>
                </div>
                <div class="flex gap-md">
                    <div class="form-group" style="flex:1">
                        <label>Category</label>
                        <select id="p-category" required>
                            <option value="">Select Category</option>
                            ${cats.map(c => `<option value="${c}" ${p && p.category === c ? 'selected' : ''}>${c}</option>`).join('')}
                        </select>
                    </div>
                    <div class="form-group" style="flex:1">
                        <label>Price (₹)</label>
                        <input type="number" id="p-price" step="0.01" min="1" value="${p ? p.price : ''}" required>
                    </div>
                </div>
                <div class="flex gap-md">
                    <div class="form-group" style="flex:1">
                        <label>Stock Quantity</label>
                        <input type="number" id="p-stock" min="0" value="${p ? p.stock : 0}" required>
                    </div>
                    <div class="form-group" style="flex:1">
                        <label>Image URL</label>
                        <input type="url" id="p-image" placeholder="https://" value="${p ? (p.image_url || '') : ''}">
                    </div>
                </div>
                ${p ? `
                <div class="form-group">
                    <label class="flex items-center gap-sm">
                        <input type="checkbox" id="p-active" ${p.is_active ? 'checked' : ''} style="width:auto"> Active
                    </label>
                </div>` : ''}
                <div class="flex gap-md mt-lg justify-end">
                    <button type="button" class="btn btn-ghost" onclick="Components.closeModal()">Cancel</button>
                    <button type="submit" class="btn btn-primary" id="p-save-btn">Save Product</button>
                </div>
            </form>`;
        Components.showModal(title, content);
    },

    async saveProduct(e, id) {
        e.preventDefault();
        const btn = document.getElementById('p-save-btn');
        btn.disabled = true; btn.textContent = 'Saving...';

        const data = {
            name: document.getElementById('p-name').value,
            description: document.getElementById('p-desc').value,
            category: document.getElementById('p-category').value,
            price: parseFloat(document.getElementById('p-price').value),
            stock: parseInt(document.getElementById('p-stock').value, 10),
            image_url: document.getElementById('p-image').value
        };

        if (id) {
            const activeEl = document.getElementById('p-active');
            if (activeEl) data.is_active = activeEl.checked;
        }

        let res;
        if (id) res = await API.updateSellerProduct(id, data);
        else res = await API.createSellerProduct(data);

        if (res.ok) {
            Components.closeModal();
            Components.toast(id ? 'Product updated' : 'Product created', 'success');
            this.init();
        } else {
            btn.disabled = false; btn.textContent = 'Save Product';
            Components.toast(res.data.error ? res.data.error.message : 'Failed to save', 'error');
        }
    },

    deleteProduct(id) {
        Components.confirm('Are you sure you want to deactivate this product? It will no longer be visible to customers.', async () => {
            const res = await API.deleteSellerProduct(id);
            if (res.ok) {
                Components.toast('Product deactivated', 'success');
                this.init();
            } else {
                const errMsg = (res.data && res.data.error && res.data.error.message) ? res.data.error.message : 'Failed to deactivate';
                Components.toast(errMsg, 'error');
            }
        });
    }
};
