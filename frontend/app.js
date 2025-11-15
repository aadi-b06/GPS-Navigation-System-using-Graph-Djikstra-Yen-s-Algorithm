document.addEventListener('DOMContentLoaded', () => {
    const map = L.map('map').setView([18.5204, 73.8567], 12);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
    }).addTo(map);

    const sourceSelect = document.getElementById('source');
    const destinationSelect = document.getElementById('destination');
    const findRouteButton = document.getElementById('find-route');
    const trafficToggle = document.getElementById('traffic');
    const directionsList = document.getElementById('directions');
    const distanceSpan = document.getElementById('distance');
    const timeSpan = document.getElementById('time');

    let routeLayers = [];
    let nodes = {};

    // Fetch nodes for dropdowns and for coordinate lookup
    fetch('/data/nodes.csv')
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            return response.text();
        })
        .then(data => {
            const rows = data.trim().split('\n').slice(1); // remove header
            rows.forEach(row => {
                const [id, name, lat, lon] = row.split(',');
                if (id && name && lat && lon) {
                    nodes[id] = { name, lat: parseFloat(lat), lon: parseFloat(lon) };
                    const option1 = document.createElement('option');
                    option1.value = id;
                    option1.textContent = name;
                    sourceSelect.appendChild(option1);

                    const option2 = document.createElement('option');
                    option2.value = id;
                    option2.textContent = name;
                    destinationSelect.appendChild(option2);
                }
            });
        })
        .catch(e => {
            console.error("Error fetching or parsing nodes.csv:", e);
            alert("Could not load location data. Please check the console for details.");
        });


    findRouteButton.addEventListener('click', async () => {
        const sourceId = sourceSelect.value;
        const destinationId = destinationSelect.value;
        const trafficMode = trafficToggle.checked ? 'heavy' : 'normal';

        if (sourceId === destinationId) {
            alert('Source and destination cannot be the same.');
            return;
        }

        // The backend expects names, not IDs. Let's get the names.
        const sourceName = nodes[sourceId]?.name;
        const destinationName = nodes[destinationId]?.name;

        if (!sourceName || !destinationName) {
            alert('Could not find names for selected source or destination.');
            return;
        }

        try {
            const response = await fetch(`http://localhost:18080/api/route?source=${encodeURIComponent(sourceName)}&destination=${encodeURIComponent(destinationName)}&traffic=${trafficMode}`);
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            const data = await response.json();
            
            // The backend returns a `paths` array.
            if (data.paths && data.paths.length > 0) {
                drawRoutes(data.paths);
                showDirections(data.paths);
            } else {
                alert('No routes found.');
                clearRoutes();
            }
        } catch (error) {
            console.error('Error fetching route:', error);
            alert('Failed to fetch route. The backend might not be running. Please check the console for details.');
        }
    });

    function clearRoutes() {
        routeLayers.forEach(layer => map.removeLayer(layer));
        routeLayers = [];
        directionsList.innerHTML = '';
        distanceSpan.textContent = '-';
        timeSpan.textContent = '-';
    }

    function drawRoutes(paths) {
        clearRoutes();

        const colors = ['green', 'orange', 'blue', 'purple'];

        paths.forEach((path, index) => {
            if (path.nodes) {
                drawPath(path.nodes, colors[index % colors.length]);
            }
        });
    }
    
    function drawPath(nodeIds, color) {
        const latLngs = nodeIds.map(nodeId => {
            const node = nodes[nodeId];
            return node ? [node.lat, node.lon] : null;
        }).filter(p => p !== null); // Filter out any null coordinates if a node isn't found

        if (latLngs.length > 1) {
            const polyline = L.polyline(latLngs, { color: color }).addTo(map);
            routeLayers.push(polyline);
            map.fitBounds(polyline.getBounds());
        }
    }

    function showDirections(paths) {
        // Show directions for the first path
        const firstPath = paths[0];
        if (firstPath && firstPath.nodes) {
            firstPath.nodes.forEach((nodeId, index) => {
                const node = nodes[nodeId];
                if (node) {
                    const li = document.createElement('li');
                    li.textContent = `Step ${index + 1}: ${node.name}`;
                    directionsList.appendChild(li);
                }
            });

            distanceSpan.textContent = firstPath.distance.toFixed(2);
            timeSpan.textContent = firstPath.time.toFixed(2);
        }
    }
});