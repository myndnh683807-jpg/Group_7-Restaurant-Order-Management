const API_BASE_URL = "http://localhost:8080/api";

document.addEventListener("DOMContentLoaded", () => {
    loadEmployees();
    loadInventory();

    document.getElementById("employeeForm").addEventListener("submit", async (e) => {
        e.preventDefault();
        const payload = {
            username: document.getElementById("empUsername").value,
            password: document.getElementById("empPassword").value,
            fullName: document.getElementById("empFullName").value,
            role: document.getElementById("empRole").value
        };

        const res = await fetch(`${API_BASE_URL}/employees`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload)
        });
        const result = await res.json();
        alert(result.message);
        if (result.status === "success") {
            document.getElementById("employeeForm").reset();
            loadEmployees();
        }
    });

    document.getElementById("inventoryForm").addEventListener("submit", async (e) => {
        e.preventDefault();
        const payload = {
            itemName: document.getElementById("itemName").value,
            quantity: parseFloat(document.getElementById("itemQuantity").value),
            unit: document.getElementById("itemUnit").value,
            minQuantity: parseFloat(document.getElementById("itemMinQuantity").value)
        };

        const res = await fetch(`${API_BASE_URL}/inventory`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload)
        });
        const result = await res.json();
        alert(result.message);
        if (result.status === "success") {
            document.getElementById("inventoryForm").reset();
            loadInventory();
        }
    });

    document.getElementById("btnLowStock").addEventListener("click", loadLowStock);
});

async function loadEmployees() {
    const res = await fetch(`${API_BASE_URL}/employees`);
    const result = await res.json();
    const tbody = document.getElementById("employeeTableBody");
    tbody.innerHTML = "";
    if (result.status === "success") {
        result.data.forEach(emp => {
            tbody.innerHTML += `<tr><td>${emp.username}</td><td>${emp.fullName}</td><td>${emp.role}</td></tr>`;
        });
    }
}

async function loadInventory() {
    const res = await fetch(`${API_BASE_URL}/inventory`);
    const result = await res.json();
    renderInventoryTable(result.data);
}

async function loadLowStock() {
    const res = await fetch(`${API_BASE_URL}/inventory/low-stock`);
    const result = await res.json();
    renderInventoryTable(result.data);
}

function renderInventoryTable(items) {
    const tbody = document.getElementById("inventoryTableBody");
    tbody.innerHTML = "";
    if (items && items.length > 0) {
        items.forEach(item => {
            const isLow = item.quantity <= item.minQuantity;
            tbody.innerHTML += `
                <tr class="${isLow ? 'low-stock-row' : ''}">
                    <td>${item.itemName} ${isLow ? '⚠️' : ''}</td>
                    <td>${item.quantity}</td>
                    <td>${item.unit}</td>
                    <td>${item.minQuantity}</td>
                </tr>`;
        });
    } else {
        tbody.innerHTML = `<tr><td colspan="4" class="text-center">No items found</td></tr>`;
    }
}