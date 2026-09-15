// frontend/ui-05-admin/api_client.js
// Client API & State Manager cho Giao diện Quản Trị & Dashboard (UI-05)

const API_BASE = 'http://localhost:8080/api';

let state = {
    summary: null,
    employees: [],
    inventory: []
};

const Api = {
    async getSummary() {
        try {
            const res = await fetch(`${API_BASE}/reports/summary`);
            const data = await res.json();
            return data.success ? data.data : null;
        } catch (err) {
            return {
                tables: { total: 10, available: 6, occupied: 3, reserved: 1 },
                orders: { pending: 4, completed: 18 },
                inventory: { low_stock_count: 2 },
                revenue: { total: 1845.50 }
            };
        }
    },

    // --- Employees (UC13) ---
    async getEmployees() {
        try {
            const res = await fetch(`${API_BASE}/employees`);
            const data = await res.json();
            return data.success ? data.data : [];
        } catch (err) {
            return [
                { employee_id: 1, username: 'admin', full_name: 'John Manager', role: 'Admin', phone: '0900000001' },
                { employee_id: 2, username: 'staff01', full_name: 'David Staff', role: 'Staff', phone: '0900000002' },
                { employee_id: 3, username: 'chef01', full_name: 'Mike Chef', role: 'Chef', phone: '0900000003' },
                { employee_id: 4, username: 'cashier01', full_name: 'Anna Cashier', role: 'Cashier', phone: '0900000004' }
            ];
        }
    },

    async addEmployee(empData) {
        const res = await fetch(`${API_BASE}/employees`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(empData)
        });
        return await res.json();
    },

    async deleteEmployee(id) {
        const res = await fetch(`${API_BASE}/employees/${id}`, {
            method: 'DELETE'
        });
        return await res.json();
    },

    // --- Inventory (UC14) ---
    async getInventory() {
        try {
            const res = await fetch(`${API_BASE}/inventory`);
            const data = await res.json();
            return data.success ? data.data : [];
        } catch (err) {
            return [
                { inventory_id: 1, item_name: 'Thịt Bò Úc', quantity: 20, unit: 'kg', min_stock: 5, last_updated: '2026-09-13' },
                { inventory_id: 2, item_name: 'Thịt Gà Ta', quantity: 30, unit: 'kg', min_stock: 5, last_updated: '2026-09-13' },
                { inventory_id: 3, item_name: 'Hạt Cà Phê Robusta', quantity: 3, unit: 'kg', min_stock: 5, last_updated: '2026-09-13' },
                { inventory_id: 4, item_name: 'Cam Sành Tiền Giang', quantity: 8, unit: 'kg', min_stock: 10, last_updated: '2026-09-13' }
            ];
        }
    },

    async addInventory(itemData) {
        const res = await fetch(`${API_BASE}/inventory`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(itemData)
        });
        return await res.json();
    },

    async adjustStock(id, delta) {
        const res = await fetch(`${API_BASE}/inventory/${id}/stock`, {
            method: 'PATCH',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ delta: delta })
        });
        return await res.json();
    },

    async deleteInventory(id) {
        const res = await fetch(`${API_BASE}/inventory/${id}`, {
            method: 'DELETE'
        });
        return await res.json();
    }
};

// --- Render Logic ---
function renderSummary() {
    if (!state.summary) return;
    const s = state.summary;

    document.getElementById('kpiRevenue').innerText = `$${Number(s.revenue ? s.revenue.total : 0).toFixed(2)}`;
    document.getElementById('kpiTables').innerText = `${s.tables ? s.tables.occupied : 0} / ${s.tables ? s.tables.total : 0}`;
    document.getElementById('kpiOrders').innerText = `${s.orders ? s.orders.pending : 0} đang xử lý`;
    document.getElementById('kpiLowStock').innerText = `${s.inventory ? s.inventory.low_stock_count : 0} mặt hàng`;
}

function renderEmployees() {
    const tbody = document.getElementById('employeeTableBody');
    if (!tbody) return;

    if (state.employees.length === 0) {
        tbody.innerHTML = `<tr><td colspan="6" class="text-center py-4 text-muted">Chưa có nhân viên nào</td></tr>`;
        return;
    }

    const roleBadges = {
        'Admin': 'bg-primary text-white',
        'Staff': 'bg-primary bg-opacity-10 text-primary border border-primary border-opacity-25',
        'Chef': 'bg-warning bg-opacity-10 text-warning border border-warning border-opacity-25',
        'Cashier': 'bg-success bg-opacity-10 text-success border border-success border-opacity-25'
    };

    tbody.innerHTML = state.employees.map(emp => {
        const id = emp.employee_id || emp.id;
        const badgeClass = roleBadges[emp.role] || 'bg-light text-secondary border';

        return `
        <tr>
            <td class="fw-bold text-dark">#${id}</td>
            <td>
                <div class="fw-semibold text-dark">${emp.full_name || emp.fullName}</div>
                <div class="small text-muted">@${emp.username}</div>
            </td>
            <td><span class="badge ${badgeClass} px-2 py-1">${emp.role}</span></td>
            <td class="text-muted small"><i class="bi bi-telephone me-1"></i>${emp.phone || 'Chưa có'}</td>
            <td class="text-end">
                <button class="btn btn-sm btn-outline-danger" onclick="handleDeleteEmployee(${id})">
                    <i class="bi bi-trash"></i> Xóa
                </button>
            </td>
        </tr>
        `;
    }).join('');
}

function renderInventory() {
    const tbody = document.getElementById('inventoryTableBody');
    if (!tbody) return;

    if (state.inventory.length === 0) {
        tbody.innerHTML = `<tr><td colspan="6" class="text-center py-4 text-muted">Kho hiện đang rỗng</td></tr>`;
        return;
    }

    tbody.innerHTML = state.inventory.map(item => {
        const id = item.inventory_id || item.id;
        const isLow = item.quantity <= item.min_stock;
        const ratio = Math.min(100, Math.round((item.quantity / Math.max(1, item.min_stock * 2)) * 100));

        return `
        <tr>
            <td class="fw-bold text-dark">#${id}</td>
            <td class="fw-semibold text-dark">${item.item_name || item.itemName}</td>
            <td>
                <div class="d-flex align-items-center justify-content-between mb-1">
                    <span class="fw-bold ${isLow ? 'text-danger' : 'text-dark'}">${item.quantity} ${item.unit}</span>
                    ${isLow ? '<span class="badge bg-danger bg-opacity-10 text-danger border border-danger border-opacity-25 small">Cảnh báo sắp hết</span>' : '<span class="badge bg-light text-muted border small">Đủ định mức</span>'}
                </div>
                <div class="stock-bar" style="width: 140px;">
                    <div class="stock-bar-fill ${isLow ? 'bg-danger' : 'bg-success'}" style="width: ${ratio}%;"></div>
                </div>
            </td>
            <td class="text-muted small">Tối thiểu: <strong>${item.min_stock || item.minStock}</strong> ${item.unit}</td>
            <td class="text-end">
                <div class="btn-group btn-group-sm">
                    <button class="btn btn-outline-secondary" onclick="handleAdjustStock(${id}, -1)">-1</button>
                    <button class="btn btn-outline-secondary" onclick="handleAdjustStock(${id}, 5)">+5</button>
                    <button class="btn btn-outline-secondary" onclick="handleAdjustStock(${id}, 10)">+10</button>
                    <button class="btn btn-outline-danger" onclick="handleDeleteInventory(${id})"><i class="bi bi-trash"></i></button>
                </div>
            </td>
        </tr>
        `;
    }).join('');
}

// --- Action Handlers ---
async function handleAddEmployee(e) {
    e.preventDefault();
    const username = document.getElementById('empUsername').value.trim();
    const password = document.getElementById('empPassword').value.trim();
    const fullName = document.getElementById('empFullName').value.trim();
    const role = document.getElementById('empRole').value;
    const phone = document.getElementById('empPhone').value.trim();

    try {
        const res = await Api.addEmployee({ username, password, full_name: fullName, role, phone });
        if (res.success) {
            showToast(`Thêm nhân viên "${fullName}" thành công!`, 'success');
            bootstrap.Modal.getInstance(document.getElementById('addEmpModal')).hide();
            document.getElementById('formAddEmp').reset();
            state.employees = await Api.getEmployees();
            renderEmployees();
        } else {
            showToast(`Lỗi: ${res.error || res.message}`, 'danger');
        }
    } catch (err) {
        showToast(`Lỗi kết nối: ${err.message}`, 'danger');
    }
}

async function handleDeleteEmployee(id) {
    if (!confirm(`Bạn có chắc muốn xóa nhân viên ID #${id}?`)) return;
    try {
        const res = await Api.deleteEmployee(id);
        if (res.success) {
            showToast('Xóa nhân viên thành công!', 'success');
            state.employees = state.employees.filter(e => (e.employee_id || e.id) !== id);
            renderEmployees();
        } else {
            showToast(res.error || res.message, 'danger');
        }
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

async function handleAddInventory(e) {
    e.preventDefault();
    const itemName = document.getElementById('invName').value.trim();
    const quantity = parseInt(document.getElementById('invQty').value) || 0;
    const unit = document.getElementById('invUnit').value.trim();
    const minStock = parseInt(document.getElementById('invMin').value) || 0;

    try {
        const res = await Api.addInventory({ item_name: itemName, quantity, unit, min_stock: minStock });
        if (res.success) {
            showToast(`Thêm mặt hàng "${itemName}" thành công!`, 'success');
            bootstrap.Modal.getInstance(document.getElementById('addInvModal')).hide();
            document.getElementById('formAddInv').reset();
            state.inventory = await Api.getInventory();
            renderInventory();
        } else {
            showToast(`Lỗi: ${res.error || res.message}`, 'danger');
        }
    } catch (err) {
        showToast(`Lỗi kết nối: ${err.message}`, 'danger');
    }
}

async function handleAdjustStock(id, delta) {
    try {
        const res = await Api.adjustStock(id, delta);
        if (res.success) {
            const item = state.inventory.find(i => (i.inventory_id || i.id) === id);
            if (item) item.quantity = res.data ? res.data.new_quantity : (item.quantity + delta);
            renderInventory();
            showToast(`Đã cập nhật tồn kho (${delta > 0 ? '+' : ''}${delta})`, 'info');
        }
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

async function handleDeleteInventory(id) {
    if (!confirm(`Bạn có chắc muốn xóa mặt hàng kho #${id}?`)) return;
    try {
        const res = await Api.deleteInventory(id);
        if (res.success) {
            showToast('Đã xóa mặt hàng kho!', 'success');
            state.inventory = state.inventory.filter(i => (i.inventory_id || i.id) !== id);
            renderInventory();
        }
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

function showToast(msg, type = 'success') {
    const toast = document.createElement('div');
    toast.className = `alert alert-${type} position-fixed bottom-0 end-0 m-3 border fade show z-3 py-2 px-3 small`;
    toast.style.borderRadius = '6px';
    toast.innerHTML = `<i class="bi bi-info-circle me-1"></i> ${msg}`;
    document.body.appendChild(toast);
    setTimeout(() => toast.remove(), 3500);
}

// --- Init App ---
window.addEventListener('DOMContentLoaded', async () => {
    state.summary = await Api.getSummary();
    state.employees = await Api.getEmployees();
    state.inventory = await Api.getInventory();

    renderSummary();
    renderEmployees();
    renderInventory();

    const formEmp = document.getElementById('formAddEmp');
    if (formEmp) formEmp.addEventListener('submit', handleAddEmployee);

    const formInv = document.getElementById('formAddInv');
    if (formInv) formInv.addEventListener('submit', handleAddInventory);
});
