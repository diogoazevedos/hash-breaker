FROM ubuntu:latest

RUN apt-get update && apt-get -y install build-essential libssl-dev

COPY main.c main.c

RUN gcc main.c -o entrypoint -lcrypto -lssl -lm -fopenmp

ENTRYPOINT ["./entrypoint"]
