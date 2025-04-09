// Plant Care System Dashboard JavaScript
document.addEventListener('DOMContentLoaded', function() {
  // Initialize data update
  updateData();
  setInterval(updateData, 2000);
  
  // Set up button event listeners
  document.getElementById('pump-on').addEventListener('click', function() {
    sendControl('pump', 'on');
  });
  
  document.getElementById('pump-off').addEventListener('click', function() {
    sendControl('pump', 'off');
  });
  
  document.getElementById('light-off').addEventListener('click', function() {
    sendControl('light', 'off');
  });
  
  document.getElementById('light-on').addEventListener('click', function() {
    sendControl('light', 'on');
  });
  
  document.getElementById('light-auto').addEventListener('click', function() {
    sendControl('light', 'auto');
  });
});

// Fetch sensor data from ESP8266 API
function updateData() {
  fetch('/api/data')
    .then(response => response.json())
    .then(data => {
      // Update connection status
      if (data.connected) {
        document.querySelector('.status-badge').classList.add('connected');
        document.querySelector('.status-badge').classList.remove('disconnected');
        document.getElementById('status-text').textContent = 'Connected';
        document.getElementById('status-icon').textContent = '●';
      } else {
        document.querySelector('.status-badge').classList.remove('connected');
        document.querySelector('.status-badge').classList.add('disconnected');
        document.getElementById('status-text').textContent = 'Disconnected';
        document.getElementById('status-icon').textContent = '●';
      }
      
      // Update sensor values and gauges
      updateSensor('temp', data.temperature, '°C', 15, 40);
      updateSensor('humidity', data.humidity, '%', 0, 100);
      updateSensor('light', data.light, '%', 0, 100);
      updateSensor('moisture', data.soil, '%', 0, 100);
      updateSensor('rain', data.rain, '%', 0, 100);
      updateSensor('soil', data.soil, '%', 0, 100);
      
      // Update control status indicators
      updatePumpStatus(data.pumpActive);
      updateLightStatus(data.lightMode);
      
      // Update last update time
      document.getElementById('update-time').textContent = 
        'Last update: ' + new Date().toLocaleTimeString();
    })
    .catch(error => {
      console.error('Error fetching data:', error);
      document.querySelector('.status-badge').classList.remove('connected');
      document.querySelector('.status-badge').classList.add('disconnected');
      document.getElementById('status-text').textContent = 'Disconnected';
    });
}

// Update a sensor display with new value
function updateSensor(id, value, unit, min, max) {
  // Update the value text
  document.getElementById(id + '-value').textContent = value;
  
  // Calculate percentage for the progress bar
  const percentage = Math.min(100, Math.max(0, ((value - min) / (max - min)) * 100));
  
  // Update the progress bar
  document.getElementById(id + '-bar').style.width = percentage + '%';
}

// Update pump status indicator
function updatePumpStatus(isActive) {
  const statusElement = document.getElementById('pump-status');
  const onButton = document.getElementById('pump-on');
  const offButton = document.getElementById('pump-off');
  
  if (isActive) {
    statusElement.textContent = 'ON';
    statusElement.classList.add('on');
    statusElement.classList.remove('off');
    onButton.classList.add('active');
    offButton.classList.remove('active');
  } else {
    statusElement.textContent = 'OFF';
    statusElement.classList.add('off');
    statusElement.classList.remove('on');
    onButton.classList.remove('active');
    offButton.classList.add('active');
  }
}

// Update light mode status indicator
function updateLightStatus(mode) {
  const statusElement = document.getElementById('light-status');
  const offButton = document.getElementById('light-off');
  const onButton = document.getElementById('light-on');
  const autoButton = document.getElementById('light-auto');
  
  // Reset all buttons
  offButton.classList.remove('active');
  onButton.classList.remove('active');
  autoButton.classList.remove('active');
  
  // Remove all status classes
  statusElement.classList.remove('off', 'on', 'auto');
  
  // Set appropriate status based on mode
  if (mode === 0) {
    statusElement.textContent = 'OFF';
    statusElement.classList.add('off');
    offButton.classList.add('active');
  } else if (mode === 1) {
    statusElement.textContent = 'ON';
    statusElement.classList.add('on');
    onButton.classList.add('active');
  } else if (mode === 2) {
    statusElement.textContent = 'AUTO';
    statusElement.classList.add('auto');
    autoButton.classList.add('active');
  }
}

// Send control commands to ESP8266
function sendControl(device, action) {
  const formData = new FormData();
  formData.append('device', device);
  formData.append('action', action);
  
  fetch('/api/control', {
    method: 'POST',
    body: formData
  })
  .then(response => {
    if (response.ok) {
      // Force an immediate data update
      updateData();
    }
  })
  .catch(error => {
    console.error('Error sending command:', error);
  });
}
