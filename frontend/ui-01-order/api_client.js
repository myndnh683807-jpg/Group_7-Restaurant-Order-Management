// frontend/ui-01-order/api_client.js
// Client API & State Manager cho Giao diện Đặt Món CukCuk POS (UI-01)

const API_BASE = 'http://localhost:8080/api';

// State nội bộ ứng dụng
let state = {
    categories: [],
    menuItems: [],
    tables: [],
    selectedCategoryId: null,
    selectedTableId: 1,
    searchQuery: '',
    cart: [], // { item_id, name, price, quantity, special_note }
};

// --- API Service (Giữ nguyên toàn bộ logic gọi Backend C++ Crow) ---
const Api = {
    async getCategories() {
        try {
            const res = await fetch(`${API_BASE}/menu`);
            const data = await res.json();
            return data.success && data.data ? data.data : [];
        } catch (err) {
            console.warn('[API] Dùng Mock Categories khi Backend chưa kết nối DB:', err);
            return [
                { category_id: 1, category_name: 'Khai vị (Appetizers)' },
                { category_id: 2, category_name: 'Món chính (Main Courses)' },
                { category_id: 3, category_name: 'Đồ uống (Beverages)' },
                { category_id: 4, category_name: 'Tráng miệng (Desserts)' }
            ];
        }
    },

    async getMenuItems(categoryId = null) {
        try {
            let url = `${API_BASE}/menu/items`;
            if (categoryId) url += `?category_id=${categoryId}`;
            const res = await fetch(url);
            const data = await res.json();
            return data.success && data.data ? data.data : [];
        } catch (err) {
            console.warn('[API] Dùng Mock Menu Items:', err);
            const mock = [
                { item_id: 1, category_id: 1, item_name: 'Nem Cuốn Tôm Thịt', price: 5.50, description: 'Gỏi cuốn tôm thịt rau sống chấm tương đậu tươi mát', image_url: 'spring_roll.jpg' },
                { item_id: 2, category_id: 1, item_name: 'Khoai Tây Chiên Bơ Tỏi', price: 4.00, description: 'Khoai tây Mỹ chiên vàng giòn sốt bơ tỏi thơm lừng', image_url: 'fries.jpg' },
                { item_id: 3, category_id: 2, item_name: 'Gà Nướng Mật Ong Rừng', price: 12.50, description: 'Đùi gà nướng sốt mật ong đậm đà ăn kèm xôi dẻo', image_url: 'chicken.jpg' },
                { item_id: 4, category_id: 2, item_name: 'Bò Bít Tết Thượng Hạng', price: 18.00, description: 'Thăn bò Úc sốt tiêu đen nướng than hoa thượng hạng', image_url: 'steak.jpg' },
                { item_id: 5, category_id: 2, item_name: 'Lẩu Hải Sản Chua Cay', price: 22.00, description: 'Tôm mực nghêu tươi sống kèm rau nấm thảo mộc', image_url: 'hotpot.jpg' },
                { item_id: 6, category_id: 3, item_name: 'Nước Cam Sành Tươi Ép', price: 3.50, description: 'Cam sành Tiền Giang vắt tươi nguyên chất 100%', image_url: 'orange.jpg' },
                { item_id: 7, category_id: 3, item_name: 'Cà Phê Sữa Đá Sài Gòn', price: 2.50, description: 'Cà phê Robusta pha phin truyền thống đậm đà', image_url: 'coffee.jpg' },
                { item_id: 8, category_id: 3, item_name: 'Trà Đào Cam Sả Tươi', price: 3.80, description: 'Trà đào thơm thảo mộc kết hợp vị cam sả thanh mát', image_url: 'peach_tea.jpg' },
                { item_id: 9, category_id: 4, item_name: 'Kem Vani Hạnh Nhân Pháp', price: 4.50, description: 'Kem Vani sữa béo rắc hạt hạnh nhân sấy giòn tan', image_url: 'icecream.jpg' }
            ];
            return categoryId ? mock.filter(m => m.category_id === categoryId) : mock;
        }
    },

    async getTables() {
        try {
            const res = await fetch(`${API_BASE}/tables`);
            const data = await res.json();
            return data.success && data.data ? data.data : [];
        } catch (err) {
            return [
                { table_id: 1, table_number: 'Bàn 01', status: 'Available' },
                { table_id: 2, table_number: 'Bàn 02', status: 'Occupied' },
                { table_id: 3, table_number: 'Bàn 03', status: 'Reserved' },
                { table_id: 4, table_number: 'Bàn 04', status: 'Available' },
                { table_id: 5, table_number: 'Bàn 05', status: 'Available' }
            ];
        }
    },

    async createOrder(tableId, items) {
        const payload = {
            table_id: parseInt(tableId),
            items: items.map(it => ({
                item_id: it.item_id,
                quantity: it.quantity,
                special_note: it.special_note || ''
            }))
        };

        const res = await fetch(`${API_BASE}/orders`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });
        return await res.json();
    }
};

// --- Cart Actions ---
function addToCart(item) {
    const existing = state.cart.find(c => c.item_id === item.item_id);
    if (existing) {
        existing.quantity += 1;
    } else {
        state.cart.push({
            item_id: item.item_id,
            name: item.item_name || item.name,
            price: Number(item.price),
            quantity: 1,
            special_note: ''
        });
    }
    renderCart();
    showToast(`Đã thêm "${item.item_name || item.name}" vào đơn hàng!`, 'success');
}

function updateCartQuantity(itemId, delta) {
    const item = state.cart.find(c => c.item_id === itemId);
    if (!item) return;
    item.quantity += delta;
    if (item.quantity <= 0) {
        state.cart = state.cart.filter(c => c.item_id !== itemId);
    }
    renderCart();
}

function removeCartItem(itemId) {
    state.cart = state.cart.filter(c => c.item_id !== itemId);
    renderCart();
}

function clearCart() {
    if (state.cart.length === 0) return;
    state.cart = [];
    renderCart();
    showToast('Đã xóa toàn bộ đơn hàng.', 'secondary');
}

function setSpecialNote(itemId, note) {
    const item = state.cart.find(c => c.item_id === itemId);
    if (item) item.special_note = note;
}

// --- Render UI CukCuk POS Style ---
function renderCategories() {
    const container = document.getElementById('categoryTabs');
    if (!container) return;

    let html = `
        <button class="category-btn ${state.selectedCategoryId === null ? 'active' : ''}" onclick="selectCategory(null)">
            <span><i class="bi bi-grid-fill me-2 text-primary"></i>Tất cả món</span>
            <span class="badge bg-light text-secondary border">${state.menuItems.length}</span>
        </button>
    `;

    const catIcons = ['🥗', '🥩', '🍹', '🍨', '🍜', '🍕', '🍰'];

    state.categories.forEach((cat, idx) => {
        const isActive = state.selectedCategoryId === cat.category_id;
        const count = state.menuItems.filter(m => m.category_id === cat.category_id).length;
        const icon = catIcons[idx % catIcons.length];
        html += `
            <button class="category-btn ${isActive ? 'active' : ''}" onclick="selectCategory(${cat.category_id})">
                <span><span class="me-2">${icon}</span>${cat.category_name}</span>
                <span class="badge ${isActive ? 'bg-primary text-white' : 'bg-light text-secondary border'}">${count}</span>
            </button>
        `;
    });

    container.innerHTML = html;
}

function renderMenuItems() {
    const container = document.getElementById('menuGrid');
    const countEl = document.getElementById('menuItemCount');
    if (!container) return;

    let filtered = state.selectedCategoryId 
        ? state.menuItems.filter(i => i.category_id === state.selectedCategoryId)
        : state.menuItems;

    if (state.searchQuery) {
        const q = state.searchQuery.toLowerCase().trim();
        filtered = filtered.filter(i => (i.item_name || i.name || '').toLowerCase().includes(q));
    }

    if (countEl) {
        countEl.innerText = `${filtered.length} món ăn`;
    }

    if (filtered.length === 0) {
        container.innerHTML = `
            <div class="col-12 text-center py-5">
                <i class="bi bi-search fs-1 text-muted d-block mb-2"></i>
                <div class="fw-bold text-secondary">Không tìm thấy món ăn phù hợp</div>
                <small class="text-muted">Vui lòng thử tìm từ khóa khác hoặc chọn nhóm thực đơn khác.</small>
            </div>`;
        return;
    }

    const foodIcons = ['🍲', '🥩', '🍗', '🥗', '🥘', '🍹', '☕', '🥤', '🍨', '🍰'];

    container.innerHTML = filtered.map((item, idx) => {
        const icon = foodIcons[idx % foodIcons.length];
        const itemJson = JSON.stringify(item).replace(/"/g, '&quot;');
        return `
        <div class="col-6 col-md-4 col-xl-3">
            <div class="food-card" onclick="addToCart(${itemJson})">
                <div class="food-img-wrapper">
                    <span>${icon}</span>
                    <span class="food-badge-price">$${Number(item.price).toFixed(2)}</span>
                </div>
                <div class="food-card-body">
                    <h6 class="food-title">${item.item_name || item.name}</h6>
                    <p class="food-desc">${item.description || 'Món ăn chế biến theo công thức chuẩn hương vị nhà hàng.'}</p>
                    <button class="btn btn-pos-add mt-auto" onclick="event.stopPropagation(); addToCart(${itemJson})">
                        <i class="bi bi-plus-lg"></i> Chọn Món
                    </button>
                </div>
            </div>
        </div>
        `;
    }).join('');
}

function renderCart() {
    const container = document.getElementById('cartItems');
    const totalEl = document.getElementById('cartTotal');
    const vatEl = document.getElementById('cartVat');
    const finalEl = document.getElementById('cartFinal');
    const badgeEl = document.getElementById('cartCount');
    const submitBtn = document.getElementById('btnSubmitOrder');

    const totalCount = state.cart.reduce((s, i) => s + i.quantity, 0);
    const subtotal = state.cart.reduce((s, i) => s + (i.price * i.quantity), 0);
    const vat = subtotal * 0.08;
    const finalTotal = subtotal + vat;

    if (badgeEl) badgeEl.innerText = totalCount;
    if (totalEl) totalEl.innerText = `$${subtotal.toFixed(2)}`;
    if (vatEl) vatEl.innerText = `$${vat.toFixed(2)}`;
    if (finalEl) finalEl.innerText = `$${finalTotal.toFixed(2)}`;
    if (submitBtn) submitBtn.disabled = state.cart.length === 0;

    if (!container) return;

    if (state.cart.length === 0) {
        container.innerHTML = `
            <div class="text-center py-5 text-muted">
                <i class="bi bi-inbox fs-1 text-secondary opacity-50 d-block mb-2"></i>
                <div class="fw-semibold text-secondary">Chưa có món nào trong đơn</div>
                <small class="text-muted">Chạm hoặc nhấn vào món ăn ở bảng giữa để ghi order</small>
            </div>`;
        return;
    }

    container.innerHTML = state.cart.map(item => `
        <div class="cart-item-row">
            <div class="d-flex justify-content-between align-items-start mb-1">
                <div class="me-2">
                    <span class="cart-item-title">${item.name}</span>
                    <div class="text-muted" style="font-size: 0.76rem;">$${item.price.toFixed(2)} / phần</div>
                </div>
                <div class="text-end">
                    <span class="cart-item-price">$${(item.price * item.quantity).toFixed(2)}</span>
                    <button class="btn btn-link text-danger p-0 ms-2 text-decoration-none" onclick="removeCartItem(${item.item_id})" title="Xóa món">
                        <i class="bi bi-x-circle"></i>
                    </button>
                </div>
            </div>

            <div class="d-flex justify-content-between align-items-center mt-2 pt-1 border-top border-light">
                <!-- Stepper (-) (qty) (+) -->
                <div class="d-flex align-items-center">
                    <button class="cukcuk-qty-btn" onclick="updateCartQuantity(${item.item_id}, -1)">-</button>
                    <span class="cukcuk-qty-val">${item.quantity}</span>
                    <button class="cukcuk-qty-btn" onclick="updateCartQuantity(${item.item_id}, 1)">+</button>
                </div>

                <!-- Special note input -->
                <div class="ms-2 flex-grow-1">
                    <input type="text" class="form-control form-control-sm cart-note-input w-100" 
                           placeholder="Ghi chú (ít cay, không hành...)" value="${item.special_note || ''}"
                           onchange="setSpecialNote(${item.item_id}, this.value)">
                </div>
            </div>
        </div>
    `).join('');
}

function renderTablesSelect() {
    const select = document.getElementById('tableSelect');
    const displayEl = document.getElementById('cartTableDisplay');
    if (!select) return;

    select.innerHTML = state.tables.map(t => {
        const tid = t.table_id || t.id;
        const tnum = t.table_number || t.tableNumber || `Bàn ${tid}`;
        const tstatus = t.status || 'Available';
        return `
            <option value="${tid}" ${tid === state.selectedTableId ? 'selected' : ''}>
                ${tnum} (${tstatus})
            </option>
        `;
    }).join('');

    if (displayEl && select.options[select.selectedIndex]) {
        displayEl.innerText = `Đang chọn: ${select.options[select.selectedIndex].text}`;
    }

    select.addEventListener('change', (e) => {
        state.selectedTableId = parseInt(e.target.value);
        if (displayEl && select.options[select.selectedIndex]) {
            displayEl.innerText = `Đang chọn: ${select.options[select.selectedIndex].text}`;
        }
    });
}

function selectCategory(catId) {
    state.selectedCategoryId = catId;
    renderCategories();
    renderMenuItems();
}

async function handleOrderSubmit() {
    if (state.cart.length === 0) return;
    const btn = document.getElementById('btnSubmitOrder');
    btn.disabled = true;
    btn.innerHTML = `<span class="spinner-border spinner-border-sm me-2"></span>Đang gửi bếp...`;

    try {
        const result = await Api.createOrder(state.selectedTableId, state.cart);
        if (result.success) {
            const orderId = result.data ? (result.data.order_id || result.data.id) : '';
            showToast(`✅ Đã gửi đơn hàng #${orderId} xuống màn hình Bếp (Bàn ${state.selectedTableId})!`, 'success');
            state.cart = [];
            renderCart();
        } else {
            showToast(`Lỗi đặt món: ${result.error || result.message}`, 'danger');
        }
    } catch (err) {
        showToast(`Không thể kết nối máy chủ API: ${err.message}`, 'danger');
    } finally {
        btn.disabled = false;
        btn.innerHTML = `<i class="bi bi-send-fill me-1"></i> GỬI BẾP (ORDER)`;
    }
}

function showToast(msg, type = 'success') {
    const toast = document.createElement('div');
    const bgClass = type === 'success' ? 'bg-success' : (type === 'danger' ? 'bg-danger' : 'bg-dark');
    toast.className = `pos-toast ${bgClass} shadow-lg`;
    toast.innerHTML = `<i class="bi bi-check-circle-fill"></i> <span>${msg}</span>`;
    document.body.appendChild(toast);
    setTimeout(() => {
        toast.style.opacity = '0';
        toast.style.transition = 'opacity 0.3s ease';
        setTimeout(() => toast.remove(), 300);
    }, 3200);
}

// --- Init App ---
window.addEventListener('DOMContentLoaded', async () => {
    // 1. Tải dữ liệu từ Backend
    state.categories = await Api.getCategories();
    state.menuItems = await Api.getMenuItems();
    state.tables = await Api.getTables();

    // 2. Render toàn bộ giao diện CukCuk
    renderCategories();
    renderMenuItems();
    renderTablesSelect();
    renderCart();

    // 3. Sự kiện Gửi Bếp
    const submitBtn = document.getElementById('btnSubmitOrder');
    if (submitBtn) submitBtn.addEventListener('click', handleOrderSubmit);

    // 4. Sự kiện Xóa giỏ hàng
    const clearBtn = document.getElementById('btnClearCart');
    if (clearBtn) clearBtn.addEventListener('click', clearCart);

    // 5. Sự kiện Tìm kiếm món ăn thời gian thực
    const searchInput = document.getElementById('foodSearchInput');
    if (searchInput) {
        searchInput.addEventListener('input', (e) => {
            state.searchQuery = e.target.value;
            renderMenuItems();
        });
    }
});
