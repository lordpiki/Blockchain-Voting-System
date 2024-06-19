// script.js
document.addEventListener("DOMContentLoaded", function () {
    const voteButton = document.getElementById("voteButton");
    const loadingOverlay = document.getElementById("loadingOverlay");

    voteButton.addEventListener("click", function () {
        // Show loading overlay
        loadingOverlay.style.display = "flex";

        const candidate = document.getElementById('candidateNumber').value;
        console.log(candidate);
        fetch('/send_vote',{
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify({'candidate':candidate}),
          })
        .then(response => response.text())
        .then(data => {
            loadingOverlay.style.display = "none";
            window.location.href = "view_votes";
        });
    });
});
