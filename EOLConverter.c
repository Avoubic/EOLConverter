//
// EOLConverter.c
// 2022.02.14 Avoubic
//

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printVersion() {
	printf("EOLConverter 1.1.0\n");
	exit(0);
}
void printHelp() {
	printf("Usage: EOLConverter [options] file...\n");
	printf("Options:\n");
	printf("  -c <to>\tConvert to:\n");
	printf("    toCR\tMac end line\n");
	printf("    toLF\tUnix end line\n");
	printf("    toCRLF\tWindows end line\n");
	printf("  -h\t\tDisplay this information.\n");
	printf("  -i\t\tDisplay file information.\n");
	printf("  -v\t\tDisplay version information.\n");
	printVersion();
}

int main(int argc, char* const argv[]) {

	int  FileName    = -1;
	int  toEndLine   = -1;
	bool Change      = false;
	bool ShowHelp    = false;
	bool ShowInfo    = false;
	bool ShowVersion = false;
	bool CR = false, LF = false, CRLF = false;

	int rez = 0;

//	opterr = 0;
	while ( (rez = getopt(argc, argv, "-:c:hiv")) != -1){
		switch (rez) {
			case 'c': toEndLine = optind -1; break;
			case 'h': ShowHelp = true; break;
			case 'i': ShowInfo = true; break;
			case 'v': ShowVersion = true; break;
			case ':': printf("option needs a value\n"); break;
			// case '?': printf ("Unknown option character `\\x%x'.\n", optopt); break;
			case 1: if (FileName < 0) FileName = optind -1;	break;
		}
	}
	if (ShowHelp) printHelp();
	if (ShowVersion) printVersion();

	if (FileName < 0) {
		printf ("No filename\n");
		printHelp();
//		exit(1);
	}

	int i = 0, c;
	FILE *in  = fopen(argv[FileName],"rb" );
	if (in == NULL) {
		perror (argv[FileName]);
		exit(2);
	}

// Провека символа конца строки
	while ((c = fgetc(in)) != EOF) {
		if (c == 13 && LF == false)
			CR = true;
		if (c == 10) {
			LF = true;
			if (CR == true){
				CR   = false;
				LF   = false;
				CRLF = true;
			}
		}
	}

	if ( CR && !LF && !CRLF) printf("File Mac end line (CR)\n");
	if (!CR &&  LF && !CRLF) printf("File Unix end line (LF)\n");
	if (!CR && !LF &&  CRLF) printf("File Windows end line (CRLF)\n");

	if (ShowInfo) {
		fclose(in);
		exit(0);
	}

	// Сброс на начало фала
	fseek(in, SEEK_SET, 0);
// ============================

	if (toEndLine < 0) {
		printf ("toEndLine indefined\n");
		printHelp();
//		exit(1);
	}

	// Приведение к нижнему регистру
	for (size_t i = 0; i < strlen(argv[toEndLine]); ++i) {
        argv[toEndLine][i] = tolower(argv[toEndLine][i]);
    }

	FILE *out = fopen(argv[FileName],"rb+");

	if (in == NULL || out == NULL) {
		perror (argv[FileName]);
		exit(2);
	}

//	rewind(out);

// Выполнение замены конца строк
	if (strncmp(argv[toEndLine], "tocr", 4) == 0 && strncmp(argv[toEndLine], "tocrlf", 6) != 0) {
		if (CR) {
			//printf("File is CR Charset\n");
		}
		else if (LF) {
			while ((c = fgetc(in)) != EOF) {
				if (c != 10) {
					fputc(c, out);
				}
				else {
					fputc(13, out);
				}
				i++;
			}
			Change = true;
		}
		else if (CRLF) {
			while ((c = fgetc(in)) != EOF) {
				if (c != 10) {
					i++;
					fputc(c, out);
				}
			}
			Change = true;
		}
		else {
			printf("No 'newline'  in text\n");
		}
	}
	else if (strncmp(argv[toEndLine], "tolf", 4) == 0) {
		if (CR) {
			while ((c = fgetc(in)) != EOF) {
				if (c != 13) {
					fputc(c, out);
				}
				else {
					fputc(10, out);
				}
				i++;
			}
			Change = true;
		}
		else if (LF) {
			//printf("File is LF Charset\n");
		}
		else if (CRLF) {
			while ((c = fgetc(in)) != EOF) {
				if (c != 13) {
					i++;
					fputc(c, out);
				}
			}
			Change = true;
		}
		else {
			printf("No 'newline'  in text\n");
		}
	}
	else if (strncmp(argv[toEndLine], "tocrlf", 6) == 0) {
		if (CR) {
			while ((c = fgetc(in)) != EOF) {
				if (c != 13) {
					fputc(c, out);
				}
				else {
					fputc(13, out);
					fputc(10, out);
					i++;
				}
				i++;
			}
			Change = true;
		}
		else if (LF) {
			while ((c = fgetc(in)) != EOF) {
				if (c != 10) {
					fputc(c, out);
				}
				else {
					fputc(13, out);
					fputc(10, out);
					i++;
				}
				i++;
			}
			Change = true;
		}
		else if (CRLF) {
			//printf("File is CRLF Charset\n");
		}
		else {
			printf("No 'newline'  in text\n");
		}
	}
	else {
		for (size_t i = 0; i <= strlen(argv[toEndLine]); ++i) {
	        printf("%d ", argv[toEndLine][i]);
	    }
	    printf("\n");
		printf("Error toEndLine\n");
	}
//=============================

	fclose(in);
	// Подрезка файла
	if (Change) {
		printf("Change %s is OK\n", argv[toEndLine]);
		ftruncate(fileno(out), i);
		printf("Number of character: %d\n", i);
	}

	fclose(out);
	return 0;
}


/*
+ Из CRLF -> LF
+ Из CRLF -> CR
+ Из CR   -> CRLF
+ Из LF   -> CRLF
+ Из CR   -> LF
+ Из LF   -> CR
*/