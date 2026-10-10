/* ============================================================
   KANYAMART PRODUCT DETAIL PAGE
   ============================================================ */
const ProductPage = {
    product: null, quantity: 1,

    render(id) {
        return `<div class="product-detail-page"><div class="container" id="product-content">${Components.loading('Loading product...')}</div></div>`;
    },

    async init(id) {
        const res = await API.getProduct(id);
        if (!res.ok) { document.getElementById('product-content').innerHTML = Components.emptyState('📦', 'Product not found', 'This product may have been removed.', 'Back to Shop', "App.navigate('/shop')"); return; }
        this.product = res.data.data;
        this.quantity = 1;
        this.renderDetail();
    },

    renderDetail() {
        const p = this.product;
        const rating = parseFloat(p.avg_rating || 0).toFixed(1);
        const stockClass = p.stock <= 0 ? 'out' : p.stock < 10 ? 'low' : '';
        const stockText = p.stock <= 0 ? 'Out of Stock' : p.stock < 10 ? `Only ${p.stock} left!` : `${p.stock} in stock`;
        const imgHtml = p.image_url ? `<img src="${p.image_url}" alt="${p.name}">` : '<div class="placeholder-icon">📦</div>';

        let reviewsHtml = '';
        if (p.reviews && p.reviews.length > 0) {
            reviewsHtml = p.reviews.map(r => `
                <div class="review-card animate-fade-in">
                    <div class="review-card-header">
                        <div>
                            <span class="review-card-user">${r.username}</span>
                            ${Components.ratingStars(r.rating)}
                        </div>
                        <span class="review-card-date">${r.created_at}</span>
                    </div>
                    <p class="review-card-comment">${r.comment || 'No comment'}</p>
                </div>`).join('');
        } else {
            reviewsHtml = Components.emptyState('💬', 'No reviews yet', 'Be the first to review this product.');
        }

        document.getElementById('product-content').innerHTML = `
            <button class="btn btn-ghost btn-sm mb-lg" onclick="App.navigate('/shop')">← Back to Shop</button>
            <div class="product-detail-layout animate-fade-in">
                <div class="product-detail-image">${imgHtml}</div>
                <div class="product-detail-info">
                    <span class="badge badge-confirmed" style="margin-bottom:var(--space-sm)">${p.category}</span>
                    <h1>${p.name}</h1>
                    <div class="product-card-rating mt-sm">
                        ${Components.ratingStars(rating, p.review_count)}
                        <span style="color:var(--text-secondary);font-size:14px;margin-left:8px">${rating} out of 5</span>
                    </div>
                    <div class="price">₹${Number(p.price).toLocaleString('en-IN')}</div>
                    <p class="description">${p.description || 'No description available.'}</p>
                    <div class="meta-row"><span class="label">Seller</span><span>${p.seller_name || 'FaalihaMart'}</span></div>
                    <div class="meta-row"><span class="label">Availability</span><span class="product-card-stock ${stockClass}">${stockText}</span></div>
                    ${p.stock > 0 ? `
                    <div class="meta-row">
                        <span class="label">Quantity</span>
                        <div class="qty-selector">
                            <button onclick="ProductPage.setQty(${Math.max(1, this.quantity - 1)})">−</button>
                            <span class="qty-value" id="product-qty">${this.quantity}</span>
                            <button onclick="ProductPage.setQty(${Math.min(this.quantity + 1, p.stock)})">+</button>
                        </div>
                    </div>
                    <div class="product-detail-actions">
                        <button class="btn btn-primary btn-lg" onclick="ProductPage.addToCart()">🛒 Add to Cart</button>
                        <button class="btn btn-secondary btn-lg" onclick="ProductPage.buyNow()">⚡ Buy Now</button>
                    </div>` : '<div class="mt-lg"><button class="btn btn-ghost btn-lg" disabled>Out of Stock</button></div>'}
                </div>
            </div>
            <div class="product-reviews-section">
                <h2>Reviews & Ratings</h2>
                <div id="review-form-area"></div>
                ${reviewsHtml}
            </div>`;

        // Show review form if user has purchased
        this.checkReviewEligibility();
    },

    setQty(q) {
        this.quantity = q;
        const el = document.getElementById('product-qty');
        if (el) el.textContent = q;
    },

    async addToCart() {
        const res = await API.addToCart(this.product.id, this.quantity);
        if (res.ok) { Components.toast('Added to cart!', 'success'); App.updateCartCount(); }
        else Components.toast(res.data.error ? res.data.error.message : 'Failed to add to cart', 'error');
    },

    async buyNow() {
        const res = await API.addToCart(this.product.id, this.quantity);
        if (res.ok) { App.updateCartCount(); App.navigate('/cart'); }
        else Components.toast(res.data.error ? res.data.error.message : 'Failed', 'error');
    },

    async checkReviewEligibility() {
        // Check if user is logged in first
        if (!App.user) return;
        const area = document.getElementById('review-form-area');
        if (!area) return;
        // Show review form (backend will validate purchase)
        area.innerHTML = `
            <div class="glass-card" style="padding:var(--space-lg);margin-bottom:var(--space-lg)">
                <h4 style="font-family:var(--font-heading);margin-bottom:var(--space-md)">Write a Review</h4>
                <div class="form-group">
                    <label>Rating</label>
                    <div class="flex gap-sm" id="review-rating-input">
                        ${[1,2,3,4,5].map(i => `<span style="cursor:pointer;font-size:24px;color:var(--text-muted)" onclick="ProductPage.setReviewRating(${i})" id="review-star-${i}">☆</span>`).join('')}
                    </div>
                </div>
                <div class="form-group">
                    <label>Comment</label>
                    <textarea id="review-comment" rows="3" placeholder="Share your experience..."></textarea>
                </div>
                <button class="btn btn-primary" onclick="ProductPage.submitReview()">Submit Review</button>
            </div>`;
    },

    reviewRating: 0,
    setReviewRating(r) {
        this.reviewRating = r;
        for (let i = 1; i <= 5; i++) {
            const star = document.getElementById('review-star-' + i);
            if (star) { star.textContent = i <= r ? '★' : '☆'; star.style.color = i <= r ? 'var(--accent-orange)' : 'var(--text-muted)'; }
        }
    },

    async submitReview() {
        if (this.reviewRating < 1) { Components.toast('Please select a rating', 'error'); return; }
        const comment = document.getElementById('review-comment').value;
        const res = await API.createReview(this.product.id, { rating: this.reviewRating, comment });
        if (res.ok) { Components.toast('Review submitted!', 'success'); this.init(this.product.id); }
        else Components.toast(res.data.error ? res.data.error.message : 'Failed to submit review', 'error');
    }
};
