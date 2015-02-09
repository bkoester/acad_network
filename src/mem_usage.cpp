#include "mem_usage.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>


int ParseLine(char* line);


int GetMemoryUsage() {
	FILE* file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];


	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmSize:", 7) == 0) {
			result = ParseLine(line);
			break;
		}
	}
	fclose(file);
	return result;
}


int ParseLine(char* line) {
	int i = strlen(line);
	while (*line < '0' || *line > '9') line++;
	line[i-3] = '\0';
	i = atoi(line);
	return i;
}
