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


### Deploy

Login to Firebase
```
docker run -dt \
  --name firebase-client \
  --volume "$PWD":/src \
  --publish 9005:9005 \
  --workdir /src \
  --entrypoint yarn \
soodesune/node-18-vitejs-python firebase login --interactive

docker exec -it firebase-client yarn firebase login
```

Init the project
```
# Do this one time only when setting up the project
docker exec -it firebase-client yarn firebase init

# Default project: thought-timer
# Public directory: public/
# [y] single page app
```

Build the assets
```
docker run -it --rm \
  --volume "$PWD":/www \
soodesune/node-18-vitejs-python build
```

Deploy
```
docker exec -it firebase-client yarn firebase deploy --only hosting
```
