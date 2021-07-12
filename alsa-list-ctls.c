
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

	void **hints;
	int err, i;

	if ((err = snd_device_name_hint(-1, "ctl", &hints)) < 0) {
		fprintf(stderr, "Unable to open ALSA config: %s\n", snd_strerror(err));
		return EXIT_FAILURE;
	}
	
	for (i = 0; hints[i]; i++) {
		char *name;
		name = snd_device_name_get_hint(hints[i], "NAME");
		if (name)
			printf("%s\n", name);
		free(name);
	}

	return EXIT_SUCCESS;
}

