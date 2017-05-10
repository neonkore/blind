/* See LICENSE file for copyright and license details. */
#include "stream.h"
#include "util.h"

#include <string.h>

USAGE("[-f frames] width height pixel-format ...")

int
main(int argc, char *argv[])
{
	struct stream stream;
	size_t n;
	int i;
	char *p;

	stream.frames = 1;

	ARGBEGIN {
	case 'f':
		stream.frames = entozu_flag(2, 'f', UARGF(), 1, SIZE_MAX);
		break;
	default:
		usage();
	} ARGEND;

	if (argc < 3)
		usage();

	stream.fd = STDIN_FILENO;
	stream.file = "<stdin>";
	stream.pixfmt[0] = '\0';

	stream.width  = entozu_arg(2, "the width",  argv[0], 1, SIZE_MAX);
	stream.height = entozu_arg(2, "the height", argv[1], 1, SIZE_MAX);
	argv += 2, argc -= 2;

	n = (size_t)argc - 1;
	for (i = 0; i < argc; i++)
		n += strlen(argv[i]);
	if (n < sizeof(stream.pixfmt)) {
		p = stpcpy(stream.pixfmt, argv[0]);
		for (i = 1; i < argc; i++) {
			*p++ = ' ';
			p = stpcpy(p, argv[i]);
		}
	}

	enset_pixel_size(2, &stream);

	fprint_stream_head(stdout, &stream);
	enfflush(2, stdout, "<stdout>");

	n = ensend_frames(2, &stream, STDOUT_FILENO, stream.frames, "<stdout>");
	if (!n)
		return 1;
	else if (n < stream.frames)
		enprintf(2, "%s: is shorter than expected\n", stream.file);
	return 0;
}
