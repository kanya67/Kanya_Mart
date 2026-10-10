#!/bin/bash

echo "====================================="
echo "   STARTING KANYAMART LOCAL SERVER   "
echo "====================================="

# Check if the build directory exists
if [ ! -f "./build/kanyamart" ]; then
    echo "Executable not found! Compiling the backend..."
    cmake --build build -j$(nproc)
fi

echo "[1/2] Starting the C++ Backend Server..."
./build/kanyamart > server.log 2>&1 &
BACKEND_PID=$!
sleep 2
echo "Backend is running! (Logs saved to server.log)"

echo "[2/2] Starting LocalTunnel to connect to Vercel..."
echo "Your Vercel site should automatically connect to this tunnel."
echo "Press Ctrl+C to stop the server."

# Trap Ctrl+C to stop the loop and kill the backend
trap "kill $BACKEND_PID; echo 'Server stopped.'; exit" INT TERM

# We request the exact same subdomain so Vercel doesn't break!
echo "Starting localtunnel loop..."
while true; do
    npx localtunnel --port 8080 --subdomain faaliha-mart-api
    echo "Localtunnel disconnected. Reconnecting in 3 seconds..."
    sleep 3
done
