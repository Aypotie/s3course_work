FROM ubuntu:latest


RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libpqxx-dev \
    libpq-dev \
    libasio-dev \
    libssl-dev \
    libkrb5-dev \
    libgssapi-krb5-2 


WORKDIR /app

COPY ./lib /app/lib
COPY ./src /app/src
COPY ./http_server.cpp /app/http_server.cpp

RUN g++ http_server.cpp -lpqxx -lpq -std=c++17 -o /app/http_server

COPY ./html /app/html
COPY ./static /app/static
COPY ./sql_query.sql /app/sql_query.sql
COPY ./.env_docker /app/.env

EXPOSE 8000

ENTRYPOINT ["./http_server"]
