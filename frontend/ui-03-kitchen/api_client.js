const API = "http://localhost:8080/api";

async function loadKitchenItems(){
    try{
        const response = await fetch(`${API}/kitchen/items`);
        const items = await response.json();

        preparingList.innerHTML="";
        pendingList.innerHTML="";
        readyList.innerHTML="";

        items.forEach(item=>{
            const card=`
            <div class="order-card">
                <div class="item-name">${item.itemName}</div>
                <div>Order #${item.orderId}</div>
                <div>Quantity: <b>${item.quantity}</b></div>
                ${item.specialNote ? `<div class="note">Note: ${item.specialNote}</div>`:""}
                <div class="mt-2">
                    <button class="btn btn-warning btn-status mb-2"
                    onclick="updateStatus(${item.orderItemId},'Preparing')">
                    🔥 Preparing
                    </button>

                    <button class="btn btn-success btn-status"
                    onclick="updateStatus(${item.orderItemId},'Ready')">
                    ✅ Ready
                    </button>
                </div>
            </div>`;

            if(item.status==="Preparing")
                preparingList.innerHTML+=card;
            else if(item.status==="Ready")
                readyList.innerHTML+=card;
            else
                pendingList.innerHTML+=card;
        });
    }catch(e){
        console.error(e);
    }
}

async function updateStatus(id,status){
    await fetch(`${API}/kitchen/items/${id}/status`,{
        method:"PUT",
        headers:{"Content-Type":"application/json"},
        body:JSON.stringify({status})
    });
    loadKitchenItems();
}

loadKitchenItems();
