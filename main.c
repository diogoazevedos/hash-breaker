#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <openssl/md5.h>

char response[8];
char* secret;
clock_t begin;
float elapsed_time;
int found = 0, length, threads;

const char CHARSET[] = "@$#?!=+%abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int CHARSET_LENGTH = strlen(CHARSET);

void compareMD5(char* input) {
    unsigned char digest[MD5_DIGEST_LENGTH];

    MD5_CTX context;

    MD5_Init(&context);
    MD5_Update(&context, input, strlen(input));
    MD5_Final(digest, &context);

    char output[33];

    for (int i = 0; i < 16; i++)
      sprintf(&output[i * 2], "%02x", (unsigned int)digest[i]);

    if (strcmp(output, secret) == 0) {
      found = 1;
      elapsed_time = (double)(clock() - begin) / CLOCKS_PER_SEC;
      strcpy(response, input);
    }
}

void sequential() {
  for (int i = 0; i < (unsigned int)pow(CHARSET_LENGTH, length); i++) {
    if (found == 1) continue;

    char* input = malloc(length + 1);
    input[length] = '\0';

    for (int j = 0; j < length; j++) {
      input[j] = CHARSET[(int)floor(i / pow(CHARSET_LENGTH, j)) % CHARSET_LENGTH];
    }

    compareMD5(input);
    free(input);
  }
}

void parallel() {
  #pragma omp parallel for shared(found) schedule(dynamic)
  for (int i = 0; i < (unsigned int)pow(CHARSET_LENGTH, length); i++) {
    if (found == 1) continue;

    char* input = malloc(length + 1);
    input[length] = '\0';

    for (int j = 0; j < length; j++) {
      input[j] = CHARSET[(int)floor(i / pow(CHARSET_LENGTH, j)) % CHARSET_LENGTH];
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

  begin = clock();
  secret = argv[1];
  length = strtol(argv[2], NULL, 10);
  threads = strtol(argv[3], NULL, 10);

  if (threads == 0) {
    printf("Sequential\n");
    sequential();
  } else {
    printf("Parallel with %i threads\n", threads);
    omp_set_num_threads(threads);
    parallel();
  }

  printf("Secret: %s\nElapsed Time: %f\n", response, elapsed_time);

  return EXIT_SUCCESS;
}
