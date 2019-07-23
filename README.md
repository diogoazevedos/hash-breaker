# Hash Breaker

### Getting started

To build and run using [Docker](https://docker.com), please use the following commands:

```sh
$ docker image build -t hash-breaker .
$ docker container run -it hash-breaker <hash> <length> <threads>
```

To build and run without Docker:

```sh
$ gcc main.c -o hash-breaker -lcrypto -lssl -lm -fopenmp
$ ./hash-breaker <hash> <length> <threads>
```
