// Get the modal
document.addEventListener("DOMContentLoaded", function() {
    var modal = document.querySelector('.modal');
    modal.style.display = 'flex'; // Show the modal

    // Additional code to handle the modal behavior
});


var modal = document.getElementById("ipModal");

// Get the button that opens the modal
var form = document.getElementById("submitForm");

// Get the <span> element that closes the modal
var span = document.getElementsByClassName("close")[0];

function isValidIP(ip) {
    var pattern = new RegExp('^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$');
    return pattern.test(ip);
}
// When the user clicks on the button, open the modal
form.onsubmit = function(e) {
    e.preventDefault();
    var input = document.getElementById("ipInput");
    var ip = e.target.ipInput.value;
    
    if (isValidIP(ip)) {
        localStorage.setItem("serverIP", ip); // Store IP in localStorage
        modal.style.display = "none";
        
        // Send the IP address to the server
        fetch('/set_ip', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ ip: ip })
        })
        .then(response => response.json())
        .then(data => {
            // Handle response from server if needed
        })
        .catch(error => {
            console.error('Error:', error);
        });
        
    } else {
        alert("Invalid IP address. Please enter a valid IP address.");
    }
}

// Disable the close button and click outside functionality
span.style.display = "none"; // Hide the close button
modal.style.cursor = "default"; // Set cursor to default

// Check if IP exists in localStorage, if not show the modal
if (!localStorage.getItem("userIP")) {
    modal.style.display = "block";
}
