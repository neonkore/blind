/* See LICENSE file for copyright and license details. */
#include "common.h"

USAGE("(file rows) ...")

int
main(int argc, char *argv[])
{
	struct stream *streams;
	size_t parts, height = 0, *rows, i;

	UNOFLAGS(argc % 2 || !argc);

	parts   = (size_t)argc / 2;
	streams = emalloc2(parts, sizeof(*streams));
	rows    = alloca(parts * sizeof(*rows));

	for (i = 0; i < parts; i++) {
		eopen_stream(streams + i, argv[i * 2]);
		rows[i] = etozu_arg("rows", argv[i * 2 + 1], 1, SIZE_MAX);
		if (streams[i].height > SIZE_MAX - height)
			eprintf("output video is too wide\n");
		height += streams[i].height;
		if (i) {
			streams[i].height = streams->height;
			echeck_compat(streams, streams + i);
		}
	}

	streams->height = height;
	fprint_stream_head(stdout, streams);
	efflush(stdout, "<stdout>");

	for (i = 0; i < parts; i++, i = i == parts ? 0 : i)
		if (esend_rows(streams + i, STDOUT_FILENO, rows[i], "<stdout>") != rows[i])
			break;
	for (i = 0; i < parts; i++)
		close(streams[i].fd);

	free(streams);
	return 0;
}
