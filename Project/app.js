const $ = (s) => document.querySelector(s),
    $$ = (s) => [...document.querySelectorAll(s)];
const esc = (s) =>
    String(s).replace(
        /[&<>"]/g,
        (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" })[c],
    );
const CATEGORIES = [
    "MPV",
    "SUV",
    "Sedan",
    "Hatchback",
    "Pickup Truck",
    "Bike",
    "Truck",
];
const C = [
    "#b3262d",
    "#555d66",
    "#8a9096",
    "#e8e8e8",
    "#2b5fa8",
    "#1f2a36",
    "#c9ccd1",
    "#2d6a4f",
    "#e07a1f",
    "#6b3fa0",
    "#222",
    "#d9d9d9",
];
// Real car photography served by Unsplash (free-to-use image URLs).
const CAR_PHOTOS = {
    audi: "https://images.unsplash.com/photo-1741029482505-a1cf606cb216?auto=format&fit=crop&w=1400&q=85",
    mercedes:
        "https://images.unsplash.com/photo-1638980707800-8f6d3fce4598?auto=format&fit=crop&w=1400&q=85",
    porsche:
        "https://images.unsplash.com/photo-1503376780353-7e6692767b70?auto=format&fit=crop&w=1400&q=85",
    sedan: "https://images.unsplash.com/photo-1578877742178-1e9773a5d2c4?auto=format&fit=crop&w=1400&q=85",
    hatchback:
        "https://images.unsplash.com/photo-1690818723690-2392147543e2?auto=format&fit=crop&w=1400&q=85",
    suv: "https://images.unsplash.com/photo-1519641471654-76ce0107ad1b?auto=format&fit=crop&w=1400&q=85",
    touring:
        "https://images.unsplash.com/photo-1549317661-bd32c8ce0db2?auto=format&fit=crop&w=1400&q=85",
    pickup: "https://images.unsplash.com/photo-1614484393797-257ee1d83a1e?auto=format&fit=crop&w=1400&q=85",
};
const PHOTO_BY_TYPE = {
    MPV: [CAR_PHOTOS.mercedes, CAR_PHOTOS.touring, CAR_PHOTOS.porsche],
    SUV: [CAR_PHOTOS.audi, CAR_PHOTOS.suv, CAR_PHOTOS.mercedes],
    Sedan: [CAR_PHOTOS.sedan],
    Hatchback: [CAR_PHOTOS.hatchback],
    "Pickup Truck": [CAR_PHOTOS.pickup, CAR_PHOTOS.suv],
};
const carPhoto = (src, alt, className = "car-photo") =>
    `<img class="${className}" src="${src}" alt="${esc(alt)}" loading="lazy" decoding="async">`;
const photoFor = (v) => {
    const photos = PHOTO_BY_TYPE[v.type] || [CAR_PHOTOS.porsche];
    return photos[Math.abs(v.id) % photos.length];
};

/* ===== Classes mirroring the C++ code (REMOVED) ===== */
// The JavaScript-side database and model classes were removed to connect to the C++ backend.

/* ===== Dummy data (REMOVED) ===== */
// Dummy data removed to connect to the C++ backend.
let ft = "MPV";

let token = localStorage.getItem("rentrideToken") || "";
let currentUser = null;
let vehicles = [];
let bookings = [];
let payments = [];
let currentCategory = "MPV";
let dashboardTab = "all";
let selectedVehicleId = 0;
let loginRole = "customer";

async function api(path, method = "GET", data = null) {
    const headers = {};
    if (token) headers.Authorization = "Bearer " + token;
    const options = { method, headers };
    if (data) {
        headers["Content-Type"] =
            "application/x-www-form-urlencoded;charset=UTF-8";
        options.body = new URLSearchParams(data).toString();
    }
    const response = await fetch(path, options);
    const result = await response.json();
    if (!response.ok) throw new Error(result.error || "Request failed");
    return result;
}

const toast = (m) => {
    const t = $("#toast");
    t.textContent = m;
    t.classList.add("on");
    clearTimeout(toast.h);
    toast.h = setTimeout(() => t.classList.remove("on"), 2800);
};
function updateRegistrationFields() {
    const isCustomer = $("#regf [name=role]").value === "customer";
    $("#license-label").hidden = !isCustomer;
    $("#license-input").hidden = !isCustomer;
    $("#license-input").required = isCustomer;
}
function go(v, s) {
    if ((v === "dash" || v === "bill") && !currentUser) {
        toast("Please log in first");
        v = "login";
    }
    $$("main").forEach((m) => (m.hidden = m.id !== "v-" + v));
    document.body.className =
        v === "home" ? "home" : v === "login" ? "login" : "";
    $$("nav a").forEach((a) =>
        a.classList.toggle(
            "on",
            a.dataset.v === v && (a.dataset.s || "") === (s || ""),
        ),
    );
    if (v === "dash") renderDashboard();
    if (v === "bill") renderBilling();
    const t = s && $("#" + s);
    t ? t.scrollIntoView({ behavior: "smooth" }) : scrollTo(0, 0);
}
function openBooking(id) {
    if (!currentUser) {
        toast("Log in as a customer to book");
        return go("login");
    }
    if (currentUser.role !== "customer")
        return toast("Only customers can book vehicles");
    selectedVehicleId = id;
    const v = vehicles.find((item) => item.id === id);
    $("#mt").textContent = v.name + " · $" + v.price + " / day";
    $("#days").value = 1;
    $("#md").hidden = false;
}

/* ===== Home ===== */
const H = [
    {
        t: "Affordable Car Rentals, Anytime, Anywhere",
        p: "Explore an extensive selection of premium vehicles tailored to match your style, preferences, and budget. Enjoy the convenience of all services under one roof.",
        bg: "linear-gradient(120deg,#1b1a14,#4a3b25 60%,#2c2a22)",
        image: CAR_PHOTOS.audi,
        alt: "Silver Audi photographed outside a modern building",
    },
    {
        t: "Premium Rides for Every Journey",
        p: "From city hatchbacks to family MPVs and rugged pickups, book the right vehicle in minutes and pay only for the days you drive.",
        bg: "linear-gradient(120deg,#0e1620,#1f3a52 60%,#142331)",
        image: CAR_PHOTOS.mercedes,
        alt: "Black Mercedes vehicle photographed on the road",
    },
];
let hi = 0;
function hero(i) {
    hi = (i + H.length) % H.length;
    const s = H[hi];
    $("#hero").style.background = s.bg;
    $("#ht").textContent = s.t;
    $("#hp").textContent = s.p;
    $("#hc").innerHTML = carPhoto(s.image, s.alt, "hero-photo");
    $("#dt").innerHTML = H.map(
        (_, k) => `<span class="${k === hi ? "on" : ""}"></span>`,
    ).join("");
}
$("#pv").onclick = () => hero(hi - 1);
$("#nx").onclick = () => hero(hi + 1);
setInterval(() => hero(hi + 1), 7000);
hero(0);
$("#lc").innerHTML = carPhoto(
    CAR_PHOTOS.porsche,
    "Premium rental car",
    "login-photo",
).replace('loading="lazy"', 'loading="eager"');
const io = new IntersectionObserver(
    (es, o) =>
        es.forEach((x) => {
            if (!x.isIntersecting) return;
            o.unobserve(x.target);
            const el = x.target,
                t = +el.dataset.n,
                st = performance.now();
            (function f(n) {
                const p = Math.min((n - st) / 1600, 1);
                el.textContent = Math.round(t * p) + el.dataset.s;
                if (p < 1) requestAnimationFrame(f);
            })(st);
        }),
    { threshold: 0.5 },
);
$$(".n").forEach((n) => io.observe(n));

// Helper for fetching and managing state
async function refreshData() {
    try {
        const result = await api("/api/vehicles");
        vehicles = result.vehicles || [];
        if (currentUser) {
            bookings = (await api("/api/bookings")).bookings || [];
            payments = (await api("/api/payments")).payments || [];
        } else {
            bookings = [];
            payments = [];
        }
    } catch (error) {
        toast("Failed to refresh data: " + error.message);
    }
    fleet();
}

function vehicleCard(vehicle) {
    const available = vehicle.available;
    return `<div class="cd"><div class="car-media">${carPhoto(photoFor(vehicle), `${vehicle.type} vehicle photo`, "car-photo")}</div><h3>${esc(vehicle.name)}</h3><p class="mu">$${vehicle.price} / day</p><div class="mt">${esc(vehicle.type)} · ${esc(vehicle.fuel)} · ${vehicle.seats} seats</div><button class="g" ${available ? "" : "disabled"} data-book="${vehicle.id}">${available ? "Book Now" : "Rented"}</button></div>`;
}

function fleet() {
    const vehicleTypes = [...new Set(vehicles.map((v) => v.type))].sort();
    $("#vt").innerHTML =
        '<option value="">Select vehicle type</option>' +
        vehicleTypes.map((t) => `<option>${t}</option>`).join("");
    $("#ft").innerHTML = vehicleTypes
        .map(
            (t) =>
                `<button class="${t === ft ? "on" : ""}" data-f="${t}">${t}</button>`,
        )
        .join("");
    const matching = vehicles.filter((v) => v.type === ft);
    $("#fg").innerHTML =
        matching.map(vehicleCard).join("") ||
        '<p class="mu">No vehicles in this category yet.</p>';
}

/* ===== Dashboard (role based) ===== */
function vehicleRows(list, role) {
    if (!list.length)
        return '<div class="tb"><p class="mu">No vehicles found.</p></div>';
    const rows = list
        .map((vehicle) => {
            const booking = bookings.find(
                (item) =>
                    item.vehicleId === vehicle.id && item.status === "ACTIVE",
            );
            const days = booking
                ? Math.max(
                      1,
                      Math.round(
                          (Date.parse(booking.endDate) -
                              Date.parse(booking.startDate)) /
                              86400000,
                      ) + 1,
                  )
                : 0;
            const action =
                role === "renter" && vehicle.ownerId === currentUser.id
                    ? `<button class="sm d" data-rem="${vehicle.id}">Remove</button>`
                    : role === "customer" &&
                        booking &&
                        booking.customerId === currentUser.id
                      ? `<button class="sm" data-ret="${booking.id}">Return</button>`
                      : role === "customer" && vehicle.available
                        ? `<button class="sm" data-book="${vehicle.id}">Book</button>`
                        : "";
            return `<tr><td>${vehicle.id}</td><td>${esc(vehicle.name)}</td><td>${esc(vehicle.type)}</td><td>$${Number(vehicle.price).toFixed(2)}</td><td><span class="bd ${vehicle.available ? "" : "r"}">${vehicle.available ? "Available" : "Booked"}</span></td><td>${days}</td><td>${booking ? "$" + Number(booking.total).toFixed(2) : "–"}</td><td>${action}</td></tr>`;
        })
        .join("");
    return `<div class="tb"><table><tr><th>ID</th><th>Vehicle</th><th>Type</th><th>Price/day</th><th>Status</th><th>Days</th><th>Bill</th><th></th></tr>${rows}</table></div>`;
}

async function renderDashboard() {
    if (!currentUser) return;
    try {
        await refreshData();
    } catch (error) {
        toast(error.message);
        return;
    }
    const role = currentUser.role;
    const tabs = [
        ["all", "All Vehicles"],
        ["avail", "Available"],
        ["booked", "Booked"],
    ];
    if (role === "renter") tabs.push(["add", "Add Vehicle"]);
    if (role === "admin")
        tabs.push(["renters", "Renters"], ["customers", "Customers"]);
    if (!tabs.some(([key]) => key === dashboardTab)) dashboardTab = "all";

    let content = "";
    if (dashboardTab === "renters" || dashboardTab === "customers") {
        try {
            const userRole = dashboardTab === "renters" ? "renter" : "customer";
            const users =
                (await api("/api/users?role=" + userRole)).users || [];
            const licenseHeader =
                dashboardTab === "customers" ? "<th>Driving licence</th>" : "";
            content = `<div class="tb"><table><tr><th>ID</th><th>Name</th><th>Email</th><th>Phone</th>${licenseHeader}</tr>${users.map((user) => `<tr><td>${user.id}</td><td>${esc(user.name)}</td><td>${esc(user.email)}</td><td>${esc(user.phone)}</td>${dashboardTab === "customers" ? `<td>${esc(user.license)}</td>` : ""}</tr>`).join("") || "<tr><td colspan=5>No accounts found.</td></tr>"}</table></div>`;
        } catch (error) {
            content = `<p class="er">${esc(error.message)}</p>`;
        }
    } else {
        let list = vehicles;
        if (role === "renter")
            list = vehicles.filter(
                (vehicle) => vehicle.ownerId === currentUser.id,
            );
        if (dashboardTab === "avail")
            list = list.filter((vehicle) => vehicle.available);
        if (dashboardTab === "booked")
            list = list.filter((vehicle) => !vehicle.available);
        let addForm = "";
        if (dashboardTab === "add" && role === "renter") {
            addForm = `<form id="af" class="af"><div><label>Vehicle name</label><input name="name" required></div><div><label>Daily price ($)</label><input name="price" type="number" min="1" required></div><div><label>Registration number</label><input name="registration" required></div><div><label>Type</label><select name="category">${CATEGORIES.map((category) => `<option>${esc(category)}</option>`).join("")}</select></div><div><label>Fuel</label><input name="fuel" value="Petrol" required></div><div><label>Seats</label><input name="seats" type="number" min="1" value="5" required></div><button class="y">Add vehicle</button></form>`;
            list = vehicles.filter(
                (vehicle) => vehicle.ownerId === currentUser.id,
            );
        }
        content = addForm + vehicleRows(list, role);
    }
    $("#v-dash").innerHTML =
        `<div class="pg"><h1>Welcome, ${esc(currentUser.name)}</h1><p class="mu">${role[0].toUpperCase() + role.slice(1)} · ID ${currentUser.id} · <a data-v="bill" style="cursor:pointer;text-decoration:underline">View bills</a></p><div class="tabs">${tabs.map(([key, label]) => `<button class="${key === dashboardTab ? "on" : ""}" data-t="${key}">${label}</button>`).join("")}</div>${content}</div>`;
}

/* ===== Billing: same detailed invoice and booking list for every role ===== */
function visibleRentalVehicles() {
    // Customers see only their own records; renters and admins can see the full rental list.
    return vehicles.filter(
        (v) =>
            v.bookingRecord &&
            (currentUser.role !== "customer" ||
                String(v.bookingRecord.customerId) === String(currentUser.id)),
    );
}

function renderBillingVehicles() {
    const records = bookings
        .filter((b) => b.status === "ACTIVE" || b.status === "RETURNED")
        .filter(
            (b) =>
                currentUser.role !== "customer" ||
                String(b.customerId) === String(currentUser.id),
        );
    if (!records.length) {
        return '<p class="mu">No booked vehicles or rental bills found.</p>';
    }
    return `<div class="tb"><table>
    <thead><tr><th>Booking ID</th><th>Vehicle ID</th><th>Vehicle</th><th>Booked by</th><th>Days</th><th>Rate / day</th><th>Total bill</th><th>Status</th><th>Payment</th><th>Invoice</th></tr></thead>
    <tbody>${records
        .map((b) => {
            const vehicle = vehicles.find((v) => v.id === b.vehicleId);
            const active = b.status === "ACTIVE";
            const payment = payments.find(
                (item) => String(item.bookingId) === String(b.id),
            );
            const paymentAction =
                payment?.status === "PAID"
                    ? `<span class="bd">Paid</span>`
                    : payment?.status === "PENDING"
                      ? `<span class="bd">Processing</span>`
                      : currentUser.role === "customer"
                        ? `<button type="button" class="sm" data-pay="${b.id}">${payment ? "Retry demo payment" : "Pay (demo)"}</button>`
                        : `<span class="mu">Not paid</span>`;
            return `<tr>
        <td>${b.id}</td>
        <td>${b.vehicleId}</td>
        <td>${esc(vehicle ? vehicle.name : "Unknown")}</td>
        <td>${esc(b.customerName)}</td>
        <td>${Math.max(1, Math.round((Date.parse(b.endDate) - Date.parse(b.startDate)) / 86400000) + 1)}</td>
        <td>$${Number(b.ratePerDay).toFixed(2)}</td>
        <td><strong>$${Number(b.total).toFixed(2)}</strong></td>
        <td><span class="bd ${active ? "" : "r"}">${active ? "Booked" : "Returned"}</span></td>
        <td>${paymentAction}</td>
        <td><button type="button" class="sm" data-invoice="${b.id}">View Bill</button></td>
      </tr>`;
        })
        .join("")}</tbody>
  </table></div>`;
}

function renderCustomerPaymentCards() {
    const customerBookings = bookings.filter(
        (booking) =>
            String(booking.customerId) === String(currentUser.id) &&
            (booking.status === "ACTIVE" || booking.status === "RETURNED"),
    );
    if (!customerBookings.length) {
        return '<p class="mu payment-empty">Your bill payment options will appear here after you make a booking.</p>';
    }
    return customerBookings
        .map((booking) => {
            const payment = payments.find(
                (item) => String(item.bookingId) === String(booking.id),
            );
            const vehicle = vehicles.find(
                (item) => String(item.id) === String(booking.vehicleId),
            );
            const action =
                payment?.status === "PAID"
                    ? '<span class="payment-status paid">Paid</span>'
                    : payment?.status === "PENDING"
                      ? '<span class="payment-status pending">Payment processing</span>'
                      : `<button type="button" class="pay-cta" data-pay="${booking.id}">${payment ? "Retry payment" : "Pay bill"} · $${Number(booking.total).toFixed(2)}</button>`;
            return `<article class="payment-card"><div class="payment-card-info"><strong>Booking #${booking.id}</strong><span>${esc(vehicle ? vehicle.name : "Vehicle rental")} · ${booking.status === "RETURNED" ? "Returned" : "Active"}</span><b>$${Number(booking.total).toFixed(2)}</b></div>${action}</article>`;
        })
        .join("");
}

async function renderBilling() {
    await refreshData();
    const roleNote =
        currentUser.role === "customer"
            ? "Enter a booking ID to view your rental bill. The list below contains only your bookings and rental records."
            : currentUser.role === "renter"
              ? "Enter a booking ID to generate an invoice. The list below shows bookings across the rental fleet."
              : "Enter a booking ID to generate an invoice. The list below shows all customer bookings and rental records.";
    $("#v-bill").innerHTML = `
    <div class="pg">
      <h1>Billing</h1>
      <p class="mu">${roleNote}</p>
      <form id="bf" class="bf">
        <input id="bid" type="number" min="1" step="1" placeholder="Enter booking ID" aria-label="Booking ID" required>
        <button class="y" type="submit">Generate Bill</button>
      </form>
      <div id="br" aria-live="polite"></div>
      ${currentUser.role === "customer" ? `<section class="payment-panel" aria-labelledby="payment-heading"><div><h2 id="payment-heading">Pay your bill</h2><p class="mu">Choose an unpaid booking below to start its secure demo payment process.</p></div><div class="payment-cards">${renderCustomerPaymentCards()}</div></section>` : ""}
      <h2>Booked Vehicles &amp; Rental Records</h2>
      <p class="mu" style="margin-bottom:18px">Select “View Bill” beside any listed vehicle to open its detailed RentRide invoice. Returned rentals remain listed for bill lookup.</p>
      <div id="billing-vehicles">${renderBillingVehicles()}</div>
    </div>`;
}

async function openInvoiceForBooking(bookingId) {
    const result = $("#br");
    const booking = bookings.find((b) => b.id === bookingId);
    if (!booking) {
        result.innerHTML =
            '<p class="er">Booking ID not found. Please enter a valid booking ID.</p>';
        return;
    }
    if (
        currentUser.role === "customer" &&
        String(booking.customerId) !== String(currentUser.id)
    ) {
        result.innerHTML =
            '<p class="er">Access denied. You can view only bills linked to your own bookings.</p>';
        return;
    }
    showReturnReceipt(booking);
    result.innerHTML = `<p class="mu">Invoice ${esc(booking.receiptNo || "unknown")} opened for booking ${booking.id}.</p>`;
}

function showReturnReceipt(receipt) {
    const money = (value) => "$" + Number(value).toFixed(2);
    const issued =
        receipt.createdAt || receipt.issuedAt
            ? new Date(receipt.createdAt || receipt.issuedAt).toLocaleString()
            : "Rental date range";
    const returned = receipt.status === "RETURNED";
    const statusLabel = returned ? "RETURN COMPLETED" : "ACTIVE RENTAL";
    const billTitle = returned ? "Rental Return Bill" : "Rental Bill";
    const note = returned
        ? "The vehicle return has been recorded successfully. This bill is calculated using the rental rate multiplied by the number of rented days. No additional taxes or fees are included."
        : "This invoice is for the rental recorded against this vehicle ID. The total is calculated using the daily rental rate multiplied by the number of rented days. No additional taxes or fees are included.";

    const vehicle = vehicles.find((v) => v.id === receipt.vehicleId);
    const vehicleName = vehicle ? vehicle.name : "Unknown Vehicle";
    const vehicleType = vehicle ? vehicle.type : "Unknown";
    const vehicleFuel = vehicle ? vehicle.fuel : "Unknown";
    const receiptPayment = payments.find(
        (item) => String(item.bookingId) === String(receipt.id),
    );
    const receiptPayButton = $("#receipt-pay");
    const canPayReceipt =
        currentUser?.role === "customer" && receiptPayment?.status !== "PAID";
    receiptPayButton.hidden = !canPayReceipt;
    receiptPayButton.disabled = receiptPayment?.status === "PENDING";
    receiptPayButton.textContent =
        receiptPayment?.status === "PENDING"
            ? "Payment processing..."
            : receiptPayment
              ? `Retry payment · $${Number(receipt.total).toFixed(2)}`
              : `Pay bill · $${Number(receipt.total).toFixed(2)}`;
    receiptPayButton.dataset.pay = receipt.id;

    const rentalDays = Math.max(
        1,
        Math.round(
            (Date.parse(receipt.endDate) - Date.parse(receipt.startDate)) /
                86400000,
        ) + 1,
    );

    $("#receipt-content").innerHTML = `
    <div class="receipt-top">
      <div><div class="receipt-brand">RENTRIDE</div><div class="mu" style="font-size:12px;letter-spacing:1px">VEHICLE RENTAL INVOICE</div></div>
      <span class="receipt-status">${esc(statusLabel)}</span>
    </div>
    <h2 id="receipt-title" style="margin-top:24px;font-weight:600">${esc(billTitle)}</h2>
    <div class="receipt-meta">
      <div><strong>Invoice number</strong><p>${esc(receipt.id || "N/A")}</p><p class="mu">${esc(issued)}</p></div>
      <div><strong>Billed to</strong><p>${esc(receipt.customerName)}</p><p class="mu">Customer ID: ${esc(receipt.customerId)}</p></div>
    </div>
    <strong>Vehicle details</strong>
    <table class="receipt-lines">
      <thead><tr><th>Description</th><th>Rental</th><th>Amount</th></tr></thead>
      <tbody>
        <tr><td><strong>${esc(vehicleName)}</strong><br><span class="mu">ID ${esc(receipt.vehicleId)} · ${esc(vehicleType)} · ${esc(vehicleFuel)}</span></td><td>${rentalDays} day(s)<br><span class="mu">${money(receipt.ratePerDay)} / day</span></td><td>${money(receipt.total)}</td></tr>
      </tbody>
    </table>
    <div class="receipt-total-row"><span>Total rental charges</span><span>${money(receipt.total)}</span></div>
    <div class="receipt-note">${note}</div>
  `;
    $("#receipt-modal").hidden = false;
    $("#receipt-close").focus();
}

function closeReturnReceipt() {
    $("#receipt-modal").hidden = true;
}

async function submitDemoPayment(bookingId) {
    try {
        const started = await api("/api/payments", "POST", { bookingId });
        toast("Demo payment is processing in the background...");
        let payment = null;
        for (let attempt = 0; attempt < 12; ++attempt) {
            await new Promise((resolve) => setTimeout(resolve, 300));
            const current = (await api("/api/payments")).payments || [];
            payment = current.find((item) => item.id === started.paymentId);
            if (payment && payment.status !== "PENDING") break;
        }
        await refreshData();
        if (!payment || payment.status === "PENDING") {
            toast(
                "Payment is still processing. Check the Billing page again shortly.",
            );
        } else if (payment.status === "PAID") {
            toast(`Demo payment complete · ${payment.transactionRef}`);
        } else {
            toast("Demo payment failed. You can retry it from Billing.");
        }
        const receiptPaymentButton = $("#receipt-pay");
        if (
            receiptPaymentButton &&
            receiptPaymentButton.dataset.pay === String(bookingId)
        ) {
            const updatedPayment = payments.find(
                (item) => String(item.bookingId) === String(bookingId),
            );
            if (updatedPayment?.status === "PAID") {
                receiptPaymentButton.hidden = true;
            } else if (updatedPayment?.status === "PENDING") {
                receiptPaymentButton.textContent = "Payment processing...";
                receiptPaymentButton.disabled = true;
            }
        }
        if (!$("#v-bill").hidden) await renderBilling();
    } catch (error) {
        toast(error.message);
    }
}

async function confirmVehicleBooking() {
    const vehicleId = selectedVehicleId;
    const days = Number($("#days").value);
    if (!vehicleId) {
        toast("Select a vehicle before confirming your booking.");
        return;
    }
    if (!Number.isInteger(days) || days < 1) {
        toast("Please enter a valid whole number of rental days.");
        return;
    }

    const startDate = new Date();
    const endDate = new Date(startDate);
    endDate.setDate(endDate.getDate() + days - 1);
    const formatDate = (date) => {
        const pad = (value) => String(value).padStart(2, "0");
        return `${date.getFullYear()}-${pad(date.getMonth() + 1)}-${pad(date.getDate())}`;
    };

    const confirmButton = $("#mok");
    confirmButton.disabled = true;
    confirmButton.textContent = "Booking...";
    try {
        const result = await api("/api/bookings", "POST", {
            vehicleId,
            startDate: formatDate(startDate),
            endDate: formatDate(endDate),
            pickupLocation: "RentRide demo pickup",
        });
        $("#md").hidden = true;
        toast(result.message || "Vehicle booked successfully.");
        await refreshData();
        await renderDashboard();
    } catch (error) {
        toast(error.message);
    } finally {
        confirmButton.disabled = false;
        confirmButton.textContent = "Confirm";
    }
}

/* ===== Events ===== */
document.addEventListener("click", async (e) => {
    const g = (s) => e.target.closest(s);
    let a;
    if ((a = g("#mcl"))) {
        $("#md").hidden = true;
    } else if ((a = g("#mok"))) {
        await confirmVehicleBooking();
    } else if ((a = g("#auth"))) {
        if (currentUser) {
            token = "";
            localStorage.removeItem("rentrideToken");
            currentUser = null;
            a.textContent = "Log in";
            a.dataset.v = "login";
            toast("Logged out");
            go("home");
        } else {
            go("login");
        }
    } else if ((a = g("[data-v]"))) go(a.dataset.v, a.dataset.s);
    else if ((a = g("[data-register]"))) {
        $("#lf").hidden = true;
        $("#regf").hidden = false;
        updateRegistrationFields();
    } else if ((a = g("[data-login]"))) {
        $("#regf").hidden = true;
        $("#lf").hidden = false;
    } else if ((a = g("[data-f]"))) {
        ft = a.dataset.f;
        fleet();
    } else if ((a = g("[data-t]"))) {
        dashboardTab = a.dataset.t;
        renderDashboard();
    } // Use renderDashboard
    else if ((a = g("[data-book]"))) openBooking(+a.dataset.book);
    else if ((a = g("[data-invoice]"))) {
        const bookingId = Number(a.dataset.invoice);
        $("#bid").value = bookingId;
        openInvoiceForBooking(bookingId);
    } else if ((a = g("[data-pay]"))) {
        submitDemoPayment(Number(a.dataset.pay));
    } else if ((a = g("[data-ret]"))) {
        const bookingId = +a.dataset.ret;
        try {
            await api("/api/return", "POST", { bookingId });
            await renderDashboard();
            const returnedBooking = bookings.find(
                (booking) => booking.id === bookingId,
            );
            if (returnedBooking) {
                showReturnReceipt(returnedBooking);
                toast("Vehicle returned. Your bill is ready.");
            } else {
                toast("Vehicle returned successfully.");
            }
        } catch (error) {
            toast(error.message);
        }
    } else if ((a = g("[data-rem]"))) {
        const vehicleId = +a.dataset.rem;
        try {
            const result = await api(
                "/api/vehicles?id=" + encodeURIComponent(vehicleId),
                "DELETE",
            );
            toast(result.message || "Vehicle removed successfully.");
            await refreshData();
            renderDashboard(); // Refresh dashboard after removal
        } catch (error) {
            toast(error.message);
        }
    } else if ((a = g("[data-r]"))) {
        loginRole = a.dataset.r;
        $$("[data-r]").forEach((b) => b.classList.toggle("on", b === a));
    }
});

$("#regf [name=role]").addEventListener("change", updateRegistrationFields);
updateRegistrationFields();

document.addEventListener("submit", async (e) => {
    e.preventDefault();
    const f = e.target,
        id = f.id;
    if (id === "regf") {
        const formData = new FormData(f);
        const registration = {
            role: formData.get("role"),
            name: String(formData.get("name") || "").trim(),
            email: String(formData.get("email") || "").trim(),
            phone: String(formData.get("phone") || "").trim(),
            license: String(formData.get("license") || "").trim(),
            password: String(formData.get("password") || ""),
        };
        try {
            const result = await api("/api/register", "POST", registration);
            loginRole = registration.role;
            $$("[data-r]").forEach((button) =>
                button.classList.toggle("on", button.dataset.r === loginRole),
            );
            $("#regf").hidden = true;
            $("#lf").hidden = false;
            $("#lf [name=un]").value = registration.email;
            f.reset();
            updateRegistrationFields();
            toast(
                `Account created. Your user ID is ${result.id}; log in with your email and password.`,
            );
        } catch (error) {
            toast(error.message);
        }
    } else if (id === "lf") {
        // Login Form
        const formData = new FormData(f);
        const identifier = String(formData.get("un") || "").trim();
        const password = String(formData.get("pw") || "");
        if (!identifier || !password)
            return toast("Enter your username or user ID and password");

        try {
            const loginData = {
                identifier,
                password,
            };
            const result = await api("/api/login", "POST", loginData);
            token = result.token;
            localStorage.setItem("rentrideToken", token);
            currentUser = result.user;

            $$("[data-r]").forEach((b) =>
                b.classList.toggle("on", b.dataset.r === currentUser.role),
            );
            dashboardTab = "all";
            $("#auth").textContent = "Log out";

            f.reset();
            toast("Welcome, " + currentUser.name);
            go("dash");
        } catch (error) {
            toast(error.message);
        }
    } else if (id === "af") {
        // Add Vehicle Form
        if (currentUser.role !== "renter") {
            toast("Only renters can add vehicles.");
            return;
        }
        const formData = new FormData(f);
        const name = formData.get("name")?.trim();
        const price = Number(formData.get("price"));
        const registration = formData.get("registration")?.trim();
        const category = formData.get("category");
        const fuel = formData.get("fuel")?.trim();
        const seats = Number(formData.get("seats"));

        if (
            !name ||
            !(price > 0) ||
            !registration ||
            !category ||
            !fuel ||
            !(seats > 0)
        ) {
            return toast("Enter valid vehicle details");
        }

        try {
            const result = await api("/api/vehicles", "POST", {
                name,
                price,
                registration,
                category,
                fuel,
                seats,
            });
            toast(result.message || "Vehicle added successfully.");
            await refreshData();
            renderDashboard(); // Refresh dashboard after adding vehicle
        } catch (error) {
            toast(error.message);
        }
    } else if (id === "bf") {
        // Billing Form (Generate Bill)
        openInvoiceForBooking(Number($("#bid").value));
    } else if (id === "rf") {
        // Home page booking request form
        toast(
            "Booking request received! Contacting WhatsApp Customer Service. (Demo)",
        );
        f.reset();
    } else if (id === "sf") {
        // Newsletter subscription form
        toast("Subscribed! (Demo)");
        f.reset();
    }
});

$("#receipt-print").onclick = () => window.print();
$("#receipt-close").onclick = closeReturnReceipt;
$("#receipt-modal").addEventListener("click", (e) => {
    if (e.target.id === "receipt-modal") closeReturnReceipt();
});
document.addEventListener("keydown", (e) => {
    if (e.key === "Escape" && !$("#receipt-modal").hidden) closeReturnReceipt();
});

$("#eye").onclick = () => {
    const p = $("#pw");
    p.type = p.type === "password" ? "text" : "password";
};

const header = $("header");
const updateHeader = () =>
    header.classList.toggle("scrolled", window.scrollY > 24);
window.addEventListener("scroll", updateHeader, { passive: true });
updateHeader();
refreshData(); // Initial data load
go("home");
