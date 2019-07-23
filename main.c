#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include <sys/time.h>
#include <openssl/md5.h>

char response[8];
char* secret;
struct timeval start, stop;
uint found = 0, length, threads;

const char CHARSET[] = "@$#?!=+%abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const uint CHARSET_LENGTH = strlen(CHARSET);

void compareMD5(char* input) {
  unsigned char digest[MD5_DIGEST_LENGTH];

  MD5_CTX context;

  MD5_Init(&context);
  MD5_Update(&context, input, strlen(input));
  MD5_Final(digest, &context);

  char output[33];

  for (uint i = 0; i < 16; i++)
    sprintf(&output[i * 2], "%02x", (uint)digest[i]);

  if (strcmp(output, secret) == 0) {
    found = 1;
    strcpy(response, input);
  }
}

void sequential() {
  for (uint i = 0; i < (uint)pow(CHARSET_LENGTH, length); i++) {
    if (found == 1) continue;

    char* input = malloc(length + 1);
    input[length] = '\0';

    for (uint j = 0; j < length; j++) {
      input[j] = CHARSET[(uint)floor(i / pow(CHARSET_LENGTH, j)) % CHARSET_LENGTH];
    }

    compareMD5(input);
    free(input);
  }
}

void parallel() {
  #pragma omp parallel for shared(found) schedule(dynamic)
  for (uint i = 0; i < (uint)pow(CHARSET_LENGTH, length); i++) {
    if (found == 1) continue;

    char* input = malloc(length + 1);
    input[length] = '\0';

    for (uint j = 0; j < length; j++) {
      input[j] = CHARSET[(uint)floor(i / pow(CHARSET_LENGTH, j)) % CHARSET_LENGTH];
    }

    compareMD5(input);
    free(input);
  }
}



int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: %s <hash> <length> <threads>", argv[0]);
    return EXIT_FAILURE;
  }

  secret = argv[1];
  length = strtol(argv[2], NULL, 10);
  threads = strtol(argv[3], NULL, 10);

  gettimeofday(&start, NULL);

  if (threads == 0) {
    printf("Sequential\n");
    sequential();
  } else {
    printf("Parallel with %i threads\n", threads);
    omp_set_num_threads(threads);
    parallel();
  }

  gettimeofday(&stop, NULL);

  uint seconds = stop.tv_sec - start.tv_sec;
  uint milliseconds = (stop.tv_usec - start.tv_usec) >= 0
    ? (stop.tv_usec - start.tv_usec)
    : 1000000 - stop.tv_usec;

  printf("Secret: %s\nElapsed Time: %d.%d\n", response, seconds, milliseconds);

  return EXIT_SUCCESS;
}
