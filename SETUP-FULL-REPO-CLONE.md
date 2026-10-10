# MQTT Telemetry Stack — Full Repo Setup (Git Clone)

For instructors, TAs, or anyone who wants the full source: the Dockerfiles behind the custom images, the raw Mosquitto/Telegraf/Grafana configs, the manual MQTT test script, and the ESP32 test sketch.

## 1. Prerequisites

- Git
- Docker Desktop (Windows/Mac) or Docker Engine (Linux)

## 2. Clone

```bash
git clone git@github.com:BooleanTron/mqtt-tutorial.git
cd mqtt-tutorial/docker
```

(Use `https://github.com/BooleanTron/mqtt-tutorial.git` instead if you haven't got SSH keys set up with GitHub.)

## 3. Start the stack

```bash
docker compose up -d
```

This pulls the same four images as the student-facing setup (`vasulabs/mosquitto-tutorial`, `influxdb:2`, `vasulabs/telegraf-tutorial`, `vasulabs/grafana-tutorial`) — the difference is you now also have the source files on disk to inspect or modify.

## What's in the repo

```
docker/
  docker-compose.yml              # the stack definition
  mosquitto/
    Dockerfile                    # builds vasulabs/mosquitto-tutorial
    config/mosquitto.conf         # no-auth broker config (source of truth)
  telegraf/
    Dockerfile                    # builds vasulabs/telegraf-tutorial
    telegraf.conf                 # MQTT -> InfluxDB bridge config
  grafana/
    Dockerfile                    # builds vasulabs/grafana-tutorial
    provisioning/                 # datasource + the premade dashboard
  publish_test.sh                 # manual MQTT publish script, for testing the broker without an ESP32
mqtt_on_ESP32_enterprise_wifi_and_pubSubClient.ino   # ESP32 test sketch
```

## Credentials

| Service  | Username | Password   |
|----------|----------|------------|
| InfluxDB | admin    | admin@123  |
| Grafana  | admin    | admin@123  |

Mosquitto has no authentication.

## Maintaining the images

The config files under `mosquitto/`, `telegraf/`, and `grafana/provisioning/` are baked into the custom images at build time, not bind-mounted at runtime. If you edit any of them, rebuild and push multi-arch (amd64 + arm64, so it works on both Intel/AMD and Apple Silicon laptops):

```bash
# one-time setup
docker buildx create --name tutorial-builder --driver docker-container --use
docker run --privileged --rm tonistiigi/binfmt --install all

# rebuild + push after editing a config
cd mosquitto && docker buildx build --platform linux/amd64,linux/arm64 -t vasulabs/mosquitto-tutorial:latest --push . && cd ..
cd telegraf  && docker buildx build --platform linux/amd64,linux/arm64 -t vasulabs/telegraf-tutorial:latest  --push . && cd ..
cd grafana   && docker buildx build --platform linux/amd64,linux/arm64 -t vasulabs/grafana-tutorial:latest   --push . && cd ..
```

Students always get the latest pushed image the next time they run `docker compose up -d` — unless they already pulled an older one, in which case tell them to run `docker compose pull` first.

## Stopping / resetting

```bash
docker compose down        # stop, keep data
docker compose down -v     # stop and wipe all stored data
```
