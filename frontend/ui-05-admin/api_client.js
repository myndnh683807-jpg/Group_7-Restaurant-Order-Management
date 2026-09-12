const API = "http://localhost:8080/api";

async function showEmployees() {

    const response =
        await fetch(
            `${API}/employees`
        );

    const employees =
        await response.json();

    let html = `
        <h4>Employees</h4>

        <table class="table table-bordered">

            <thead>
            <tr>
                <th>ID</th>
                <th>Username</th>
                <th>Full Name</th>
                <th>Role</th>
                <th>Phone</th>
                <th>Action</th>
            </tr>
            </thead>

            <tbody>
    `;

    employees.forEach(employee => {

        html += `
            <tr>

                <td>
                    ${employee.id}
                </td>

                <td>
                    ${employee.username}
                </td>

                <td>
                    ${employee.fullName}
                </td>

                <td>
                    ${employee.role}
                </td>

                <td>
                    ${employee.phone || ""}
                </td>

                <td>

                    <button
                        class="btn btn-danger btn-sm"
                        onclick="
                            deleteEmployee(
                                ${employee.id}
                            )
                        "
                    >
                        Delete
                    </button>

                </td>

            </tr>
        `;
    });

    html += `
            </tbody>
        </table>
    `;

    document.getElementById(
        "content"
    ).innerHTML = html;
}

async function deleteEmployee(id) {

    const response =
        await fetch(
            `${API}/employees/${id}`,
            {
                method: "DELETE"
            }
        );

    if (response.ok) {
        showEmployees();
    }
}

async function showInventory() {

    const response =
        await fetch(
            `${API}/inventory`
        );

    const inventory =
        await response.json();

    let html = `
        <h4>Inventory</h4>

        <table class="table table-bordered">

            <thead>

            <tr>
                <th>ID</th>
                <th>Item</th>
                <th>Quantity</th>
                <th>Unit</th>
                <th>Min Stock</th>
                <th>Status</th>
            </tr>

            </thead>

            <tbody>
    `;

    inventory.forEach(item => {

        const lowStock =
            item.quantity <=
            item.minStock;

        html += `
            <tr
                class="
                    ${lowStock
                        ? "low-stock"
                        : ""}
                "
            >

                <td>
                    ${item.id}
                </td>

                <td>
                    ${item.itemName}
                </td>

                <td>
                    ${item.quantity}
                </td>

                <td>
                    ${item.unit}
                </td>

                <td>
                    ${item.minStock}
                </td>

                <td>
                    ${lowStock
                        ? "Low Stock"
                        : "Normal"}
                </td>

            </tr>
        `;
    });

    html += `
            </tbody>
        </table>
    `;

    document.getElementById(
        "content"
    ).innerHTML = html;
}

showEmployees();