# Webserver

### Setup

```
docker run -it --rm \
  --volume "$PWD":/www \
soodesune/node-18-vitejs-python add highcharts-react-official highcharts

docker run -it --rm \
  --volume "$PWD":/www \
soodesune/node-18-vitejs-python add --save-dev @types/web-bluetooth
```

### Run the development server

```
docker run -it --rm \
  --publish 3000:5173 \
  --volume "$PWD":/www \
soodesune/node-18-vitejs-python dev --host
```
