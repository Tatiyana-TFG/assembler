#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include <datatypes.h>

int valid_label(char *name, Macro *mcro);
line_type classify_line(char *line);
directive_type classify_dir(char *line);
int is_label (char **line, char *name, Macro *mcro);

#endif