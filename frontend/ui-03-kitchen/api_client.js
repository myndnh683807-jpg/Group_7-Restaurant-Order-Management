const API = "http://localhost:8080/api";

async function loadKitchenItems() {

    const response =
        await fetch(
            `${API}/kitchen/items`
        );

    const items =
        await response.json();

    const body =
        document.getElementById(
            "kitchenBody"
        );

    body.innerHTML = "";

    items.forEach(item => {

        body.innerHTML += `
            <tr>

                <td>
                    ${item.orderItemId}
                </td>

                <td>
                    ${item.orderId}
                </td>

                <td>
                    ${item.itemName}
                </td>

                <td>
                    ${item.quantity}
                </td>

                <td>
                    ${item.specialNote || ""}
                </td>

                <td>
                    ${item.status}
                </td>

                <td>

                    <button
                        class="btn btn-warning btn-sm"
                        onclick="
                            updateStatus(
                                ${item.orderItemId},
                                'Preparing'
                            )
                        "
                    >
                        Preparing
                    </button>

                    <button
                        class="btn btn-success btn-sm"
                        onclick="
                            updateStatus(
                                ${item.orderItemId},
                                'Ready'
                            )
                        "
                    >
                        Ready
                    </button>

                </td>

            </tr>
        `;
    });
}

async function updateStatus(
    itemId,
    status
) {

    const response =
        await fetch(
            `${API}/kitchen/items/${itemId}/status`,
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
        loadKitchenItems();
    }
}

loadKitchenItems();