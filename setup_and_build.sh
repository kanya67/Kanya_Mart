#!/bin/bash
set -e

echo "=========================================="
echo "    KanyaMart Build & Run Script"
echo "=========================================="

echo "[1/4] Installing system dependencies (sudo required)..."
sudo apt-get update
sudo apt-get install -y git gcc g++ cmake libjsoncpp-dev uuid-dev zlib1g-dev openssl libssl-dev libpq-dev postgresql postgresql-contrib

echo "[2/4] Setting up PostgreSQL database and user..."
# Switch to postgres user and run the setup
sudo -u postgres psql -c "CREATE DATABASE kanyamart;" || true
sudo -u postgres psql -c "CREATE USER kanyamart_user WITH ENCRYPTED PASSWORD 'KanyaMart2026Secure';" || true
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE kanyamart TO kanyamart_user;" || true

echo "[3/4] Initializing Database Schema..."
sudo -u postgres psql -d kanyamart -a -f sql/001_schema.sql
sudo -u postgres psql -d kanyamart -a -f sql/002_indexes.sql
sudo -u postgres psql -d kanyamart -a -f sql/003_seed_data.sql

echo "[4/4] Building the application..."
rm -rf build
cmake -B build -S .
cmake --build build -j$(nproc)

echo ""
echo "=========================================="
echo "    Build Complete! To run the server:"
echo "    cd build && ./kanyamart"
echo "=========================================="
