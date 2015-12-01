//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
long long words, size;
float *M;
char *vocab;

void str_to_vec(char st1[], float *vec, long long *bi);
float distance(float *vec1, float *vec2);

void str_to_vec(char st1[], float *vec, long long *bi) {
  long long a, b, c, cn;
  char st[256][max_size];
  float len;
//  printf("string: %s\n", st1);
  cn = 0;
  b = 0;
  c = 0;
  while (1) {
    st[cn][b] = st1[c];
    b++;
    c++;
    st[cn][b] = 0;
    if (st1[c] == 0) break;
    if (st1[c] == ' ') {
      cn++;
      b = 0;
      c++;
    }
  }
  cn++;
  for (a = 0; a < cn; a++) {
    for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
    if (b == words) b = -1;
    bi[a] = b;
//    printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
    if (b == -1) {
//      printf("Out of dictionary word!\n");
      break;
    }
  }
  
//  if (b == -1) return;
//  printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
  for (a = 0; a < size; a++) vec[a] = 0;
  for (b = 0; b < cn; b++) {
    if (bi[b] == -1) return;
    for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
  }
  
  len = 0;
  for (a = 0; a < size; a++) len += vec[a] * vec[a];
  len = sqrt(len);
  for (a = 0; a < size; a++) vec[a] /= len;
  
  return;
}

float distance(float *vec1, float *vec2) {
  float dist;
  
  dist = 0;
  for (int a = 0; a < size; a++) dist += vec1[a] * vec2[a];
  
  return dist;
}

int main(int argc, char **argv) {
  FILE *f;
  char *bestw[N];
  float dist, len, bestd[N], vec[max_size], ans1[max_size], ans2[max_size], ans3[max_size], ans4[max_size];
  long long a, b, c, d, cn, bi[256], a1bi[256], a2bi[256], a3bi[256], a4bi[256];

  f = fopen("/Users/joshsilverman/Dropbox/Apps/cosi101a/cosi101a/vectors.bin", "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    vocab[b * max_w + a] = 0;
    for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
    len = 0;
    for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
    len = sqrt(len);
    for (a = 0; a < size; a++) M[a + b * size] /= len;
  }
  fclose(f);
  for (a = 0; a < N; a++) bestd[a] = 0;
  for (a = 0; a < N; a++) bestw[a][0] = 0;

  for (int i = 0; i < argc; ++i) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  
  str_to_vec(argv[2], vec, bi);
  
  str_to_vec(argv[3], ans1, a1bi);
  dist = distance(vec, ans1);
  printf("%f", dist);
  
  str_to_vec(argv[4], ans2, a2bi);
  dist = distance(vec, ans2);
  printf(", %f", dist);

  str_to_vec(argv[5], ans3, a3bi);
  dist = distance(vec, ans3);
  printf(", %f", dist);
  
  str_to_vec(argv[6], ans4, a4bi);
  dist = distance(vec, ans4);
  printf(", %f", dist);
}
