#ifndef CLASSIFIER_H
#define CLASSIFIER_H

int valid_label(char *name, Macro *mcro);
line_type classify_line(char *line);
directive_type classift_dir(char *line);
int is_label (char **line, char *name, Macro *mcro);

#endif