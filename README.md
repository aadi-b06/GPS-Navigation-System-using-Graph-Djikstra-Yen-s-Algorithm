# Pune GPS Navigation System

This project is a GPS navigation system for Pune that calculates and visualizes the shortest and alternate routes between two locations.

## Overview

The system is composed of a C++ backend and a web-based frontend.

- **Backend**: A C++ application using the Crow framework to provide a REST API for route calculation. It uses Dijkstra's algorithm to find the shortest path and Yen's algorithm to find alternate routes.
- **Frontend**: A web interface built with HTML, CSS, and JavaScript that uses Leaflet.js to display an interactive map. It communicates with the backend to fetch and display the routes.

## Features

- Select source and destination from a list of Pune intersections.
- Toggle between normal and heavy traffic modes.
- View the shortest path and three alternate routes on the map.
- See turn-by-turn directions.
- Display estimated travel time and distance.

## Project Structure

```
/pune-gps
  /data
    nodes.csv
    edges.csv
  /backend
    graph.h
    graph.cpp
    dijkstra.h
    dijkstra.cpp
    yen.h
    yen.cpp
    main.cpp
    CMakeLists.txt
  /frontend
    index.html
    style.css
    app.js
  CMakeLists.txt
  README.md
```

## How to Build and Run

### Prerequisites

- C++17 compiler
- CMake
- Crow C++ library

### Build

1. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```

2. Run CMake and build the project:
   ```sh
   cmake ..
   make
   ```

### Run

1. Start the backend server:
   ```sh
   ./backend/pune-gps-backend
   ```

2. Open the `frontend/index.html` file in your web browser.

## API

- **Endpoint**: `/api/route`
- **Method**: `GET`
- **Parameters**:
  - `source`: The name of the source location.
  - `destination`: The name of the destination location.
  - `traffic`: (Optional) Set to `heavy` to simulate heavy traffic.

- **Example**:
  ```
  /api/route?source=FC%20Road&destination=Viman%20Nagar&traffic=heavy
  ```