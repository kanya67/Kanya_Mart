/* ============================================================
   KANYAMART REUSABLE COMPONENTS
   ============================================================ */

const Components = {
    // Toast notifications
    toast(message, type = 'info', duration = 3000) {
        const container = document.getElementById('toast-container');
        const toast = document.createElement('div');
        toast.className = `toast ${type}`;
        const icons = { success: '✓', error: '✗', info: 'ℹ' };
        toast.innerHTML = `<span>${icons[type] || 'ℹ'}</span><span>${message}</span><span class="toast-close" onclick="this.parentElement.remove()">×</span>`;
        container.appendChild(toast);
        setTimeout(() => { toast.style.opacity = '0'; toast.style.transform = 'translateX(30px)'; setTimeout(() => toast.remove(), 300); }, duration);
    },

    // Loading spinner
    loading(text = 'Loading...') {
        return `<div class="loading-container"><div class="spinner"></div><p>${text}</p></div>`;
    },

    // Empty state
    emptyState(icon, title, message, btnText, btnAction) {
        let btn = btnText ? `<button class="btn btn-primary" onclick="${btnAction}">${btnText}</button>` : '';
        return `<div class="empty-state"><div class="icon">${icon}</div><h3>${title}</h3><p>${message}</p>${btn}</div>`;
    },

    // Rating stars
    ratingStars(rating, count = null) {
        let stars = '';
        for (let i = 1; i <= 5; i++) {
            if (i <= Math.floor(rating)) stars += '<span class="star filled">★</span>';
            else if (i === Math.ceil(rating) && rating % 1 >= 0.5) stars += '<span class="star filled">★</span>';
            else stars += '<span class="star">☆</span>';
        }
        let countHtml = count !== null ? `<span class="count">(${count})</span>` : '';
        return `<div class="rating-stars">${stars} ${countHtml}</div>`;
    },

    // Status badge
    statusBadge(status) {
        const s = status.toLowerCase().replace(/ /g, '_');
        const labels = {
            'pending': 'Pending', 'confirmed': 'Confirmed', 'processing': 'Processing',
            'shipped': 'Shipped', 'out_for_delivery': 'Out for Delivery',
            'delivered': 'Delivered', 'cancelled': 'Cancelled'
        };
        return `<span class="badge badge-${s}">${labels[s] || status}</span>`;
    },

    // Order timeline
    orderTimeline(currentStatus) {
        const steps = ['PENDING', 'CONFIRMED', 'PROCESSING', 'SHIPPED', 'OUT_FOR_DELIVERY', 'DELIVERED'];
        const icons = ['📋', '✓', '⚙', '🚚', '📍', '📦'];
        const labels = ['Placed', 'Confirmed', 'Processing', 'Shipped', 'Out for Delivery', 'Delivered'];
        const currentIdx = steps.indexOf(currentStatus);

        if (currentStatus === 'CANCELLED') {
            return `<div class="order-timeline"><div class="timeline-step active"><div class="timeline-dot" style="background:var(--accent-red);border-color:var(--accent-red)">✗</div><div class="timeline-label" style="color:var(--accent-red)">Cancelled</div></div></div>`;
        }

        let html = '<div class="order-timeline">';
        steps.forEach((step, i) => {
            let cls = '';
            if (i < currentIdx) cls = 'completed';
            else if (i === currentIdx) cls = 'active';
            html += `<div class="timeline-step ${cls}"><div class="timeline-dot">${i <= currentIdx ? '✓' : icons[i]}</div><div class="timeline-label">${labels[i]}</div></div>`;
        });
        html += '</div>';
        return html;
    },

    // Product card
    productCard(product) {
        const stockClass = product.stock <= 0 ? 'out' : product.stock < 10 ? 'low' : '';
        const stockText = product.stock <= 0 ? 'Out of Stock' : product.stock < 10 ? `Only ${product.stock} left` : 'In Stock';
        const rating = product.avg_rating ? parseFloat(product.avg_rating).toFixed(1) : '0.0';
        const reviewCount = product.review_count || 0;
        const imgHtml = product.image_url ? `<img src="${product.image_url}" alt="${product.name}" loading="lazy">` : `<div class="placeholder-icon">📦</div>`;

        return `
        <div class="product-card animate-fade-in" onclick="App.navigate('/product/${product.id}')">
            <div class="product-card-image">
                ${imgHtml}
                <span class="category-tag">${product.category}</span>
            </div>
            <div class="product-card-body">
                <div class="product-card-name">${product.name}</div>
                <div class="product-card-seller">by ${product.seller_name || 'KanyaMart'}</div>
                <div class="product-card-rating">
                    ${this.ratingStars(rating)}
                    <span class="count">${rating} (${reviewCount})</span>
                </div>
                <div class="product-card-footer">
                    <div>
                        <div class="product-card-price"><span class="currency">₹</span>${Number(product.price).toLocaleString('en-IN')}</div>
                        <div class="product-card-stock ${stockClass}">${stockText}</div>
                    </div>
                    ${product.stock > 0 ? `<button class="add-to-cart-btn" onclick="event.stopPropagation(); App.addToCart('${product.id}')" title="Add to Cart">+</button>` : ''}
                </div>
            </div>
        </div>`;
    },

    // Pagination
    pagination(page, totalPages, onPageChange) {
        if (totalPages <= 1) return '';
        let html = '<div class="pagination">';
        html += `<button ${page <= 1 ? 'disabled' : ''} onclick="${onPageChange}(${page - 1})">‹</button>`;
        for (let i = 1; i <= Math.min(totalPages, 7); i++) {
            html += `<button class="${i === page ? 'active' : ''}" onclick="${onPageChange}(${i})">${i}</button>`;
        }
        html += `<button ${page >= totalPages ? 'disabled' : ''} onclick="${onPageChange}(${page + 1})">›</button>`;
        html += '</div>';
        return html;
    },

    // Quantity selector
    qtySelector(productId, quantity, max = 999) {
        return `
        <div class="qty-selector">
            <button onclick="CartPage.updateQty('${productId}', ${quantity - 1})">−</button>
            <span class="qty-value">${quantity}</span>
            <button onclick="CartPage.updateQty('${productId}', ${Math.min(quantity + 1, max)})">+</button>
        </div>`;
    },

    // Modal
    showModal(title, content) {
        const existing = document.querySelector('.modal-overlay');
        if (existing) existing.remove();
        const overlay = document.createElement('div');
        overlay.className = 'modal-overlay';
        overlay.onclick = (e) => { if (e.target === overlay) overlay.remove(); };
        overlay.innerHTML = `<div class="modal"><div class="modal-header"><h3>${title}</h3><span class="modal-close" onclick="this.closest('.modal-overlay').remove()">×</span></div><div class="modal-body">${content}</div></div>`;
        document.body.appendChild(overlay);
        return overlay;
    },

    closeModal() {
        const overlay = document.querySelector('.modal-overlay');
        if (overlay) overlay.remove();
    },

    // Confirm dialog
    confirm(message, onConfirm) {
        const content = `<p style="color:var(--text-secondary);margin-bottom:var(--space-lg)">${message}</p><div class="flex gap-md justify-center"><button class="btn btn-ghost" onclick="Components.closeModal()">Cancel</button><button class="btn btn-primary" onclick="Components.closeModal(); (${onConfirm})()">Confirm</button></div>`;
        this.showModal('Confirm', content);
    }
};
