const API = "http://localhost:8080/api";

async function loadBill() {

    const orderId =
        document.getElementById(
            "orderId"
        ).value;

    const response =
        await fetch(
            `${API}/payments/orders/${orderId}`
        );

    if (!response.ok) {
        alert("Order not found.");
        return;
    }

    const data =
        await response.json();

    document.getElementById(
        "bill"
    ).innerHTML = `
        <div class="bill-box">

            <h4>
                Order #${data.orderId}
            </h4>

            <p>
                Subtotal:
                ${data.subtotal}
            </p>

            <label>
                Discount
            </label>

            <input
                id="discount"
                type="number"
                value="0"
                class="form-control mb-2"
            >

            <label>
                Cashier ID
            </label>

            <input
                id="cashierId"
                type="number"
                class="form-control mb-2"
            >

            <label>
                Payment Method
            </label>

            <select
                id="paymentMethod"
                class="form-select mb-3"
            >

                <option value="Cash">
                    Cash
                </option>

                <option value="Card">
                    Card
                </option>

                <option value="QR">
                    QR
                </option>

            </select>

            <button
                class="btn btn-success"
                onclick="pay(${data.orderId})"
            >
                Payment
            </button>

        </div>
    `;
}

async function pay(orderId) {

    const discountAmount =
        Number(
            document.getElementById(
                "discount"
            ).value
        );

    const cashierId =
        Number(
            document.getElementById(
                "cashierId"
            ).value
        );

    const paymentMethod =
        document.getElementById(
            "paymentMethod"
        ).value;

    const response =
        await fetch(
            `${API}/payments`,
            {
                method: "POST",

                headers: {
                    "Content-Type":
                        "application/json"
                },

                body: JSON.stringify({
                    orderId,
                    cashierId,
                    discountAmount,
                    paymentMethod
                })
            }
        );

    if (response.ok) {

        const invoice =
            await response.json();

        alert(
            `Payment successful. Invoice ID: ${invoice.invoiceId}`
        );

    } else {

        alert(
            "Payment failed."
        );
    }
}

async function loadRevenue() {

    const date =
        document.getElementById(
            "reportDate"
        ).value;

    const response =
        await fetch(
            `${API}/reports/revenue?date=${date}`
        );

    if (!response.ok) {
        return;
    }

    const data =
        await response.json();

    document.getElementById(
        "revenue"
    ).innerHTML = `
        <div class="alert alert-info">
            Revenue: ${data.totalRevenue}
        </div>
    `;
}