// frontend/ui-04-pos/api_client.js
// Client API & State Manager cho Giao diện Thu Ngân & POS (UI-04)

const API_BASE = 'http://localhost:8080/api';

let state = {
    activeOrders: [],
    selectedOrder: null,
    billBreakdown: null,
    paymentMethod: 'Cash',
    isVip: false,
    amountPaid: 0,
    changeDue: 0
};

const Api = {
    async getActiveOrders() {
        try {
            const res = await fetch(`${API_BASE}/orders`);
            const data = await res.json();
            if (data.success && data.data) {
                // Lọc đơn chưa thanh toán
                return data.data.filter(o => o.order_status !== 'Completed' && o.order_status !== 'Cancelled');
            }
            return [];
        } catch (err) {
            console.warn('[API] Mock active orders fallback:', err);
            return [
                {
                    order_id: 1, table_id: 2, order_date: 'Hôm nay, 19:30', order_status: 'Pending', total_amount: 32.50,
                    items: [
                        { item_id: 4, order_item_id: 1, quantity: 1, unit_price: 18.00, item_status: 'Served', special_note: 'Bò Bít Tết' },
                        { item_id: 3, order_item_id: 2, quantity: 1, unit_price: 12.50, item_status: 'Served', special_note: 'Gà Nướng Mật Ong' },
                        { item_id: 6, order_item_id: 3, quantity: 1, unit_price: 2.00, item_status: 'Served', special_note: 'Cà phê' }
                    ]
                },
                {
                    order_id: 2, table_id: 1, order_date: 'Hôm nay, 19:45', order_status: 'In Progress', total_amount: 14.50,
                    items: [
                        { item_id: 1, order_item_id: 4, quantity: 2, unit_price: 5.50, item_status: 'Preparing', special_note: 'Nem cuốn' },
                        { item_id: 5, order_item_id: 5, quantity: 1, unit_price: 3.50, item_status: 'Ready', special_note: 'Nước cam' }
                    ]
                }
            ];
        }
    },

    async checkBill(orderId, isVip = false, discountPercent = 0) {
        try {
            const res = await fetch(`${API_BASE}/payment/check-bill`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ order_id: orderId, is_vip: isVip, discount_percent: discountPercent })
            });
            const data = await res.json();
            return data.success ? data.data : null;
        } catch (err) {
            return null;
        }
    },

    async processPayment(orderId, paymentMethod, discountAmount, finalAmount, amountPaid) {
        const res = await fetch(`${API_BASE}/payment/checkout`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                order_id: orderId,
                cashier_id: 1,
                payment_method: paymentMethod,
                discount_amount: discountAmount,
                final_amount: finalAmount,
                amount_paid: amountPaid
            })
        });
        return await res.json();
    },

    async getInvoices() {
        try {
            const res = await fetch(`${API_BASE}/payment/invoices`);
            const data = await res.json();
            return data.success ? data.data : [];
        } catch (err) {
            return [];
        }
    },

    async getRevenueReport() {
        try {
            const res = await fetch(`${API_BASE}/reports/revenue`);
            const data = await res.json();
            return data.success ? data.data : null;
        } catch (err) {
            return null;
        }
    }
};

// --- Render Logic ---
function renderOrderSelector() {
    const container = document.getElementById('orderSelector');
    if (!container) return;

    if (state.activeOrders.length === 0) {
        container.innerHTML = `<div class="p-4 text-center text-muted small"><i class="bi bi-inbox fs-4 d-block mb-1"></i>Không có đơn bàn nào đang phục vụ</div>`;
        return;
    }

    container.innerHTML = state.activeOrders.map(order => {
        const isSelected = state.selectedOrder && state.selectedOrder.order_id === order.order_id;
        return `
        <div class="pos-order-card ${isSelected ? 'active' : ''}" onclick="selectOrder(${order.order_id})">
            <div class="d-flex justify-content-between align-items-center mb-1">
                <span class="fw-bold text-dark"><i class="bi bi-table text-primary me-1"></i>Bàn #${order.table_id} (Đơn #${order.order_id})</span>
                <span class="badge bg-warning text-dark border">${order.order_status}</span>
            </div>
            <div class="d-flex justify-content-between text-muted small">
                <span><i class="bi bi-clock me-1"></i>${order.order_date}</span>
                <span class="text-dark fw-bold">$${Number(order.total_amount).toFixed(2)}</span>
            </div>
        </div>
        `;
    }).join('');
}

async function selectOrder(orderId) {
    const order = state.activeOrders.find(o => o.order_id === orderId);
    if (!order) return;
    state.selectedOrder = order;

    const label = document.getElementById('posSelectedTableLabel');
    if (label) {
        label.innerText = `Bàn #${order.table_id} (Đơn #${order.order_id})`;
    }

    renderOrderSelector();
    await updateCalculation();
}

async function updateCalculation() {
    if (!state.selectedOrder) return;

    let discountPct = state.isVip ? 10.0 : 0.0;
    const customDiscInput = document.getElementById('inputDiscount');
    if (customDiscInput && customDiscInput.value) {
        discountPct = parseFloat(customDiscInput.value) || 0.0;
    }

    // Call API check-bill or calculate locally
    let bill = await Api.checkBill(state.selectedOrder.order_id, state.isVip, discountPct);
    if (!bill) {
        // Fallback local calculate
        let subtotal = 0.0;
        if (state.selectedOrder.items) {
            state.selectedOrder.items.forEach(i => subtotal += (i.unit_price * i.quantity));
        }
        if (subtotal <= 0) subtotal = state.selectedOrder.total_amount;
        const discountAmt = subtotal * (discountPct / 100.0);
        const vat = (subtotal - discountAmt) * 0.08;
        const final = (subtotal - discountAmt) + vat;
        bill = {
            order_id: state.selectedOrder.order_id,
            subtotal: subtotal,
            discount_percent: discountPct,
            discount_amount: discountAmt,
            vat_amount: vat,
            final_amount: final,
            items: state.selectedOrder.items || []
        };
    }

    state.billBreakdown = bill;
    renderBillBreakdown();
}

function renderBillBreakdown() {
    const bill = state.billBreakdown;
    if (!bill) return;

    const itemsContainer = document.getElementById('billItemsList');
    if (itemsContainer) {
        itemsContainer.innerHTML = (bill.items || []).map(i => `
            <tr>
                <td class="text-dark fw-semibold">${i.special_note || 'Món ăn #' + i.item_id}</td>
                <td class="text-center text-muted">x${i.quantity}</td>
                <td class="text-end text-muted">$${Number(i.unit_price).toFixed(2)}</td>
                <td class="text-end text-dark fw-bold">$${(i.unit_price * i.quantity).toFixed(2)}</td>
            </tr>
        `).join('');
    }

    document.getElementById('valSubtotal').innerText = `$${bill.subtotal.toFixed(2)}`;
    document.getElementById('valDiscount').innerText = `-$${bill.discount_amount.toFixed(2)}`;
    document.getElementById('valVat').innerText = `$${bill.vat_amount.toFixed(2)}`;
    document.getElementById('valFinalTotal').innerText = `$${bill.final_amount.toFixed(2)}`;

    // Quick cash suggestion
    const cashInput = document.getElementById('inputCashReceived');
    if (cashInput && (!cashInput.value || parseFloat(cashInput.value) < bill.final_amount)) {
        cashInput.value = Math.ceil(bill.final_amount);
    }
    calculateChange();

    document.getElementById('btnSubmitPayment').disabled = false;
}

function calculateChange() {
    if (!state.billBreakdown) return;
    const cashInput = document.getElementById('inputCashReceived');
    const changeEl = document.getElementById('valChangeDue');

    const paid = parseFloat(cashInput.value) || 0;
    const final = state.billBreakdown.final_amount;
    const change = Math.max(0, paid - final);

    state.amountPaid = paid;
    state.changeDue = change;

    if (changeEl) {
        changeEl.innerText = `$${change.toFixed(2)}`;
    }
}

function setPaymentMethod(method) {
    state.paymentMethod = method;
    document.querySelectorAll('.btn-method').forEach(b => b.classList.remove('active'));
    event.currentTarget.classList.add('active');
}

async function handleCheckout() {
    if (!state.selectedOrder || !state.billBreakdown) return;

    const btn = document.getElementById('btnSubmitPayment');
    btn.disabled = true;
    btn.innerHTML = `<span class="spinner-border spinner-border-sm me-2"></span>Đang xử lý thanh toán...`;

    try {
        const res = await Api.processPayment(
            state.selectedOrder.order_id,
            state.paymentMethod,
            state.billBreakdown.discount_amount,
            state.billBreakdown.final_amount,
            state.amountPaid
        );

        if (res.success) {
            showToast(`Thanh toán thành công Bàn #${state.selectedOrder.table_id}! Mã hóa đơn: #${res.data.invoice_id}`, 'success');
            showReceiptModal(res.data);
            
            // Refresh order list
            state.activeOrders = await Api.getActiveOrders();
            state.selectedOrder = null;
            state.billBreakdown = null;
            renderOrderSelector();
            document.getElementById('billItemsList').innerHTML = '';
            document.getElementById('valFinalTotal').innerText = '$0.00';
            btn.disabled = true;
        } else {
            showToast(`Lỗi thanh toán: ${res.error || res.message}`, 'danger');
        }
    } catch (err) {
        showToast(`Lỗi: ${err.message}`, 'danger');
    } finally {
        btn.innerHTML = `<i class="bi bi-printer me-2"></i> In Hóa Đơn & Thanh Toán`;
    }
}

function showReceiptModal(invoice) {
    document.getElementById('receiptInvoiceId').innerText = `#INV-${invoice.invoice_id || invoice.id || '999'}`;
    document.getElementById('receiptDate').innerText = new Date().toLocaleString('vi-VN');
    document.getElementById('receiptTable').innerText = `Bàn #${invoice.table_id || '01'}`;
    document.getElementById('receiptTotal').innerText = `$${Number(invoice.final_amount).toFixed(2)}`;
    document.getElementById('receiptMethod').innerText = invoice.payment_method;
    document.getElementById('receiptPaid').innerText = `$${Number(invoice.amount_paid || invoice.final_amount).toFixed(2)}`;
    document.getElementById('receiptChange').innerText = `$${Number(invoice.change_due || 0).toFixed(2)}`;

    const modal = new bootstrap.Modal(document.getElementById('receiptModal'));
    modal.show();
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
    state.activeOrders = await Api.getActiveOrders();
    renderOrderSelector();

    const vipCheck = document.getElementById('checkVip');
    if (vipCheck) {
        vipCheck.addEventListener('change', async (e) => {
            state.isVip = e.target.checked;
            await updateCalculation();
        });
    }

    const discInput = document.getElementById('inputDiscount');
    if (discInput) {
        discInput.addEventListener('input', async () => {
            await updateCalculation();
        });
    }

    const cashInput = document.getElementById('inputCashReceived');
    if (cashInput) {
        cashInput.addEventListener('input', calculateChange);
    }

    const btnSubmit = document.getElementById('btnSubmitPayment');
    if (btnSubmit) {
        btnSubmit.addEventListener('click', handleCheckout);
    }
});
