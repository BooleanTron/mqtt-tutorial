# MQTT Telemetry Stack — Quick Setup (Docker Images Only)

For students. No git, no cloning, no editing any files — just Docker.

## 1. Install Docker

Install Docker Desktop (Windows/Mac) or Docker Engine (Linux). That's the only prerequisite.

## 2. Create one file

Make a new empty folder anywhere, and inside it create a file named exactly `docker-compose.yml` with this content:

```yaml
services:
  mosquitto:
    image: vasulabs/mosquitto-tutorial:latest
    container_name: mosquitto
    ports:
      - "1883:1883"
    volumes:
      - mosquitto_data:/mosquitto/data
      - mosquitto_log:/mosquitto/log
    restart: unless-stopped

  influxdb:
    image: influxdb:2
    container_name: influxdb
    environment:
      DOCKER_INFLUXDB_INIT_MODE: setup
      DOCKER_INFLUXDB_INIT_USERNAME: admin
      DOCKER_INFLUXDB_INIT_PASSWORD: admin@123
      DOCKER_INFLUXDB_INIT_ORG: mqtt-tutorial
      DOCKER_INFLUXDB_INIT_BUCKET: telemetry
      DOCKER_INFLUXDB_INIT_ADMIN_TOKEN: admin@123
    ports:
      - "8086:8086"
    volumes:
      - influxdb_data:/var/lib/influxdb2
    restart: unless-stopped

  telegraf:
    image: vasulabs/telegraf-tutorial:latest
    container_name: telegraf
    environment:
      INFLUX_TOKEN: admin@123
      INFLUX_ORG: mqtt-tutorial
      INFLUX_BUCKET: telemetry
    depends_on:
      - mosquitto
      - influxdb
    restart: unless-stopped

  grafana:
    image: vasulabs/grafana-tutorial:latest
    container_name: grafana
    environment:
      GF_SECURITY_ADMIN_USER: admin
      GF_SECURITY_ADMIN_PASSWORD: admin@123
      INFLUX_ORG: mqtt-tutorial
      INFLUX_BUCKET: telemetry
      INFLUX_TOKEN: admin@123
    ports:
      - "3000:3000"
    volumes:
      - grafana_data:/var/lib/grafana
    depends_on:
      - influxdb
    restart: unless-stopped

volumes:
  mosquitto_data:
  mosquitto_log:
  influxdb_data:
  grafana_data:
```

## 3. Start the stack

Open a terminal inside that folder and run:

```bash
docker compose up -d
```

Docker will pull four images (mosquitto, influxdb, telegraf, grafana — the first three are custom-built with all config and the dashboard already baked in) and start everything together.

## Credentials

| Service  | Username | Password   |
|----------|----------|------------|
| InfluxDB | admin    | admin@123  |
| Grafana  | admin    | admin@123  |

The MQTT broker (Mosquitto) has no authentication — any ESP32 can publish to it without a username/password.

## Point your ESP32 at it

In the `.ino` sketch, set:

```cpp
const char* mqtt_server = "<IP of the machine running docker compose>";
```

The ESP32 publishes JSON (`{"temperature":...,"humidity":...}`) to `test/publish`. Telegraf is already subscribed to that topic and writes every field into InfluxDB automatically — no code changes needed if you keep that topic name.

## View the dashboard

1. Open `http://<host>:3000` → log in with `admin` / `admin@123`.
2. The **MQTT Telemetry** dashboard is already there on the home page — it's pre-provisioned, no setup needed.

## Ports

| Service   | Port | Purpose              |
|-----------|------|-----------------------|
| mosquitto | 1883 | MQTT broker          |
| influxdb  | 8086 | Time-series storage  |
| grafana   | 3000 | Dashboard UI         |

## Stopping / resetting

```bash
docker compose down        # stop, keep data
docker compose down -v     # stop and wipe all stored data
```
