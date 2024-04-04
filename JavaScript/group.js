function submitColor() {
    event.preventDefault();
    
    const colorHex = document.getElementById("colorHex").innerText;
    
    if (colorHex.trim() === "") {
        alert("No selection made! Please select a color for your group!"); 
    } else {
        alert("Your group's confirmed choice of selection is: " + colorHex + "\n" + "Good luck & Enjoy your session together! :))");
        window.location.href = "home.html";
    }
}

document.addEventListener("DOMContentLoaded", function () {
    const colorSlider = document.getElementById("colorSlider");
    const colorThumb = document.getElementById("colorThumb");
    const colorLens = document.getElementById("colorLens");
    const colorHex = document.getElementById("colorHex");

    colorThumb.style.display = "none";
    colorLens.style.display = "none";
    colorHex.style.display = "none";

    let hasInteracted = false;

    function updateColor(e) {
        if (!hasInteracted) return;
        const rect = colorSlider.getBoundingClientRect();
        let x = e.clientX - rect.left;
        let y = e.clientY - rect.top;
        const centerX = rect.width / 2;
        const centerY = rect.height / 2;

        // Calculate distance from center
        const distance = Math.sqrt((x - centerX) ** 2 + (y - centerY) ** 2);

        // If the cursor is outside the color slider circle, limit its position
        if (distance > rect.width / 2) {
            const angle = Math.atan2(y - centerY, x - centerX);
            x = centerX + (rect.width / 2) * Math.cos(angle);
            y = centerY + (rect.width / 2) * Math.sin(angle);
        }

        const angle = Math.atan2(y - centerY, x - centerX);
        const angleDeg = (angle * 180) / Math.PI + 180;

        const colors = [
            "#c87bff",
            "#eb7bff",
            "#ff5cff",
            "#ff00ff",
            "#ffffff",
            "#d1ff64",
            "#c5ff6f",
            "#b0ff85",
            "#a4ff91",
            "#7bff7b",
            "#7bffa3",
            "#7bffe9",
            "#7bffff",
            "#7bc7ff",
            "#7bb8ff",
            "#847bff",
            "#957bff",
            "#c07bff",
            "#c87bff"
        ];

        const index = Math.floor(angleDeg / (360 / colors.length));

        const color = colors[index];

        colorThumb.style.transform = `translate(-50%, -50%) translate(${x}px, ${y}px)`;
        colorLens.style.transform = `translate(-50%, -50%) translate(${x}px, ${y}px)`;
        colorLens.style.backgroundColor = color;
        colorHex.innerText = color;
        colorHex.style.color = color;
        colorHex.style.left = `${x + colorLens.offsetWidth / 2}px`;
        colorHex.style.top = `${y - colorHex.offsetHeight / 2}px`;
    }

    function showColorPicker(e) {
        hasInteracted = true;
        colorThumb.style.display = "block";
        colorLens.style.display = "block";
        colorHex.style.display = "block";
        updateColor(e);
        hasInteracted = false;
    }

    colorSlider.addEventListener("mousemove", updateColor);
    colorSlider.addEventListener("mousedown", showColorPicker);
});
