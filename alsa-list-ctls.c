
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CURRENT_VERSION "0.2.0"

void usage(const char *name) {
	printf("Usage: %s [OPTION]...\n", name);
	printf("\n");
	printf("-h, --help       print this help\n");
	printf("-v. --version    print current version\n");
	printf("-V, --verbose    include CTL description\n");
}

int main(int argc, char *argv[]) {

	void **hints;
	int err, i;
	bool verbose = false;

	if (argc > 2) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if (argc == 2) {
		if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
			usage(argv[0]);
			return EXIT_SUCCESS;
		}
		if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[1], "--version") == 0)) {
			printf("%s: version %s\n", argv[0], CURRENT_VERSION);
			return EXIT_SUCCESS;
		}
		if ((strcmp(argv[1], "-V") == 0) || (strcmp(argv[1], "--verbose") == 0))
			verbose = true;
	}

	if ((err = snd_device_name_hint(-1, "ctl", &hints)) < 0) {
		fprintf(stderr, "Unable to open ALSA config: %s\n", snd_strerror(err));
		return EXIT_FAILURE;
	}
	
	for (i = 0; hints[i]; i++) {
		char *value;
		value = snd_device_name_get_hint(hints[i], "NAME");
		if (value) {
			printf("%s\n", value);
			free(value);
			if (verbose) {
				value = snd_device_name_get_hint(hints[i], "DESC");
				if (value) {
					char *last_line = value;
					char *nl = strchr(value, '\n');
					if (nl) {
						printf("    %.*s\n", (int)(nl - value), value);
						last_line = nl + 1;
					}
					printf("    %s\n", last_line);
					free(value);
				}
			}
		}
	}

	return EXIT_SUCCESS;
}

