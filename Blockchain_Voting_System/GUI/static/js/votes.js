function fetchVotes() {
    fetch('/fetch_votes')
      .then(response => response.json())
      .then(data => {
        const candidates = data.candidates;
        const votes = data.votes;
  
        const ctx = document.getElementById("voteChart").getContext("2d");

        new Chart(ctx, {
          type: 'bar', // Or 'horizontalBar' for horizontal cubes
          data: {
            labels: candidates,
            datasets: [{
                label: 'Votes',
              data: votes,
              backgroundColor: 'green', // Use the created gradient
              borderWidth: 1
            }]
          },
                options: {
                    scales: {
                        y: {
                            beginAtZero: true
                        }
                    }
                }
            });
        });
}

document.addEventListener("DOMContentLoaded", function () {
    fetchVotes();
});

document.getElementById("refresh").addEventListener("click", function() {

    window.location.href = "view_votes";
});
