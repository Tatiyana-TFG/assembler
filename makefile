assembler: macro.o macro_helpers.o first_pass.o first_pass_helper.o second_pass.o second_pass_helper.o write_output_file.c classifier.o main.o
	gcc -pedantic -ansi -Wall macro.o macro_helpers.o first_pass.o first_pass_helper.o second_pass.o second_pass_helper.o write_output_file.o classifier.o main.o -o assembler
macro.o: macro.c header_files/datatypes.h header_files/macro_helpers.h header_files/macro.h
	gcc -c -pedantic -ansi -Wall macro.c -o macro.o
macro_helpers.o: macro_helpers.c header_files/macro_helpers.h
	gcc -c -pedantic -ansi -Wall macro_helpers.c -o macro_helpers.o
first_pass.o: first_pass.c header_files/datatypes.h header_files/first_pass_helpers.h header_files/classifier.h
	gcc -c -pedantic -ansi -Wall first_pass.c -o first_pass.o
first_pass_helper.o: first_pass_helper.c header_files/macro_helpers.h header_files/datatypes.h header_files/constants.h header_files/first_pass_helper.h
	gcc -c -pedantic -ansi -Wall first_pass_helper.c -o first_pass_helper.o
second_pass.o: second_pass.c header_files/macro_helpers.h header_files/second_pass.h header_files/constants.h header_files/classifier.h header_files/first_pass_helper.h header_files/second_pass_helper.h
	gcc -c -pedantic -ansi -Wall second_pass.c -o second_pass.o
second_pass_helper.o: second_pass_helper.c header_files/second_pass.h header_files/datatypes.h header_files/constants.h header_files/classifier.h header_files/first_pass_helper.h
	gcc -c -pedantic -ansi -Wall second_pass_helper.c -o second_pass_helper.o
write_output_file.o: write_output_file.c header_files/datatypes.h header_files/constants.h header_files/first_pass_helper.h header_files/second_pass_helper.h header_files/write_output_files.h
	gcc -c -pedantic -ansi -Wall write_output_file.c -o write_output_file.o
classifier.o: classifier.c header_files/first_pass_helper.h header_files/datatypes.h header_files/macro.h
	gcc -c -pedantic -ansi -Wall classifier.c -o classifier.o
main.o: main.c header_files/first_pass_helper.h header_files/second_pass_helper.h header_files/datatypes.h header_files/macro.h header_files/macro_helpers.h header_files/second_pass.h header_files/write_output_file.h
	gcc -c -pedantic -ansi -Wall main.c -o main.o




