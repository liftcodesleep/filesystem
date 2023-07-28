#include "parsePath.h"

int is_file(char *path)
{
  validPath *blob = parse_path(path);
  return blob->parent->isFile;
}

int is_dir(char *path)
{
  validPath *blob = parse_path(path);
  return !blob->parent->isFile;
}