# MQTT Telemetry Stack

Broker (Mosquitto) + bridge (Telegraf) + storage (InfluxDB) + dashboard (Grafana), wired together with Docker Compose. Mosquitto, Telegraf, and Grafana are published as pre-built images on Docker Hub (`vasulabs/mosquitto-tutorial`, `vasulabs/telegraf-tutorial`, `vasulabs/grafana-tutorial`) with their config and dashboard already baked in — nothing to build, no config folders needed locally.

## Distributing to students

Students only need the single `docker-compose.yml` file — not this whole repo. Give them just that file (e.g. paste it in the manual, or host it as a single download); they save it into an empty folder and run:

```bash
docker compose up -d
```

That's it — no `.env` file, no git, no config folders to download. Compose pulls all four images and starts the stack with fixed, ready-to-use credentials:

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
2. The **MQTT Telemetry** dashboard is already there on the home page — it's pre-provisioned, no setup needed. The InfluxDB datasource behind it is also pre-provisioned if anyone wants to build extra panels via **Explore**.

## Ports

| Service   | Port | Purpose              |
|-----------|------|----------------------|
| mosquitto | 1883 | MQTT broker          |
| influxdb  | 8086 | Time-series storage  |
| grafana   | 3000 | Dashboard UI         |

## Stopping / resetting

```bash
docker compose down        # stop, keep data
docker compose down -v     # stop and wipe all stored data
```

## Maintaining the images (instructor only)

The `mosquitto.conf`, `telegraf.conf`, and `grafana/provisioning/` files in this repo are the *source* for the three custom images — they're baked in at build time, not bind-mounted. If you change any of them, rebuild and push multi-arch (amd64 + arm64, so it works on Intel/AMD and Apple Silicon laptops):

```bash
# one-time setup
docker buildx create --name tutorial-builder --driver docker-container --use
docker run --privileged --rm tonistiigi/binfmt --install all

# rebuild + push after editing a config
cd mosquitto && docker buildx build --platform linux/amd64,linux/arm64 -t vasulabs/mosquitto-tutorial:latest --push . && cd ..
cd telegraf  && docker buildx build --platform linux/amd64,linux/arm64 -t vasulabs/telegraf-tutorial:latest  --push . && cd ..
cd grafana   && docker buildx build --platform linux/amd64,linux/arm64 -t vasulabs/grafana-tutorial:latest   --push . && cd ..
```

Students always get the latest pushed image the next time they run `docker compose up -d` (unless they already have an old one cached — tell them to run `docker compose pull` first if you've pushed an update).
