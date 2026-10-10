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

# We request the exact same subdomain so Vercel doesn't break!
npx localtunnel --port 8080 --subdomain kanya-mart-api-v2

# When user presses Ctrl+C and kills localtunnel, kill the backend too
kill $BACKEND_PID
echo "Server stopped."
