const inventoryTableBody = document.querySelector('#inventory-table tbody');
   const addChemicalForm = document.getElementById('add-chemical-form');
   const chemicalForm = document.getElementById('chemical-form');
   const chemicalIdInput = document.getElementById('chemicalId');
   const isExpiryRadio = document.getElementById('isExpiry');
   const isDOMRadio = document.getElementById('isDOM');
   const showAllRadio = document.getElementById('showAll');
   const showActiveRadio = document.getElementById('showActive');
   const showInactiveRadio = document.getElementById('showInactive');

   let allChemicals = [
       { id: '1', dateReceived: '2024-03-01', productName: 'Cydectin', quantity: 500, quantityUnit: 'mL', expiryDate: '2025-03-01', whp: 28, batchNumber: 'A123', updatedBy: 'John Doe', status: 'Active', dateType: 'expiry' },
       { id: '2', dateReceived: '2024-03-15', productName: 'Vaccine X', quantity: 100, quantityUnit: 'mL', expiryDate: '2024-12-31', whp: 0, batchNumber: 'B456', updatedBy: 'Jane Smith', status: 'Active', dateType: 'expiry' },
       { id: '3', dateReceived: '2024-04-01', productName: 'Antiseptic Y', quantity: 250, quantityUnit: 'g', expiryDate: '2026-04-01', whp: 14, batchNumber: 'C789', updatedBy: 'Peter Jones', status: 'Inactive', dateType: 'expiry' },
   ];

   // --- Helper Functions ---
   const clearTable = () => inventoryTableBody.innerHTML = '';

   const populateForm = (data) => {
       for (const key in data) {
           if (chemicalForm.elements[key]) {
               chemicalForm.elements[key].value = data[key] || '';
           }
       }
       if (data.dateType === 'dom') {
           isDOMRadio.checked = true;
       } else {
           isExpiryRadio.checked = true;
       }
   };

   const formatDate = (dateString) => {
       if (!dateString) return '';
       const date = new Date(dateString);
       const day = String(date.getDate()).padStart(2, '0');
       const month = String(date.getMonth() + 1).padStart(2, '0'); // Months are 0-indexed
       const year = date.getFullYear();
       return `${day}-${month}-${year}`;
   };

   // --- Main Functions ---
   const displayChemicals = (chemicals) => {
       clearTable();
       chemicals.forEach(chemical => {
           const row = inventoryTableBody.insertRow();
           const cells = [
               formatDate(chemical.dateReceived),
               chemical.productName,
               `${chemical.quantity} ${chemical.quantityUnit}`,
               formatDate(chemical.expiryDate),
               chemical.whp,
               chemical.batchNumber,
               chemical.updatedBy,
               chemical.status
           ];
           cells.forEach(text => row.insertCell().textContent = text);

           const actionsCell = row.insertCell();
           const statusButton = document.createElement('button');
           statusButton.textContent = chemical.status === 'Active' ? 'Deactivate' : 'Activate';
           statusButton.onclick = () => changeStatus(chemical.id, chemical.status === 'Active' ? 'Inactive' : 'Active');
           actionsCell.appendChild(statusButton);

           const editButton = document.createElement('button');
           editButton.textContent = 'Edit';
           editButton.onclick = () => editChemical(chemical.id);
           actionsCell.appendChild(editButton);
       });
   };

   const showAllChemicals = () => {
       displayChemicals(allChemicals);
       showAllRadio.checked = true;
   };

   const showActiveChemicals = () => {
       displayChemicals(allChemicals.filter(c => c.status === 'Active'));
       showActiveRadio.checked = true;
   };

   const showInactiveChemicals = () => {
       displayChemicals(allChemicals.filter(c => c.status === 'Inactive'));
       showInactiveRadio.checked = true;
   };

   const openAddChemicalForm = () => {
       addChemicalForm.style.display = 'block';
       chemicalForm.reset();
       chemicalIdInput.value = '';
       isExpiryRadio.checked = true; // Default to Expiry
   };

   const closeAddChemicalForm = () => addChemicalForm.style.display = 'none';

   const editChemical = (id) => {
       openAddChemicalForm();
       const chemical = allChemicals.find(chemical => chemical.id === id);
       if (chemical) {
           populateForm(chemical);
       }
   };

   const changeStatus = (id, status) => {
       const chemical = allChemicals.find(chemical => chemical.id === id);
       if (chemical) {
           chemical.status = status;
           displayChemicals(allChemicals);
       }
   };

   chemicalForm.addEventListener('submit', (event) => {
       event.preventDefault();
       const formData = new FormData(chemicalForm);
       const data = Object.fromEntries(formData.entries());
       data.dateType = document.querySelector('input[name="dateType"]:checked').value; // Get selected radio button

       if (chemicalIdInput.value) {
           // Edit existing
           const index = allChemicals.findIndex(c => c.id === chemicalIdInput.value);
           if (index !== -1) {
               data.id = chemicalIdInput.value;
               allChemicals[index] = data;
           }
       } else {
           // Add new
           data.id = String(Math.random()); // Simple dummy ID
           data.status = 'Active';
           allChemicals.push(data);
       }

       displayChemicals(allChemicals);
       closeAddChemicalForm();
   });

   // --- Initialization ---
   showAllChemicals(); // Show all chemicals on load