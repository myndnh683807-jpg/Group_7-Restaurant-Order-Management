const API = "http://localhost:8080/api";


async function apiRequest(url, options = {}) {

    const response =
        await fetch(url, options);

    let data = {};

    try {
        data = await response.json();
    } catch (_) {
    }

    if (!response.ok) {
        throw new Error(
            data.message || "Request failed."
        );
    }

    return data;
}


async function loadDashboard() {

    await Promise.all([
        loadTables(),
        loadReadyFood(),
        loadAssistanceCalls()
    ]);
}


async function loadTables() {

    try {

        const tables =
            await apiRequest(
                `${API}/tables`
            );


        document.getElementById(
            "availableCount"
        ).textContent =
            tables.filter(
                t => t.status === "Available"
            ).length;


        document.getElementById(
            "occupiedCount"
        ).textContent =
            tables.filter(
                t => t.status === "Occupied"
            ).length;


        document.getElementById(
            "reservedCount"
        ).textContent =
            tables.filter(
                t => t.status === "Reserved"
            ).length;


        document.getElementById(
            "cleaningCount"
        ).textContent =
            tables.filter(
                t => t.status === "Cleaning"
            ).length;


        const container =
            document.getElementById(
                "tableList"
            );

        container.innerHTML = "";


        tables.forEach(table => {

            let detail =
                `Capacity: ${table.capacity} seats`;

            let actions = "";


            if (table.status === "Available") {

                actions = `
                    <button
                        class="btn action-btn btn-outline-success"
                        onclick="openTable(${table.id})"
                    >
                        Open Table
                    </button>
                `;
            }


            if (table.status === "Reserved") {

                actions = `
                    <button
                        class="btn action-btn btn-outline-primary"
                        onclick="checkInTable(${table.id})"
                    >
                        Check In Customer
                    </button>
                `;
            }


            if (table.status === "Occupied") {

                detail =
                    table.orderDate
                    ? `Active: ${formatElapsed(table.orderDate)}`
                    : `Capacity: ${table.capacity} seats`;


                actions = `
                    <button
                        class="btn action-btn btn-outline-secondary"
                        onclick="viewOrder(${table.id})"
                    >
                        View Order
                    </button>

                    <button
                        class="btn action-btn btn-outline-danger"
                        ${
                            table.activeOrderId
                            ? ""
                            : "disabled"
                        }
                        onclick="requestBill(
                            ${table.activeOrderId || 0}
                        )"
                    >
                        Request Bill
                    </button>
                `;
            }


            container.innerHTML += `
                <div class="col-md-6 col-lg-4">

                    <div
                        class="table-card
                        status-${table.status.toLowerCase()}"
                    >

                        <div class="table-card-header">

                            <h5>
                                ${table.tableNumber}
                            </h5>

                            <span
                                class="status-badge
                                status-${table.status.toLowerCase()}"
                            >
                                ${table.status}
                            </span>

                        </div>


                        <div class="table-detail">
                            ${detail}
                        </div>


                        <div class="table-actions">
                            ${actions}
                        </div>

                    </div>

                </div>
            `;
        });

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


async function openTable(tableId) {

    await changeTableStatus(
        tableId,
        "Occupied",
        "Table opened."
    );
}


async function checkInTable(tableId) {

    await changeTableStatus(
        tableId,
        "Occupied",
        "Customer checked in."
    );
}


async function changeTableStatus(
    tableId,
    status,
    message
) {

    try {

        await apiRequest(
            `${API}/tables/${tableId}/status`,
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


        showMessage(
            message,
            "success"
        );


        await loadTables();

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


async function viewOrder(tableId) {

    try {

        const order =
            await apiRequest(
                `${API}/tables/${tableId}/order`
            );


        const items =
            order.items.length === 0
            ? `
                <p class="text-muted">
                    No order items.
                </p>
            `
            : order.items.map(item => `
                <div class="order-line">

                    <div>

                        <strong>
                            Dish #${item.itemId}
                        </strong>

                        <div class="small text-muted">
                            Qty: ${item.quantity}
                            |
                            Status: ${item.status}
                        </div>

                    </div>

                    <div>
                        $
                        ${(
                            item.quantity *
                            item.unitPrice
                        ).toFixed(2)}
                    </div>

                </div>
            `).join("");


        document.getElementById(
            "orderModalLabel"
        ).textContent =
            `${order.tableNumber}
            - Order #${order.orderId}`;


        document.getElementById(
            "orderModalBody"
        ).innerHTML = `

            <p>
                <strong>Order Status:</strong>
                ${order.orderStatus}
            </p>

            <p>
                <strong>Total:</strong>
                $${Number(
                    order.totalAmount
                ).toFixed(2)}
            </p>

            <hr>

            ${items}
        `;


        const modal =
            new bootstrap.Modal(
                document.getElementById(
                    "orderModal"
                )
            );

        modal.show();

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


async function requestBill(orderId) {

    if (!orderId) {

        showMessage(
            "This table has no active order.",
            "warning"
        );

        return;
    }


    try {

        await apiRequest(
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
            "Payment requested.",
            "success"
        );

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


async function loadReadyFood() {

    try {

        const items =
            await apiRequest(
                `${API}/service/ready-food`
            );


        const container =
            document.getElementById(
                "readyFoodList"
            );


        if (items.length === 0) {

            container.innerHTML = `
                <div class="empty-state">
                    No food is ready for serving.
                </div>
            `;

            return;
        }


        const groups = {};


        items.forEach(item => {

            const key =
                item.tableId;


            if (!groups[key]) {

                groups[key] = {
                    tableNumber:
                        item.tableNumber,

                    items: []
                };
            }


            groups[key].items.push(
                item
            );
        });


        container.innerHTML =
            Object.values(groups)
                .map(group => `

                    <div class="service-card">

                        <div class="service-info">

                            <strong>
                                ${group.tableNumber}
                            </strong>

                            ${group.items
                                .map(item => `

                                    <div>
                                        ${item.quantity}x
                                        Dish #${item.itemId}

                                        ${
                                            item.specialNote
                                            ? ` - ${item.specialNote}`
                                            : ""
                                        }
                                    </div>

                                `)
                                .join("")}

                        </div>


                        <div class="service-actions">

                            ${group.items
                                .map(item => `

                                    <button
                                        class="btn btn-success btn-sm"
                                        onclick="
                                            markServed(
                                                ${item.orderItemId}
                                            )
                                        "
                                    >
                                        Mark Served
                                    </button>

                                `)
                                .join("")}

                        </div>

                    </div>

                `)
                .join("");

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


async function markServed(
    orderItemId
) {

    try {

        await apiRequest(
            `${API}/service/serve/${orderItemId}`,
            {
                method: "PUT"
            }
        );


        showMessage(
            "Food marked as served.",
            "success"
        );


        await loadReadyFood();

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


async function loadAssistanceCalls() {

    try {

        const calls =
            await apiRequest(
                `${API}/service/assistance`
            );


        const container =
            document.getElementById(
                "assistanceList"
            );


        if (calls.length === 0) {

            container.innerHTML = `
                <div class="empty-state">
                    No customer service calls.
                </div>
            `;

            return;
        }


        container.innerHTML =
            calls.map(call => `

                <div class="service-card">

                    <div class="service-info">

                        <strong>
                            ${call.tableNumber}
                        </strong>

                        <div>
                            Elapsed:
                            ${formatSeconds(
                                call.elapsedSeconds
                            )}
                        </div>

                    </div>


                    <button
                        class="btn btn-primary btn-sm"
                        onclick="
                            resolveCall(
                                ${call.tableId}
                            )
                        "
                    >
                        Resolve Call
                    </button>

                </div>

            `).join("");

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


async function resolveCall(
    tableId
) {

    try {

        await apiRequest(
            `${API}/service/assist/${tableId}/resolve`,
            {
                method: "PUT"
            }
        );


        showMessage(
            "Customer call resolved.",
            "success"
        );


        await loadAssistanceCalls();

    } catch (error) {

        showMessage(
            error.message,
            "danger"
        );
    }
}


function formatElapsed(
    orderDate
) {

    const start =
        new Date(
            orderDate.replace(
                " ",
                "T"
            )
        );


    if (
        Number.isNaN(
            start.getTime()
        )
    ) {
        return "--:--:--";
    }


    const seconds =
        Math.max(
            0,

            Math.floor(
                (
                    Date.now() -
                    start.getTime()
                ) / 1000
            )
        );


    return formatSeconds(
        seconds
    );
}


function formatSeconds(
    totalSeconds
) {

    const hours =
        Math.floor(
            totalSeconds / 3600
        );


    const minutes =
        Math.floor(
            (
                totalSeconds % 3600
            ) / 60
        );


    const seconds =
        totalSeconds % 60;


    return [
        hours,
        minutes,
        seconds
    ]
    .map(
        value =>
            String(value)
                .padStart(
                    2,
                    "0"
                )
    )
    .join(":");
}


function showMessage(
    message,
    type
) {

    document.getElementById(
        "message"
    ).innerHTML = `

        <div
            class="
                alert
                alert-${type}
                alert-dismissible
                fade
                show
            "
        >

            ${message}

            <button
                type="button"
                class="btn-close"
                data-bs-dismiss="alert"
            >
            </button>

        </div>
    `;
}


loadDashboard();

setInterval(
    loadDashboard,
    5000
);