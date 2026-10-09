/* ============================================================
   KANYAMART SHOP/MARKETPLACE PAGE
   ============================================================ */
const ShopPage = {
    products: [], filters: { search: '', category: '', sort: 'newest', min_price: '', max_price: '', min_rating: '', page: 1 },
    totalPages: 1, total: 0,
    searchTimeout: null,

    render() {
        return `
        <div class="shop-page">
            <div class="container-wide">
                <div class="shop-header">
                    <h1>Marketplace</h1>
                    <div class="search-bar" style="margin:0 auto">
                        <span class="search-icon">🔍</span>
                        <input type="text" id="shop-search" placeholder="Search products, categories and more..."
                               value="${this.filters.search}" oninput="ShopPage.onSearch(this.value)">
                        <div id="search-results" class="search-results hidden"></div>
                    </div>
                </div>
                <div class="shop-layout">
                    <aside class="filter-panel" id="filter-panel">
                        <div class="filter-section">
                            <h4>Category</h4>
                            <div class="filter-chips" id="category-chips">
                                ${['All','Electronics','Fashion','Home','Beauty','Sports','Accessories','Books','Gaming'].map(c =>
                                    `<span class="filter-chip ${this.filters.category === (c === 'All' ? '' : c) ? 'active' : ''}"
                                           onclick="ShopPage.setCategory('${c === 'All' ? '' : c}')">${c}</span>`
                                ).join('')}
                            </div>
                        </div>
                        <div class="filter-section">
                            <h4>Price Range</h4>
                            <div class="flex gap-sm">
                                <input type="number" placeholder="Min" value="${this.filters.min_price}" onchange="ShopPage.setFilter('min_price', this.value)" style="width:50%">
                                <input type="number" placeholder="Max" value="${this.filters.max_price}" onchange="ShopPage.setFilter('max_price', this.value)" style="width:50%">
                            </div>
                        </div>
                        <div class="filter-section">
                            <h4>Rating</h4>
                            <div class="filter-chips">
                                ${[0,4,3,2,1].map(r =>
                                    `<span class="filter-chip ${this.filters.min_rating == r ? 'active' : ''}"
                                           onclick="ShopPage.setFilter('min_rating', ${r})">${r === 0 ? 'All' : r + '★+'}</span>`
                                ).join('')}
                            </div>
                        </div>
                        <button class="btn btn-ghost btn-full mt-md" onclick="ShopPage.resetFilters()">Clear Filters</button>
                    </aside>
                    <div>
                        <div class="shop-sort">
                            <span class="shop-count" id="product-count">Loading...</span>
                            <div class="flex gap-sm items-center">
                                <button class="btn btn-ghost btn-sm" onclick="document.getElementById('filter-panel').classList.toggle('show')" style="display:none" id="filter-toggle-btn">
                                    ⚙ Filters
                                </button>
                                <select id="sort-select" onchange="ShopPage.setFilter('sort', this.value)">
                                    <option value="newest" ${this.filters.sort === 'newest' ? 'selected' : ''}>Newest</option>
                                    <option value="price_asc" ${this.filters.sort === 'price_asc' ? 'selected' : ''}>Price: Low → High</option>
                                    <option value="price_desc" ${this.filters.sort === 'price_desc' ? 'selected' : ''}>Price: High → Low</option>
                                    <option value="rating" ${this.filters.sort === 'rating' ? 'selected' : ''}>Top Rated</option>
                                    <option value="popular" ${this.filters.sort === 'popular' ? 'selected' : ''}>Most Popular</option>
                                </select>
                            </div>
                        </div>
                        <div id="product-grid" class="product-grid">${Components.loading('Loading products...')}</div>
                        <div id="product-pagination"></div>
                    </div>
                </div>
            </div>
        </div>`;
    },

    async init() {
        await this.loadProducts();
        // Show filter toggle on mobile
        if (window.innerWidth <= 1024) {
            const btn = document.getElementById('filter-toggle-btn');
            if (btn) btn.style.display = 'inline-flex';
        }
    },

    async loadProducts() {
        const grid = document.getElementById('product-grid');
        if (grid) grid.innerHTML = Components.loading('Loading products...');

        const res = await API.getProducts(this.filters);
        if (res.ok && res.data.data) {
            this.products = res.data.data;
            this.total = res.data.pagination ? res.data.pagination.total : this.products.length;
            this.totalPages = res.data.pagination ? res.data.pagination.total_pages : 1;
            this.renderProducts();
        } else {
            if (grid) grid.innerHTML = Components.emptyState('🔍', 'No products found', 'Try adjusting your search or filters.', 'Clear Filters', 'ShopPage.resetFilters()');
        }
    },

    renderProducts() {
        const grid = document.getElementById('product-grid');
        const countEl = document.getElementById('product-count');
        const pagEl = document.getElementById('product-pagination');
        if (!grid) return;

        if (this.products.length === 0) {
            grid.innerHTML = Components.emptyState('🔍', 'No products found', 'Try adjusting your search or filters.', 'Clear Filters', 'ShopPage.resetFilters()');
        } else {
            grid.innerHTML = this.products.map(p => Components.productCard(p)).join('');
        }

        if (countEl) countEl.textContent = `${this.total} product${this.total !== 1 ? 's' : ''} found`;
        if (pagEl) pagEl.innerHTML = Components.pagination(this.filters.page, this.totalPages, 'ShopPage.goToPage');
    },

    onSearch(value) {
        clearTimeout(this.searchTimeout);
        this.searchTimeout = setTimeout(() => {
            this.filters.search = value;
            this.filters.page = 1;
            this.loadProducts();
        }, 300);
    },

    setCategory(cat) {
        this.filters.category = cat;
        this.filters.page = 1;
        this.loadProducts();
        // Update chips
        document.querySelectorAll('#category-chips .filter-chip').forEach(chip => {
            const chipCat = chip.textContent.trim();
            chip.classList.toggle('active', (cat === '' && chipCat === 'All') || chipCat === cat);
        });
    },

    setFilter(key, value) {
        this.filters[key] = value;
        this.filters.page = 1;
        this.loadProducts();
    },

    resetFilters() {
        this.filters = { search: '', category: '', sort: 'newest', min_price: '', max_price: '', min_rating: '', page: 1 };
        const searchInput = document.getElementById('shop-search');
        if (searchInput) searchInput.value = '';
        App.navigate('/shop'); // Re-render
    },

    goToPage(page) {
        ShopPage.filters.page = page;
        ShopPage.loadProducts();
        window.scrollTo({ top: 0, behavior: 'smooth' });
    }
};
