// frontend/ui-02-table/api_client.js
// Client API & State Manager cho Giao diện Sơ Đồ Bàn (UI-02)

const API_BASE = 'http://localhost:8080/api';

let state = {
    tables: [],
    paymentRequests: [],
    assistanceRequests: [],
    selectedTable: null,
    filterStatus: 'all'
};

const Api = {
    async getTables() {
        try {
            const res = await fetch(`${API_BASE}/tables`);
            const data = await res.json();
            return data.success && data.data ? data.data : [];
        } catch (err) {
            console.warn('[API] Mock tables fallback:', err);
            return [
                { table_id: 1, table_number: 'Bàn 01', capacity: 4, status: 'Available' },
                { table_id: 2, table_number: 'Bàn 02', capacity: 4, status: 'Occupied' },
                { table_id: 3, table_number: 'Bàn 03', capacity: 6, status: 'Reserved' },
                { table_id: 4, table_number: 'Bàn 04', capacity: 2, status: 'Available' },
                { table_id: 5, table_number: 'Bàn 05', capacity: 8, status: 'Occupied' },
                { table_id: 6, table_number: 'Bàn 06', capacity: 4, status: 'Available' }
            ];
        }
    },

    async updateStatus(tableId, status) {
        const res = await fetch(`${API_BASE}/tables/${tableId}/status`, {
            method: 'PATCH',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ status })
        });
        return await res.json();
    },

    async addTable(tableNumber, capacity) {
        const res = await fetch(`${API_BASE}/tables`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ table_number: tableNumber, capacity: parseInt(capacity) })
        });
        return await res.json();
    },

    async getServiceRequests() {
        try {
            const [pRes, aRes] = await Promise.all([
                fetch(`${API_BASE}/service/payment/requests`),
                fetch(`${API_BASE}/service/assistance/requests`)
            ]);
            const pData = await pRes.json();
            const aData = await aRes.json();
            return {
                payments: pData.success ? pData.data : [],
                assistances: aData.success ? aData.data : []
            };
        } catch (err) {
            return { payments: [2], assistances: [5] };
        }
    },

    async requestPayment(tableId) {
        const res = await fetch(`${API_BASE}/service/payment/request`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ table_id: tableId })
        });
        return await res.json();
    },

    async requestAssistance(tableId) {
        const res = await fetch(`${API_BASE}/service/assistance/request`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ table_id: tableId })
        });
        return await res.json();
    },

    async resolveAssistance(tableId) {
        const res = await fetch(`${API_BASE}/service/assistance/resolve`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ table_id: tableId })
        });
        return await res.json();
    }
};

// --- Render Logic ---
function renderStats() {
    const total = state.tables.length;
    const avail = state.tables.filter(t => t.status === 'Available').length;
    const occ = state.tables.filter(t => t.status === 'Occupied').length;
    const resv = state.tables.filter(t => t.status === 'Reserved').length;

    document.getElementById('statTotal').innerText = total;
    document.getElementById('statAvailable').innerText = avail;
    document.getElementById('statOccupied').innerText = occ;
    document.getElementById('statReserved').innerText = resv;
}

function renderTableGrid() {
    const container = document.getElementById('tableGrid');
    if (!container) return;

    const filtered = state.filterStatus === 'all'
        ? state.tables
        : state.tables.filter(t => t.status.toLowerCase() === state.filterStatus.toLowerCase());

    if (filtered.length === 0) {
        container.innerHTML = `<div class="col-12 text-center py-5 text-muted"><i class="bi bi-inbox fs-1 d-block mb-2"></i>Không tìm thấy bàn phù hợp</div>`;
        return;
    }

    container.innerHTML = filtered.map(table => {
        const id = table.table_id || table.id;
        const number = table.table_number || table.tableNumber;
        const status = table.status || 'Available';
        const capacity = table.capacity || 4;

        let statusClass = 'status-available';
        let statusBadge = '<span class="badge bg-light text-secondary border px-2 py-1">Trống</span>';

        if (status === 'Occupied') {
            statusClass = 'status-occupied';
            statusBadge = '<span class="badge bg-success text-white px-2 py-1">Đang Dùng</span>';
        } else if (status === 'Reserved') {
            statusClass = 'status-reserved';
            statusBadge = '<span class="badge bg-warning text-dark px-2 py-1">Đặt Trước</span>';
        }

        const isCallingAssistance = state.assistanceRequests.includes(id);
        const isCallingPayment = state.paymentRequests.includes(id);

        return `
        <div class="col-12 col-sm-6 col-md-4 col-xl-3">
            <div class="table-box ${statusClass}" onclick="openTableModal(${id})">
                ${isCallingPayment ? '<span class="position-absolute top-0 start-50 translate-middle badge rounded-pill bg-warning text-dark px-2 py-1 border"><i class="bi bi-cash me-1"></i> Báo tính tiền</span>' : ''}
                ${isCallingAssistance ? '<span class="position-absolute top-0 start-50 translate-middle badge rounded-pill bg-danger text-white px-2 py-1 border"><i class="bi bi-bell me-1"></i> Cần hỗ trợ!</span>' : ''}
                <div class="table-avatar">
                    <i class="bi bi-grid-fill"></i>
                </div>
                <div class="table-name">${number}</div>
                <div class="table-cap"><i class="bi bi-people me-1"></i>Sức chứa: ${capacity} khách</div>
                <div class="mt-2">${statusBadge}</div>
            </div>
        </div>
        `;
    }).join('');
}

function renderServiceQueue() {
    const container = document.getElementById('serviceQueue');
    if (!container) return;

    if (state.paymentRequests.length === 0 && state.assistanceRequests.length === 0) {
        container.innerHTML = `<div class="text-center py-4 text-muted small"><i class="bi bi-check2-circle fs-4 d-block text-success mb-1"></i>Không có yêu cầu nào đang chờ.</div>`;
        return;
    }

    let html = '';
    state.assistanceRequests.forEach(tableId => {
        html += `
        <div class="queue-card queue-assistance">
            <div>
                <span class="badge bg-danger me-1">HỖ TRỢ</span>
                <span class="fw-bold text-dark">Bàn #${tableId}</span>
            </div>
            <button class="btn btn-sm btn-outline-danger fw-semibold" onclick="handleResolveAssistance(${tableId})">
                <i class="bi bi-check-lg"></i> Xử Lý
            </button>
        </div>`;
    });

    state.paymentRequests.forEach(tableId => {
        html += `
        <div class="queue-card queue-payment">
            <div>
                <span class="badge bg-warning text-dark me-1">TÍNH TIỀN</span>
                <span class="fw-bold text-dark">Bàn #${tableId}</span>
            </div>
            <a href="/ui-04-pos/index.html" class="btn btn-sm btn-warning fw-semibold">
                <i class="bi bi-cash-stack"></i> Mở POS
            </a>
        </div>`;
    });

    container.innerHTML = html;
}

// --- User Actions ---
function openTableModal(tableId) {
    const table = state.tables.find(t => (t.table_id || t.id) === tableId);
    if (!table) return;
    state.selectedTable = table;

    document.getElementById('modalTableNumber').innerText = table.table_number || table.tableNumber;
    document.getElementById('modalTableCapacity').innerText = table.capacity || 4;
    document.getElementById('modalTableStatusSelect').value = table.status || 'Available';

    const modal = new bootstrap.Modal(document.getElementById('tableDetailModal'));
    modal.show();
}

async function handleStatusChange() {
    if (!state.selectedTable) return;
    const newStatus = document.getElementById('modalTableStatusSelect').value;
    const id = state.selectedTable.table_id || state.selectedTable.id;

    try {
        await Api.updateStatus(id, newStatus);
        state.selectedTable.status = newStatus;
        renderStats();
        renderTableGrid();
        bootstrap.Modal.getInstance(document.getElementById('tableDetailModal')).hide();
        showToast(`Đã đổi trạng thái ${state.selectedTable.table_number || state.selectedTable.tableNumber} sang ${newStatus}!`, 'success');
    } catch (err) {
        showToast(`Lỗi cập nhật: ${err.message}`, 'danger');
    }
}

async function handleCallService(type) {
    if (!state.selectedTable) return;
    const id = state.selectedTable.table_id || state.selectedTable.id;

    if (type === 'payment') {
        await Api.requestPayment(id);
        if (!state.paymentRequests.includes(id)) state.paymentRequests.push(id);
        showToast(`Đã gửi yêu cầu thanh toán cho Bàn #${id}`, 'warning');
    } else {
        await Api.requestAssistance(id);
        if (!state.assistanceRequests.includes(id)) state.assistanceRequests.push(id);
        showToast(`Đã gửi yêu cầu hỗ trợ cho Bàn #${id}`, 'danger');
    }
    renderTableGrid();
    renderServiceQueue();
}

async function handleResolveAssistance(tableId) {
    await Api.resolveAssistance(tableId);
    state.assistanceRequests = state.assistanceRequests.filter(id => id !== tableId);
    renderTableGrid();
    renderServiceQueue();
    showToast(`Đã hoàn tất hỗ trợ Bàn #${tableId}`, 'success');
}

function setFilter(status) {
    state.filterStatus = status;
    document.querySelectorAll('.btn-filter').forEach(b => b.classList.remove('active'));
    event.target.classList.add('active');
    renderTableGrid();
}

function showToast(msg, type = 'success') {
    const toast = document.createElement('div');
    toast.className = `alert alert-${type} position-fixed bottom-0 end-0 m-3 shadow-lg fade show z-3`;
    toast.style.borderRadius = '12px';
    toast.innerHTML = `<i class="bi bi-info-circle me-2"></i> ${msg}`;
    document.body.appendChild(toast);
    setTimeout(() => toast.remove(), 3500);
}

// --- Init App ---
window.addEventListener('DOMContentLoaded', async () => {
    state.tables = await Api.getTables();
    const srv = await Api.getServiceRequests();
    state.paymentRequests = srv.payments;
    state.assistanceRequests = srv.assistances;

    renderStats();
    renderTableGrid();
    renderServiceQueue();

    const saveStatusBtn = document.getElementById('btnSaveTableStatus');
    if (saveStatusBtn) saveStatusBtn.addEventListener('click', handleStatusChange);
});
