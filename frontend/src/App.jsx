import { useEffect, useState } from "react";
import "./App.css";

const initialTelemetry = {
  accelerationG: 0,
  forceNewton: 0,
  requestedCurrentAmps: 0,
  appliedCurrentAmps: 0,
  temperatureCelsius: 25,
  safeMode: false,
};

const WS_URL = "ws://127.0.0.1:3001";

function App() {
  const [telemetry, setTelemetry] = useState(initialTelemetry);
  const [connected, setConnected] = useState(false);

  useEffect(() => {
    console.log(`[UI] Connecting to ${WS_URL}`);

    const socket = new WebSocket(WS_URL);

    socket.onopen = () => {
      console.log("[UI] WebSocket CONNECTED");
      setConnected(true);
    };

    socket.onmessage = (event) => {
      console.log("[UI] Telemetry received:", event.data);

      try {
        const data = JSON.parse(event.data);

        if (data.type === "telemetry") {
          setTelemetry({
            accelerationG: Number(data.accelerationG) || 0,
            forceNewton: Number(data.forceNewton) || 0,
            requestedCurrentAmps:
              Number(data.requestedCurrentAmps) || 0,
            appliedCurrentAmps:
              Number(data.appliedCurrentAmps) || 0,
            temperatureCelsius:
              Number(data.temperatureCelsius) || 0,
            safeMode: Boolean(data.safeMode),
          });
        }
      } catch (error) {
        console.error("[UI] Invalid telemetry:", error);
      }
    };

    socket.onerror = (error) => {
      console.error("[UI] WebSocket ERROR:", error);
      setConnected(false);
    };

    socket.onclose = (event) => {
      console.log(
        `[UI] WebSocket CLOSED code=${event.code} reason=${event.reason}`
      );

      setConnected(false);
    };

    return () => {
      console.log("[UI] Closing WebSocket");

      if (
        socket.readyState === WebSocket.OPEN ||
        socket.readyState === WebSocket.CONNECTING
      ) {
        socket.close();
      }
    };
  }, []);

  return (
    <main className="dashboard">
      <header className="header">
        <div>
          <h1>Real-Time Telemetry Monitor</h1>
          <p>MR-Fluid Suspension Control System</p>
        </div>

        <div className={`status ${connected ? "online" : "offline"}`}>
          <span className="status-dot"></span>

          {connected ? "SYSTEM RUNNING" : "DISCONNECTED"}
        </div>
      </header>

      <section className="metrics">
        <MetricCard
          title="ACCELERATION"
          value={telemetry.accelerationG.toFixed(2)}
          unit="g"
          color="blue"
        />

        <MetricCard
          title="DAMPING FORCE"
          value={telemetry.forceNewton.toFixed(1)}
          unit="N"
          color="purple"
        />

        <MetricCard
          title="COIL CURRENT"
          value={telemetry.appliedCurrentAmps.toFixed(2)}
          unit="A"
          color="green"
        />

        <MetricCard
          title="TEMPERATURE"
          value={telemetry.temperatureCelsius.toFixed(0)}
          unit="°C"
          color={
            telemetry.temperatureCelsius >= 85
              ? "red"
              : telemetry.temperatureCelsius >= 70
                ? "orange"
                : "cyan"
          }
        />
      </section>

      <section className="telemetry-panel">
        <div className="panel-header">
          <div>
            <h2>Live Suspension Telemetry</h2>
            <p>Real-time data from the C++ control loop</p>
          </div>

          <div
            className={`safe-mode ${
              telemetry.safeMode ? "danger" : "safe"
            }`}
          >
            {telemetry.safeMode ? "SAFE MODE" : "SAFE"}
          </div>
        </div>

        <div className="telemetry-grid">
          <TelemetryRow
            label="Acceleration"
            value={`${telemetry.accelerationG.toFixed(4)} g`}
            percentage={(telemetry.accelerationG / 2.5) * 100}
            color="blue"
          />

          <TelemetryRow
            label="Damping Force"
            value={`${telemetry.forceNewton.toFixed(2)} N`}
            percentage={(telemetry.forceNewton / 125) * 100}
            color="purple"
          />

          <TelemetryRow
            label="Requested Current"
            value={`${telemetry.requestedCurrentAmps.toFixed(4)} A`}
            percentage={(telemetry.requestedCurrentAmps / 5) * 100}
            color="orange"
          />

          <TelemetryRow
            label="Applied Current"
            value={`${telemetry.appliedCurrentAmps.toFixed(4)} A`}
            percentage={(telemetry.appliedCurrentAmps / 5) * 100}
            color="green"
          />

          <TelemetryRow
            label="Temperature"
            value={`${telemetry.temperatureCelsius.toFixed(1)} °C`}
            percentage={(telemetry.temperatureCelsius / 85) * 100}
            color="cyan"
          />
        </div>
      </section>

      <footer>
        <span>WebSocket: {WS_URL}</span>
        <span>•</span>
        <span>Telemetry: 10 Hz</span>
        <span>•</span>
        <span>Control Loop: 100 Hz</span>
      </footer>
    </main>
  );
}

function MetricCard({ title, value, unit, color }) {
  return (
    <article className={`metric-card ${color}`}>
      <div className="metric-title">{title}</div>

      <div className="metric-value">
        {value}
        <span>{unit}</span>
      </div>
    </article>
  );
}

function TelemetryRow({ label, value, percentage, color }) {
  const width = Math.max(0, Math.min(100, percentage));

  return (
    <div className="telemetry-row">
      <div className="row-top">
        <span>{label}</span>
        <strong>{value}</strong>
      </div>

      <div className="bar-background">
        <div
          className={`bar ${color}`}
          style={{ width: `${width}%` }}
        />
      </div>
    </div>
  );
}

export default App;
