document.addEventListener('DOMContentLoaded', () => {
  // Function to display an error
  function showError(elementId, message) {
    const element = document.getElementById(elementId);
    element.textContent = message;
    element.style.color = "red";
  }

  // Function to display a temporary message
  function showTemporaryMessage(elementId, message, duration = 3000) {
    const element = document.getElementById(elementId);
    element.textContent = message;
    setTimeout(() => {
      element.textContent = "";
    }, duration);
  }

  // Function to update ESP status
  function updateStatus() {
    fetch('/api/status')
      .then(resp => resp.json())
      .then(data => {
        console.log("Status received:", data);
        const statusElement = document.getElementById('esp-status');
        if (data.status === "online") {
          statusElement.textContent = `Connected (Last contact: ${new Date(data.lastContact).toLocaleString()})`;
          statusElement.style.color = "green";
        } else {
          statusElement.textContent = "Offline";
          statusElement.style.color = "red";
        }
      })
      .catch(() => {
        showError('esp-status', "Error connecting to server");
      });
  }

  // Function to create a chart
  function createChart(id, labels, datasets) {
    const ctx = document.getElementById(id).getContext('2d');
    new Chart(ctx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: datasets
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
          legend: { display: true },
        }
      }
    });
  }

  // Function to display the map
  function displayMap(data) {
    const map = L.map('mapid').setView([50.559110, 5.465493], 13); // Default position

    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      maxZoom: 19,
      attribution: '© OpenStreetMap contributors',
    }).addTo(map);

    // Add marker if coordinates are available
    if (data.gps) {
      const [latitude, longitude] = data.gps.split(',').map(coord => parseFloat(coord.trim()));
      L.marker([latitude, longitude]).addTo(map)
        .bindPopup(`Current position: ${latitude}, ${longitude}`)
        .openPopup();
    }
  }

  // Function to fetch data and generate charts
  function updateCharts() {
    fetch('/api/data')
      .then(resp => resp.json())
      .then(data => {
        console.log("Data received for charts:", data);

        const labels = data.map(entry => new Date(entry.timestamp).toLocaleString());

        // Combined temperatures
        const temperatureBMP = data.map(entry => entry.temperatureBMP);
        const temperatureDHT = data.map(entry => entry.temperatureDHT);
        createChart('temperatureChart', labels, [
          {
            label: 'Temperature BMP (°C)',
            data: temperatureBMP,
            borderColor: 'rgba(255, 99, 132, 1)',
            fill: false
          },
          {
            label: 'Temperature DHT (°C)',
            data: temperatureDHT,
            borderColor: 'rgba(255, 159, 64, 1)',
            fill: false
          }
        ]);

        // Other charts
        createChart('pressionChart', labels, [{
          label: 'Pressure (hPa)',
          data: data.map(entry => entry.pression),
          borderColor: 'rgba(54, 162, 235, 1)',
          fill: false
        }]);

        createChart('humiditéChart', labels, [{
          label: 'Humidity (%)',
          data: data.map(entry => entry.humidite),
          borderColor: 'rgba(75, 192, 192, 1)',
          fill: false
        }]);

        createChart('gazChart', labels, [{
          label: 'Gas MQ (Analog)',
          data: data.map(entry => entry.MQ_analogique),
          borderColor: 'rgba(153, 102, 255, 1)',
          fill: false
        }]);

        createChart('niveauSonoreChart', labels, [{
          label: 'Sound Level (dB SPL)',
          data: data.map(entry => entry.niveauSonoreDBSPL),
          borderColor: 'rgba(255, 206, 86, 1)',
          fill: false
        }]);

        // Display the map with the latest GPS position
        const latestData = data[data.length - 1];
        displayMap(latestData);
      })
      .catch(() => {
        showError('esp-status', "Error fetching data for charts.");
      });
  }

  // Initialize automatic updates
  setInterval(updateStatus, 5000);
  setInterval(updateCharts, 30000);

  // Initial calls
  updateStatus();
  updateCharts();
});
