/*
 * leader-launcher
 * ---------------
 *  Author: Mark Ulrich
 *  Created: Sun 09 Feb 2020 12:56:31 PM CST
 *
 *  A simple, fast, lightweight launcher taking inspiration from VIM's "leader
 *  key" concept.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_MAP_SEQ_LEN  32  
#define MAX_MAP_DESC_LEN 256  
#define MAX_MAP_CMD_LEN  256  

typedef struct map_s {
  char seq[MAX_MAP_SEQ_LEN];
  char desc[MAX_MAP_DESC_LEN];
  char cmd[MAX_MAP_CMD_LEN];

  struct map_s* next;
} map_t;

static const char* app_name;
map_t test_mapping[] = {
  { "term",   "Launch a terminal",      "st" },
  { "br",     "Launch a web browser",   "chromium" }
};


void usage();
void exec_mapping(map_t* map);
map_t* add_mapping(map_t** map, map_t* new_mapping);

map_t* add_mapping(map_t** map, map_t* new_mapping)
{
  if (!*map) {
    *map = new_mapping;
    return new_mapping;
  }

  map_t* node = *map;
  while (node->next) ++node;
  node->next = new_mapping;
  return new_mapping;
}


void usage() { printf("%s options\n", app_name); }


void exec_mapping(map_t* map)
{
  /*extern char** environ;*/
  const char* args = map->cmd;
  int ret = 0;
  int pid = -1;

  /*for (int i = 0; environ[i]; ++i) {*/
    /*printf("%s\n", environ[i]);*/
  /*}*/

  /*printf("%s\n", getenv("PATH"));*/

  pid = fork();
  if (pid == -1) {
    perror("fork");
  }
  else if (pid == 0) {
    ret = execlp(map->cmd, args, NULL);
    if (ret) {
      perror("execve");
    }
  } 
}

int main(int argc, char* argv[]) {
  app_name = argv[0];
  if (argc < 1) {
    usage();
    exit(0);
  }

  map_t* map = NULL;
  for (int i = 0; i < sizeof(test_mapping)/sizeof(map_t); ++i)
    add_mapping(&map, &test_mapping[i]);

  char buf[MAX_MAP_SEQ_LEN];
  fgets(buf, MAX_MAP_SEQ_LEN, stdin);
  buf[strcspn(buf, "\n")] = '\0';

  map_t* cur = map;
  while (cur) {
    if (!strncmp(buf, cur->seq, MAX_MAP_SEQ_LEN)) {
      exec_mapping(cur);
      break;
    }
    cur = cur->next;
  }
  if (!cur) {
    /* TODO: Attempt to run command in `buf` if it exists in `PATH`? */
    fprintf(stderr, "Unknown mapping: %s\n", buf);
  }

  return 0;
}
