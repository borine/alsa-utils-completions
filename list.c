/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.

 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org>
 */

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <alsa/asoundlib.h>

#include "version.h"


static bool verbose = false;

static void usage(const char *name) {
	printf("Usage: %s [OPTION]...\n", name);
	printf("Options\n");
	printf("  -h, --help       print this help and exit\n"
	       "  -V. --version    print current version and exit\n"
	       "  -v, --verbose    include extra detail\n"
	       "  -c, --card       list connected sound cards\n"
	       "  -H, --hwdep      list configured hw dependent devices\n"
	       "  -m, --mixer      list configured mixers\n"
	       "  -p, --pcm        list configured pcms\n"
	       "  -r, --rawmidi    list configured rawmidi devices\n"
	       "  -s, --seq        list configured pcms\n");
}

struct hint_index {
	char *name;
	unsigned int index;
};

static int hint_cmp(const void *p1, const void *p2) {
	const struct hint_index *hint1 = p1;
	const struct hint_index *hint2 = p2;
	if (hint1->name == NULL && hint2->name == NULL)
		return 0;
	if (hint1->name == NULL)
		return -1;
	if (hint2->name == NULL)
		return 1;
	return strcmp(hint1->name, hint2->name);
}

static int list_hints(int card, const char *iface) {
	void **hints;
	int err;
	size_t i, count = 0;

	if ((err = snd_device_name_hint(card, iface, &hints)) < 0) {
		fprintf(stderr, "Unable to open ALSA config: card:%d iface:%s %s\n", card, iface, snd_strerror(err));
		return -1;
	}
	
	for (i = 0; hints[i]; i++)
		count++;

	if (count == 0)
		return 0;

	struct hint_index *index = malloc(count * sizeof(struct hint_index));

	for (i = 0; i < count; i++) {
		index[i].name = snd_device_name_get_hint(hints[i], "NAME");
		index[i].index = i;
	}
	qsort(index, count, sizeof(struct hint_index), hint_cmp);

	for (i = 0; i < count && index[i].name; i++) {
		printf("%s\n", index[i].name);
		free(index[i].name);
		if (verbose) {
			char *description = snd_device_name_get_hint(hints[index[i].index], "DESC");
			if (description) {
				char *last_line = description;
				char *nl = strchr(description, '\n');
				if (nl) {
					printf("    %.*s\n", (int)(nl - description), description);
					last_line = nl + 1;
				}
				printf("    %s\n", last_line);
				free(description);
			}
		}
	}

	free(index);
	return 0;
}

static int list_cards(void) {
	int card_index = -1;
	int ret;
	snd_ctl_card_info_t *card_info;

	if ((ret = snd_ctl_card_info_malloc(&card_info)) == -1) {
		fprintf(stderr, "Out of memory\n");
		return -1;
	}

	do {
		const char *name;
		const char *longname;
		const char *id;
		const char *driver;
		const  char *mixername;
		const char *components;

		char ctl_name[6];
		snd_ctl_t *ctl;

		if ((ret = snd_card_next(&card_index)) != 0) {
			fprintf(stderr, "Failed to read card index: %s\n", snd_strerror(ret));
			snd_ctl_card_info_free(card_info);
			return -1;
		}
		if (card_index == -1)
			break;
		snprintf(ctl_name, sizeof(ctl_name), "hw:%d", card_index);
		if ((ret = snd_ctl_open(&ctl, ctl_name, 0)) < 0)
			continue;
	
		snd_ctl_card_info(ctl, card_info);

		id = snd_ctl_card_info_get_id(card_info);
		if (verbose) {
			driver = snd_ctl_card_info_get_driver(card_info);
			name = snd_ctl_card_info_get_name(card_info);
			longname = snd_ctl_card_info_get_longname(card_info);
			mixername = snd_ctl_card_info_get_mixername(card_info);
			components = snd_ctl_card_info_get_components(card_info);
			printf("%s\n"
				   "    index: %d\n"
				   "    name: %s\n"
				   "    longname: %s\n"
				   "    driver: %s\n"
				   "    mixername: %s\n"
				   "    components: %s\n",
					id, card_index, driver, name, longname, mixername, components);
		}
		else
			printf("%d %s\n", card_index, id);

		snd_ctl_card_info_clear(card_info);
		snd_ctl_close(ctl);

	} while (card_index != -1);

	snd_ctl_card_info_free(card_info);

	return 0;
}

int main(int argc, char *argv[]) {

	int err;
	bool cards = false;
	bool hwdep = false;
	bool mixers = false;
	bool pcms = false;
	bool rawmidi = false;
	bool seq = false;
	int count = 0;

	int opt;
	const char *opts = "hVvcHmprs";
	const struct option longopts[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'V' },
		{ "verbose", no_argument, NULL, 'v' },
		{ "card", no_argument, NULL, 'c' },
		{ "hwdep", no_argument, NULL, 'H' },
		{ "mixer", no_argument, NULL, 'm' },
		{ "pcm", no_argument, NULL, 'p' },
		{ "rawmidi", no_argument, NULL, 'r' },
		{ "sequencer", no_argument, NULL, 's' },
		{ 0, 0, 0, 0 },
	};

	while ((opt = getopt_long(argc, argv, opts, longopts, NULL)) != -1)
		switch (opt) {
		case 'h' /* --help */ :
			usage(argv[0]);
			return EXIT_SUCCESS;

		case 'V' /* --version */ :
			printf("%s\n", VERSION_ID);
			return EXIT_SUCCESS;

		case 'v' /* --verbose */ :
			verbose = true;
			break;

		case 'c' /* --card */ :
			cards = true;
			++count;
			break;

		case 'H' /* --hwdep */ :
			hwdep = true;
			++count;
			break;

		case 'm' /* --mixer */ :
			mixers = true;
			++count;
			break;

		case 'p' /* --pcm */ :
			pcms = true;
			++count;
			break;

		case 'r' /* --rawmidi */ :
			rawmidi = true;
			++count;
			break;

		case 's' /* --sequencer */ :
			seq = true;
			++count;
			break;

		default:
			fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
			return EXIT_FAILURE;
		}

	err = 0;

	if (!(cards || hwdep || mixers || pcms || rawmidi || seq))
		cards = true;

	if (cards) {
		if (count > 1)
			printf("*** Connected Cards ***\n");
		err += list_cards();
	}

	if (hwdep) {
		if (count > 1)
			printf("*** Configured Hardware Dependent Devices ***\n");
		for (int card = -1; (snd_card_next(&card) == 0) && card >= 0; )
			err += list_hints(card, "hwdep");
	}

	if (mixers) {
		if (count > 1)
			printf("*** Configured Mixers ***\n");
		err += list_hints(-1, "ctl");
	}

	if (pcms) {
		if (count > 1)
			printf("*** Configured PCMs ***\n");
		err += list_hints(-1, "pcm");
	}

	if (rawmidi) {
		if (count > 1)
			printf("*** Configured Rawmidi Devices ***\n");
		err += list_hints(-1, "rawmidi");
	}

	if (seq) {
		if (count > 1)
			printf("*** Configured Sequencers ***\n");
		err += list_hints(-1, "seq");
	}

	return err == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
