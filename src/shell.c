#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

const int LINE_MAX = 256;

void process_line(char line[LINE_MAX + 1]) {
   if (!strncmp(line, "ls", LINE_MAX)) {
      fprintf(stdout, "your files here\n");

   } else {
      fprintf(stdout, "%s\n", line);
   }
}

int main(int argc, char *argv[]) {
   char line[LINE_MAX + 1];
   int c, index;

   index = 0;
   c = fgetc(stdin);
   while (c > 0 && c != '\n' && index < LINE_MAX) {
      line[index++] = c;
      c = fgetc(stdin);
   }
   line[index] = '\0';

   if (ferror(stdin)) {
      fprintf(stderr, "error while reading line: %s\n", strerror(errno));
      return 1;

   } else {
      process_line(line);
      return 0;
   }
}
