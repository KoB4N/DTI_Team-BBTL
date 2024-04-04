var inputField;

window.onload = function() {
    inputField = document.getElementById('group-size');
    inputField.placeholder = 'e.g. 2'; // Set placeholder text

    var maxValue = 2; // Maximum value allowed for 2 digits

    inputField.addEventListener('focus', function() {
        if (inputField.value === 'e.g. 2' || inputField.value === '0') {
            inputField.value = '';
            inputField.style.color = 'black'; 
        }
    });

    inputField.addEventListener('blur', function() {
        if (inputField.value === '') {
            inputField.value = 'e.g. 2';
            inputField.style.color = 'gray'; 
        }
    });

    inputField.addEventListener('input', function(event) {
        validateInput(event, inputField, maxValue);
    });
};

function validateInput(event, inputField, maxValue) {
    var currentValue = inputField.value.replace(/\D/g, ''); // Allow only digits
    currentValue = currentValue.slice(0, 3); // Limit to 3 characters

    // If current value exceeds max value or is 0 or less, truncate it
    if (parseInt(currentValue) > maxValue || parseInt(currentValue) <= 0) {
        currentValue = currentValue.slice(0, -1);
    }

    inputField.value = currentValue;
};

function submitPax() {
    event.preventDefault();

    if (inputField.value === 'e.g. 2' || inputField.value === '' || parseInt(inputField.value) <= 0) {
        alert("Please enter a valid Group Size.");
    } else {
        var confirmation = confirm("You entered your Group Size as " + inputField.value + " Pax?");
        if (confirmation) {
            window.location.href = "group.html";
        }
    }
}
