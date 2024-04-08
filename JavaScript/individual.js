// Function to handle data transfer
// function sendDataToPython(numPax, hexValue) {
//     // const xhr = new XMLHttpRequest();
//     // xhr.open("POST", "http://localhost:5500/send_data", true);
//     // xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
//     // xhr.send(JSON.stringify({numPax: numPax}, {hexValue: hexValue}))
    
//     // Send data to Python script via HTTP POST request
//     fetch('http://127.0.0.1:5500/individual.html', {
//         method: 'POST',
//         headers: {
//         'Content-Type': 'application/json',
//         },
//         body: JSON.stringify({ numPax: numPax, hexValue: hexValue }),
//     })
//     .then(response => {
//         if (!response.ok) {
//         throw new Error('Network response was not ok');
//         }
//         return response.json();
//     })
//     .then(data => {
//         console.log('Data sent successfully:', data);
//     })
//     .catch(error => {
//         console.error('Error sending data:', error);
//     });
// }

// Function to handle color selection
function submitColor() {
    event.preventDefault();

    var slider = document.getElementById('colorSlider');
    var thumb = document.getElementById('colorThumb');
    var percentage = (thumb.offsetLeft + thumb.offsetWidth / 2) / slider.offsetWidth;
    var color = getColorAtPercentage(percentage);

    if (hexDisplay.innerText.trim() === "") {
        alert("Selection is invalid! Please try again!"); 
    } else {
        var confirmation = confirm("Are you sure of your level of individual study intensity?");
        
        if (confirmation) {
            // sendDataToPython(1, hexDisplay.innerText);
            alert("Your confirmed choice of selection is: " + hexDisplay.innerText + "\n" + "Good luck & Enjoy your study session! :))");
            window.location.href = "home.html";
        };
    }
}

// Function to get color at a specific percentage of the slider
function getColorAtPercentage(percentage) {
    // Constrain the percentage within the valid range (0 to 1)
    percentage = Math.max(0, Math.min(1, percentage));

    var startColor = [255, 255, 0]; // Yellow
    var middleColor = [255, 165, 0]; // Orange
    var endColor = [255, 0, 0]; // Red

    // Interpolate colors based on the percentage
    var color;
    if (percentage <= 0.5) {
        color = [
            Math.round(startColor[0] + (middleColor[0] - startColor[0]) * (percentage * 2)),
            Math.round(startColor[1] + (middleColor[1] - startColor[1]) * (percentage * 2)),
            Math.round(startColor[2] + (middleColor[2] - startColor[2]) * (percentage * 2))
        ];
    } else {
        color = [
            Math.round(middleColor[0] + (endColor[0] - middleColor[0]) * ((percentage - 0.5) * 2)),
            Math.round(middleColor[1] + (endColor[1] - middleColor[1]) * ((percentage - 0.5) * 2)),
            Math.round(middleColor[2] + (endColor[2] - middleColor[2]) * ((percentage - 0.5) * 2))
        ];
    }

    // Convert RGB to HEX
    return rgbToHex(color[0], color[1], color[2]);
}


// Function to convert RGB to HEX
function rgbToHex(r, g, b) {
    return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
}

function componentToHex(c) {
    var hex = c.toString(16);
    return hex.length == 1 ? "0" + hex : hex;
}

// Slider thumb draggable functionality
var thumb = document.getElementById('colorThumb');
var slider = document.getElementById('colorSlider');
var lens = document.getElementById('colorLens');
var hexDisplay = document.getElementById('colorHex');
hexDisplay.innerText = "#ffff00";

var debounceTimer; // Variable to store the debounce timer

thumb.addEventListener('mousedown', function(e) {
    document.addEventListener('mousemove', moveThumb);
    document.addEventListener('mousemove', debounceUpdateLensAndHex); // Update lens and HEX value with debouncing while dragging
    document.addEventListener('mouseup', function() {
        document.removeEventListener('mousemove', moveThumb);
        document.removeEventListener('mousemove', debounceUpdateLensAndHex); // Remove listener for updating lens and HEX value
        lens.style.display = 'none'; // Hide lens when not dragging
        hexDisplay.style.display = 'none'; // Hide HEX value display when not dragging
    });
});

// Call updateLensAndHex at initialization to display color and hex value correctly
updateLensAndHex();

// Function to move the slider thumb
function moveThumb(e) {
    var newPosition = e.clientX - slider.getBoundingClientRect().left - thumb.offsetWidth / 2;
    // Calculate the maximum position based on the slider's width and the thumb's width
    var maxPosition = slider.offsetWidth - thumb.offsetWidth;
    // Ensure the thumb stays within the slider bounds
    newPosition = Math.max(0, Math.min(maxPosition, newPosition));
    thumb.style.left = newPosition + 'px';
    debounceUpdateLensAndHex(e); // Update lens and HEX value with debouncing
}

// Function to debounce the update of lens and HEX value
function debounceUpdateLensAndHex(e) {
    clearTimeout(debounceTimer); // Clear previous debounce timer
    debounceTimer = setTimeout(function() {
        updateLensAndHex(e); // Call the function to update lens and HEX value
    }, 10); // Adjust the debounce delay as needed for smoother updates
}

// Function to update the lens position and display HEX value
function updateLensAndHex(e) {
    var thumbPosition = thumb.getBoundingClientRect();
    var percentage = (e.clientX - slider.getBoundingClientRect().left) / slider.offsetWidth;
    var color = getColorAtPercentage(percentage);

    // Update lens position
    lens.style.left = thumbPosition.left + 'px';
    lens.style.top = thumbPosition.top - lens.offsetHeight + 'px';

    // Update lens color dynamically based on percentage
    lens.style.backgroundColor = color;

    // Show lens
    lens.style.display = 'block';

    // Update HEX value display
    hexDisplay.innerText = color;

    // Position HEX value display below the thumb
    hexDisplay.style.top = thumbPosition.top + thumb.offsetHeight + 'px';
    hexDisplay.style.left = thumbPosition.left + 'px'; // Align with the thumb
    hexDisplay.style.display = 'block'; // Show HEX value display
}