# Webserver

## www Development Server
```
docker run --rm -it \
  --publish 8080:80 \
  --volume "$PWD/www":/opt/bitnami/nginx/html:ro \
  --volume "$PWD/nginx/conf/nginx.conf":/opt/bitnami/nginx/conf/nginx.conf:ro \
  bitnami/nginx:1.19
```


