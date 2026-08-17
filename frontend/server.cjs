const http = require("http");
const { spawn } = require("child_process");
const WebSocket = require("ws");
const path = require("path");

const PORT = 3001;

const projectRoot = path.resolve(__dirname, "..");
const application = path.join(projectRoot, "suspension_app");

console.log("[BRIDGE] Project root:", projectRoot);
console.log("[BRIDGE] Application:", application);

const server = http.createServer((req, res) => {
    if (req.url === "/health") {
        res.writeHead(200, {
            "Content-Type": "application/json",
            "Access-Control-Allow-Origin": "*"
        });

        res.end(JSON.stringify({
            status: "running"
        }));

        return;
    }

    res.writeHead(404);
    res.end("Not found");
});

const wss = new WebSocket.Server({
    server
});

let latestTelemetry = null;

function broadcast(data) {
    const message = JSON.stringify(data);

    for (const client of wss.clients) {
        if (client.readyState === WebSocket.OPEN) {
            client.send(message);
        }
    }
}

wss.on("connection", (socket) => {
    console.log("[BRIDGE] React client connected");

    if (latestTelemetry) {
        socket.send(JSON.stringify(latestTelemetry));
    }

    socket.on("close", () => {
        console.log("[BRIDGE] React client disconnected");
    });
});

function parseTelemetry(line) {
    const match = line.match(
        /Accel:\s*([-+]?\d*\.?\d+)\s*g\s*\|\s*Force:\s*([-+]?\d*\.?\d+)\s*N\s*\|\s*Requested:\s*([-+]?\d*\.?\d+)\s*A(?:\s*\|\s*Applied:\s*([-+]?\d*\.?\d+)\s*A)?(?:\s*\|\s*Temp:\s*([-+]?\d*\.?\d+)\s*C)?\s*\|\s*SafeMode:\s*(YES|NO)/
    );

    if (!match) {
        return null;
    }

    return {
        type: "telemetry",
        timestamp: Date.now(),

        accelerationG: Number(match[1]),
        forceNewton: Number(match[2]),
        requestedCurrentAmps: Number(match[3]),

        appliedCurrentAmps:
            match[4] !== undefined
                ? Number(match[4])
                : Number(match[3]),

        temperatureCelsius:
            match[5] !== undefined
                ? Number(match[5])
                : 25,

        safeMode: match[6] === "YES"
    };
}

console.log("[BRIDGE] Starting suspension application...");

const child = spawn(application, [], {
    cwd: projectRoot,
    stdio: ["pipe", "pipe", "pipe"]
});

child.stdout.on("data", (chunk) => {
    const text = chunk.toString();

    console.log("[C++]", text.trim());

    const telemetry = parseTelemetry(text);

    if (telemetry) {
        latestTelemetry = telemetry;

        console.log("[BRIDGE] Telemetry:", telemetry);

        broadcast(telemetry);
    }
});

child.stderr.on("data", (chunk) => {
    console.error("[C++ ERROR]", chunk.toString());
});

child.on("error", (error) => {
    console.error("[BRIDGE] Failed to start suspension_app:");
    console.error(error);
});

child.on("exit", (code, signal) => {
    console.log(
        `[BRIDGE] suspension_app exited. code=${code} signal=${signal}`
    );
});

server.listen(PORT, () => {
    console.log("");
    console.log("======================================");
    console.log(" Suspension Telemetry Bridge");
    console.log("======================================");
    console.log(` WebSocket: ws://localhost:${PORT}`);
    console.log(` Health:    http://localhost:${PORT}/health`);
    console.log("======================================");
    console.log("");
});