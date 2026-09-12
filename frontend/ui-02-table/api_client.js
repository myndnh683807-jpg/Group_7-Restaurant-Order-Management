const API = "http://localhost:8080/api";

async function loadTables() {
    try {
        const response =
            await fetch(`${API}/tables`);

        const tables =
            await response.json();

        const container =
            document.getElementById("tableList");

        container.innerHTML = "";

        tables.forEach(table => {

            container.innerHTML += `
                <div class="col-md-3">
                    <div class="table-card ${table.status}">

                        <h5>
                            ${table.tableNumber}
                        </h5>

                        <p>
                            ID: ${table.id}
                        </p>

                        <p>
                            Capacity: ${table.capacity}
                        </p>

                        <p>
                            Status: ${table.status}
                        </p>

                        <select
                            id="status-${table.id}"
                            class="form-select mb-2"
                        >
                            <option value="Available">
                                Available
                            </option>

                            <option value="Occupied">
                                Occupied
                            </option>

                            <option value="Reserved">
                                Reserved
                            </option>
                        </select>

                        <button
                            class="btn btn-primary btn-sm"
                            onclick="updateStatus(${table.id})"
                        >
                            Update
                        </button>

                        <button
                            class="btn btn-danger btn-sm"
                            onclick="deleteTable(${table.id})"
                        >
                            Delete
                        </button>

                    </div>
                </div>
            `;
        });

    } catch (error) {
        showMessage(
            "Cannot load tables.",
            "danger"
        );
    }
}

async function addTable() {

    const tableNumber =
        document.getElementById(
            "tableNumber"
        ).value;

    const capacity =
        Number(
            document.getElementById(
                "capacity"
            ).value
        );

    const response =
        await fetch(
            `${API}/tables`,
            {
                method: "POST",

                headers: {
                    "Content-Type":
                        "application/json"
                },

                body: JSON.stringify({
                    tableNumber,
                    capacity
                })
            }
        );

    if (response.ok) {
        showMessage(
            "Table added.",
            "success"
        );

        loadTables();
    }
}

async function updateStatus(id) {

    const status =
        document.getElementById(
            `status-${id}`
        ).value;

    const response =
        await fetch(
            `${API}/tables/${id}/status`,
            {
                method: "PUT",

                headers: {
                    "Content-Type":
                        "application/json"
                },

                body: JSON.stringify({
                    status
                })
            }
        );

    if (response.ok) {
        loadTables();
    }
}

async function deleteTable(id) {

    const response =
        await fetch(
            `${API}/tables/${id}`,
            {
                method: "DELETE"
            }
        );

    if (response.ok) {
        loadTables();
    }
}

async function requestAssistance() {

    const tableId =
        Number(
            document.getElementById(
                "assistTableId"
            ).value
        );

    const response =
        await fetch(
            `${API}/service/assist`,
            {
                method: "POST",

                headers: {
                    "Content-Type":
                        "application/json"
                },

                body: JSON.stringify({
                    tableId
                })
            }
        );

    showMessage(
        response.ok
            ? "Assistance requested."
            : "Request failed.",
        response.ok
            ? "success"
            : "danger"
    );
}

async function requestPayment() {

    const orderId =
        Number(
            document.getElementById(
                "paymentOrderId"
            ).value
        );

    const response =
        await fetch(
            `${API}/service/payment`,
            {
                method: "POST",

                headers: {
                    "Content-Type":
                        "application/json"
                },

                body: JSON.stringify({
                    orderId
                })
            }
        );

    showMessage(
        response.ok
            ? "Payment requested."
            : "Request failed.",
        response.ok
            ? "success"
            : "danger"
    );
}

async function serveFood() {

    const itemId =
        Number(
            document.getElementById(
                "serveItemId"
            ).value
        );

    const response =
        await fetch(
            `${API}/service/serve/${itemId}`,
            {
                method: "PUT"
            }
        );

    showMessage(
        response.ok
            ? "Food served."
            : "Cannot serve food.",
        response.ok
            ? "success"
            : "danger"
    );
}

function showMessage(message, type) {

    document.getElementById(
        "message"
    ).innerHTML = `
        <div class="alert alert-${type}">
            ${message}
        </div>
    `;
}

loadTables();