// frontend/ui-03-kitchen/api_client.js
// Client API & State Manager cho Màn Hình Bếp KDS (UI-03 - UC10, UC11)

const API_BASE = 'http://localhost:8080/api';

let state = {
    items: [],
    autoRefresh: true,
    pollingInterval: null
};

const Api = {
    async getKitchenItems() {
        try {
            const res = await fetch(`${API_BASE}/kitchen/items`);
            const data = await res.json();
            return data.success && data.data ? data.data : [];
        } catch (err) {
            console.warn('[API] Dung Mock Kitchen Items:', err);
            return [
                { order_item_id: 101, order_id: 1, table_number: 'Bàn 02', item_id: 4, item_name: 'Bò Bít Tết Thượng Hạng', quantity: 2, special_note: 'Medium rare, nhiều tiêu đen', item_status: 'Pending', order_date: '10 phút trước' },
                { order_item_id: 102, order_id: 1, table_number: 'Bàn 02', item_id: 2, item_name: 'Khoai Tây Chiên Giòn', quantity: 1, special_note: 'Chiên giòn', item_status: 'Preparing', order_date: '12 phút trước' },
                { order_item_id: 103, order_id: 2, table_number: 'Bàn 01', item_id: 3, item_name: 'Gà Nướng Mật Ong', quantity: 1, special_note: 'Ít ngọt', item_status: 'Ready', order_date: '18 phút trước' },
                { order_item_id: 104, order_id: 3, table_number: 'Bàn 05', item_id: 1, item_name: 'Nem Cuốn Tôm Thịt', quantity: 3, special_note: 'Không cay', item_status: 'Served', order_date: '25 phút trước' }
            ];
        }
    },

    async updateStatus(orderItemId, newStatus) {
        const res = await fetch(`${API_BASE}/kitchen/items/${orderItemId}/status`, {
            method: 'PATCH',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ status: newStatus })
        });
        return await res.json();
    }
};

// --- Render Logic ---
function renderKanban() {
    const colPending = document.getElementById('colPending');
    const colPreparing = document.getElementById('colPreparing');
    const colReady = document.getElementById('colReady');
    const colServed = document.getElementById('colServed');

    const countPending = document.getElementById('countPending');
    const countPreparing = document.getElementById('countPreparing');
    const countReady = document.getElementById('countReady');
    const countServed = document.getElementById('countServed');

    const pendingList = state.items.filter(i => i.item_status === 'Pending');
    const preparingList = state.items.filter(i => i.item_status === 'Preparing');
    const readyList = state.items.filter(i => i.item_status === 'Ready');
    const servedList = state.items.filter(i => i.item_status === 'Served');

    if (countPending) countPending.innerText = pendingList.length;
    if (countPreparing) countPreparing.innerText = preparingList.length;
    if (countReady) countReady.innerText = readyList.length;
    if (countServed) countServed.innerText = servedList.length;

    colPending.innerHTML = pendingList.length ? pendingList.map(item => renderTicket(item, 'Pending')).join('') : emptyState();
    colPreparing.innerHTML = preparingList.length ? preparingList.map(item => renderTicket(item, 'Preparing')).join('') : emptyState();
    colReady.innerHTML = readyList.length ? readyList.map(item => renderTicket(item, 'Ready')).join('') : emptyState();
    colServed.innerHTML = servedList.length ? servedList.map(item => renderTicket(item, 'Served')).join('') : emptyState();
}

function renderTicket(item, status) {
    let actionBtn = '';
    let borderClass = 'border-pending';

    if (status === 'Pending') {
        borderClass = 'border-pending';
        actionBtn = `
            <button class="btn btn-action-kds btn-kds-pending w-100" onclick="changeItemStatus(${item.order_item_id}, 'Preparing')">
                <i class="bi bi-play-fill me-1"></i> Bắt Đầu Nấu
            </button>`;
    } else if (status === 'Preparing') {
        borderClass = 'border-preparing';
        actionBtn = `
            <button class="btn btn-action-kds btn-kds-preparing w-100" onclick="changeItemStatus(${item.order_item_id}, 'Ready')">
                <i class="bi bi-check2-circle me-1"></i> Nấu Xong (Sẵn Sàng)
            </button>`;
    } else if (status === 'Ready') {
        borderClass = 'border-ready';
        actionBtn = `
            <button class="btn btn-action-kds btn-kds-ready w-100" onclick="changeItemStatus(${item.order_item_id}, 'Served')">
                <i class="bi bi-send-check me-1"></i> Đã Ra Món
            </button>`;
    } else {
        borderClass = 'border-served';
        actionBtn = `<span class="text-success small fw-semibold"><i class="bi bi-check-all me-1"></i>Đã ra món cho khách</span>`;
    }

    return `
    <div class="ticket-card ${borderClass}">
        <div class="ticket-header">
            <span class="table-tag">${item.table_number || 'Bàn ?'}</span>
            <span class="ticket-time"><i class="bi bi-clock me-1"></i>${item.order_date || 'Vừa xong'}</span>
        </div>
        <div class="ticket-item-title">
            <span>${item.item_name}</span>
            <span class="ticket-qty">x${item.quantity}</span>
        </div>
        ${item.special_note ? `<div class="ticket-note"><i class="bi bi-chat-left-dots me-1"></i>${item.special_note}</div>` : ''}
        <div class="mt-2">
            ${actionBtn}
        </div>
    </div>
    `;
}

function emptyState() {
    return `<div class="text-center py-5 text-muted small"><i class="bi bi-inbox fs-4 d-block text-secondary mb-1"></i>Không có món</div>`;
}

// --- Action Handlers ---
async function changeItemStatus(orderItemId, newStatus) {
    try {
        await Api.updateStatus(orderItemId, newStatus);
        const target = state.items.find(i => i.order_item_id === orderItemId);
        if (target) {
            target.item_status = newStatus;
        }
        renderKanban();
        showToast(`Đã chuyển món #${orderItemId} sang trạng thái "${newStatus}"`, 'success');
    } catch (err) {
        showToast(`Lỗi cập nhật: ${err.message}`, 'danger');
    }
}

async function loadData() {
    state.items = await Api.getKitchenItems();
    renderKanban();
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
    await loadData();

    // Auto poll every 6s
    state.pollingInterval = setInterval(async () => {
        if (state.autoRefresh) {
            await loadData();
        }
    }, 6000);

    const btnRefresh = document.getElementById('btnRefresh');
    if (btnRefresh) {
        btnRefresh.addEventListener('click', async () => {
            btnRefresh.disabled = true;
            await loadData();
            showToast('Đã làm mới danh sách món bếp!', 'info');
            btnRefresh.disabled = false;
        });
    }
});
