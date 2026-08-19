#!/bin/bash

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRONTEND_DIR="$PROJECT_ROOT/frontend"
BUILD_DIR="$PROJECT_ROOT/build"
SUSPENSION_APP="$PROJECT_ROOT/suspension_app"

echo "=============================================="
echo " MR-Fluid Suspension Telemetry System"
echo "=============================================="
echo "Project: $PROJECT_ROOT"
echo

# ------------------------------------------------
# Check required programs
# ------------------------------------------------

for cmd in cmake node npm; do
    if ! command -v "$cmd" >/dev/null 2>&1; then
        echo "[ERROR] Required command not found: $cmd"
        exit 1
    fi
done

# ------------------------------------------------
# Build C++ application
# ------------------------------------------------

echo "[1/5] Configuring C++ project..."

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR"

echo
echo "[2/5] Building C++ project..."

cmake --build "$BUILD_DIR" -j"$(nproc)"

# ------------------------------------------------
# Copy executable
# ------------------------------------------------

echo
echo "[3/5] Updating suspension_app..."

cp "$BUILD_DIR/project1" "$SUSPENSION_APP"
chmod +x "$SUSPENSION_APP"

echo "[OK] Executable:"
ls -lh "$SUSPENSION_APP"

# ------------------------------------------------
# Check Node dependencies
# ------------------------------------------------

echo
echo "[4/5] Checking frontend dependencies..."

cd "$FRONTEND_DIR"

if [ ! -d "node_modules" ]; then
    echo "[INFO] Installing npm dependencies..."
    npm install
else
    echo "[INFO] node_modules already exists."
fi

# ------------------------------------------------
# Cleanup function
# ------------------------------------------------

cleanup() {
    echo
    echo "=============================================="
    echo " Shutting down system..."
    echo "=============================================="

    if [ -n "${VITE_PID:-}" ] && kill -0 "$VITE_PID" 2>/dev/null; then
        echo "[STOP] Vite frontend..."
        kill "$VITE_PID" 2>/dev/null || true
    fi

    if [ -n "${BRIDGE_PID:-}" ] && kill -0 "$BRIDGE_PID" 2>/dev/null; then
        echo "[STOP] Telemetry bridge..."
        kill "$BRIDGE_PID" 2>/dev/null || true
    fi

    echo "[OK] Shutdown complete."

    exit 0
}

trap cleanup INT TERM EXIT

# ------------------------------------------------
# Start telemetry bridge
# ------------------------------------------------

echo
echo "[5/5] Starting telemetry bridge..."

cd "$FRONTEND_DIR"

node server.cjs &
BRIDGE_PID=$!

echo "[BRIDGE] PID: $BRIDGE_PID"

# ------------------------------------------------
# Wait for bridge
# ------------------------------------------------

echo
echo "[INFO] Waiting for telemetry bridge..."

for i in {1..30}; do
    if curl -sf http://localhost:3001/health >/dev/null 2>&1; then
        echo "[OK] Telemetry bridge is running."
        break
    fi

    sleep 0.5
done

if ! curl -sf http://localhost:3001/health >/dev/null 2>&1; then
    echo "[ERROR] Telemetry bridge failed to start."
    exit 1
fi

# ------------------------------------------------
# Start React/Vite frontend
# ------------------------------------------------

echo
echo "[FRONTEND] Starting Vite development server..."

npm run dev -- --host 127.0.0.1 &
VITE_PID=$!

echo "[FRONTEND] PID: $VITE_PID"

echo
echo "=============================================="
echo " SYSTEM RUNNING"
echo "=============================================="
echo
echo " C++ Control Loop : 100 Hz"
echo " Telemetry        : 10 Hz"
echo " WebSocket        : ws://localhost:3001"
echo " Health           : http://localhost:3001/health"
echo " Frontend         : http://localhost:5173"
echo
echo " Press Ctrl+C to stop the complete system."
echo "=============================================="
echo

# ------------------------------------------------
# Keep script alive
# ------------------------------------------------

wait