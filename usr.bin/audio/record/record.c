/*	$NetBSD: record.c,v 1.18 2002/01/15 08:59:21 mrg Exp $	*/

/*
 * Copyright (c) 1999 Matthew R. Green
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * SunOS compatible audiorecord(1)
 */

#include <sys/types.h>
#include <sys/audioio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/uio.h>

#include <err.h>
#include <fcntl.h>
#include <paths.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libaudio.h"

audio_info_t info, oinfo;
ssize_t	total_size = -1;
char	*device;
char	*ctldev;
int	format = AUDIO_FORMAT_SUN;
char	*header_info;
char	default_info[8] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
int	audiofd, ctlfd, outfd;
int	qflag, aflag, fflag;
int	verbose;
int	monitor_gain, omonitor_gain;
int	gain;
int	balance;
int	port;
int	encoding;
char	*encoding_str;
int	precision;
int	sample_rate;
int	channels;
struct timeval record_time;
struct timeval start_time;	/* XXX because that's what gettimeofday returns */

void (*conv_func) (char *, size_t);
void swap16 (char *, size_t);
void swap32 (char *, size_t);

void usage (void);
int main (int, char *[]);
int timeleft (struct timeval *, struct timeval *);
void cleanup (int) __attribute__((__noreturn__));
int write_header_sun (void **, size_t *, int *);
int write_header_wav (void **, size_t *, int *);
void write_header (void);
void rewrite_header (void);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	char	*buffer;
	size_t	len, bufsize;
	int	ch, no_time_limit = 1;

	while ((ch = getopt(argc, argv, "ab:C:F:c:d:e:fhi:m:P:p:qt:s:Vv:")) != -1) {
		switch (ch) {
		case 'a':
			aflag++;
			break;
		case 'b':
			decode_int(optarg, &balance);
			if (balance < 0 || balance > 63)
				errx(1, "balance must be between 0 and 63\n");
			break;
		case 'C':
			ctldev = optarg;
			break;
		case 'F':
			format = audio_format_from_str(optarg);
			if (format < 0)
				errx(1, "Unknown audio format; "
				    "supported formats: \"sun\" and \"wav\"");
			break;
		case 'c':
			decode_int(optarg, &channels);
			if (channels < 0 || channels > 16)
				errx(1, "channels must be between 0 and 16\n");
			break;
		case 'd':
			device = optarg;
			break;
		case 'e':
			encoding_str = optarg;
			break;
		case 'f':
			fflag++;
			break;
		case 'i':
			header_info = optarg;
			break;
		case 'm':
			decode_int(optarg, &monitor_gain);
			if (monitor_gain < 0 || monitor_gain > 255)
				errx(1, "monitor volume must be between 0 and 255\n");
			break;
		case 'P':
			decode_int(optarg, &precision);
			if (precision != 4 && precision != 8 &&
			    precision != 16 && precision != 24 &&
			    precision != 32)
				errx(1, "precision must be between 4, 8, 16, 24 or 32");
			break;
		case 'p':
			len = strlen(optarg);

			if (strncmp(optarg, "mic", len) == 0)
				port |= AUDIO_MICROPHONE;
			else if (strncmp(optarg, "cd", len) == 0 ||
			           strncmp(optarg, "internal-cd", len) == 0)
				port |= AUDIO_CD;
			else if (strncmp(optarg, "line", len) == 0)
				port |= AUDIO_LINE_IN;
			else
				errx(1,
			    "port must be `cd', `internal-cd', `mic', or `line'");
			break;
		case 'q':
			qflag++;
			break;
		case 's':
			decode_int(optarg, &sample_rate);
			if (sample_rate < 0 || sample_rate > 48000 * 2)	/* XXX */
				errx(1, "sample rate must be between 0 and 96000\n");
			break;
		case 't':
			no_time_limit = 0;
			decode_time(optarg, &record_time);
			break;
		case 'V':
			verbose++;
			break;
		case 'v':
			decode_int(optarg, &gain);
			if (gain < 0 || gain > 255)
				errx(1, "volume must be between 0 and 255\n");
			break;
		/* case 'h': */
		default:
			usage();
			/* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;

	/*
	 * open the audio device, and control device
	 */
	if (device == NULL && (device = getenv("AUDIODEVICE")) == NULL &&
	    (device = getenv("AUDIODEV")) == NULL) /* Sun compatibility */
		device = _PATH_AUDIO;
	if (ctldev == NULL && (ctldev = getenv("AUDIOCTLDEVICE")) == NULL)
		ctldev = _PATH_AUDIOCTL;

	audiofd = open(device, O_RDONLY);
	if (audiofd < 0)
		err(1, "failed to open %s", device);
	ctlfd = open(ctldev, O_RDWR);
	if (ctlfd < 0)
		err(1, "failed to open %s", ctldev);

	/*
	 * work out the buffer size to use, and allocate it.  also work out
	 * what the old monitor gain value is, so that we can reset it later.
	 */
	if (ioctl(ctlfd, AUDIO_GETINFO, &oinfo) < 0)
		err(1, "failed to get audio info");
	bufsize = oinfo.record.buffer_size;
	if (bufsize < 32 * 1024)
		bufsize = 32 * 1024;
	omonitor_gain = oinfo.monitor_gain;

	buffer = malloc(bufsize);
	if (buffer == NULL)
		err(1, "couldn't malloc buffer of %d size", (int)bufsize);

	/*
	 * open the output file
	 */
	if (argc != 1)
		usage();
	if (argv[0][0] != '-' && argv[0][1] != '\0') {
		outfd = open(*argv, O_CREAT|(aflag ? O_APPEND : O_TRUNC)|O_WRONLY, 0666);
		if (outfd < 0)
			err(1, "could not open %s", *argv);
	} else
		outfd = STDOUT_FILENO;

	/*
	 * convert the encoding string into a value.
	 */
	if (encoding_str) {
		encoding = audio_enc_to_val(encoding_str);
		if (encoding == -1)
			errx(1, "unknown encoding, bailing...");
	}
	else
		encoding = AUDIO_ENCODING_ULAW;

	/*
	 * set up audio device for recording with the speified parameters
	 */
	AUDIO_INITINFO(&info);

	/*
	 * for these, get the current values for stuffing into the header
	 */
#define SETINFO(x)	if (x) info.record.x = x; else x = oinfo.record.x
	SETINFO (sample_rate);
	SETINFO (channels);
	SETINFO (precision);
	SETINFO (encoding);
	SETINFO (gain);
	SETINFO (port);
	SETINFO (balance);
#undef SETINFO

	if (monitor_gain)
		info.monitor_gain = monitor_gain;
	else
		monitor_gain = oinfo.monitor_gain;

	info.mode = AUMODE_RECORD;
	if (ioctl(ctlfd, AUDIO_SETINFO, &info) < 0)
		err(1, "failed to reset audio info");

	signal(SIGINT, cleanup);
	write_header();
	total_size = 0;

	(void)gettimeofday(&start_time, NULL);
	while (no_time_limit || timeleft(&start_time, &record_time)) {
		if (read(audiofd, buffer, bufsize) != bufsize)
			err(1, "read failed");
		if (conv_func)
			conv_func(buffer, bufsize);
		if (write(outfd, buffer, bufsize) != bufsize)
			err(1, "write failed");
		total_size += bufsize;
	}
	cleanup(0);
}

int
timeleft(start_tvp, record_tvp)
	struct timeval *start_tvp;
	struct timeval *record_tvp;
{
	struct timeval now, diff;

	(void)gettimeofday(&now, NULL);
	timersub(&now, start_tvp, &diff);
	timersub(record_tvp, &diff, &now);

	return (now.tv_sec > 0 || (now.tv_sec == 0 && now.tv_usec > 0));
}

void
cleanup(signo)
	int signo;
{

	close(audiofd);
	rewrite_header();
	close(outfd);
	if (omonitor_gain) {
		AUDIO_INITINFO(&info);
		info.monitor_gain = omonitor_gain;
		if (ioctl(ctlfd, AUDIO_SETINFO, &info) < 0)
			err(1, "failed to reset audio info");
	}
	close(ctlfd);
	exit(0);
}

int
write_header_sun(hdrp, lenp, leftp)
	void **hdrp;
	size_t *lenp;
	int *leftp;
{
	static int warned = 0;
	static sun_audioheader auh;
	int sunenc;

	/* if we can't express this as a Sun header, don't write any */
	if (audio_encoding_to_sun(encoding, precision, &sunenc) != 0) {
		if (!qflag && !warned)
			warnx("failed to convert to sun encoding from %d:%d; "
			      "Sun audio header not written",
			      encoding, precision);
		warned = 1;
		return -1;
	}

	auh.magic = htonl(AUDIO_FILE_MAGIC);
	if (outfd == STDOUT_FILENO)
		auh.data_size = htonl(AUDIO_UNKNOWN_SIZE);
	else
		auh.data_size = htonl(total_size);
	auh.encoding = htonl(sunenc);
	auh.sample_rate = htonl(sample_rate);
	auh.channels = htonl(channels);
	if (header_info) {
		int 	len, infolen;

		infolen = ((len = strlen(header_info)) + 7) & 0xfffffff8;
		*leftp = infolen - len;
		auh.hdr_size = htonl(sizeof(auh) + infolen);
	} else {
		*leftp = sizeof(default_info);
		auh.hdr_size = htonl(sizeof(auh) + *leftp);
	}
	*(sun_audioheader **)hdrp = &auh;
	*lenp = sizeof auh;
	return 0;
}

void
swap16(buf, len)
	char *buf;
	size_t len;
{
	char *p, t;

	for (p = buf; p < buf + len + 1; p += 2) {
		t = p[0];
		p[0] = p[1];
		p[1] = t;
	}
}

void
swap32(buf, len)
	char *buf;
	size_t len;
{
	char *p, t;

	for (p = buf; p < buf + len + 3; p += 4) {
		t = p[0];
		p[0] = p[3];
		p[3] = t;
		t = p[1];
		p[1] = p[2];
		p[2] = t;
	}
}

int
write_header_wav(hdrp, lenp, leftp)
	void **hdrp;
	size_t *lenp;
	int *leftp;
{
	/*
	 * WAV header we write looks like this:
	 *
	 *      bytes   purpose
	 *      0-3     "RIFF"
	 *      4-7     file length (minus 8)
	 *      8-15    "WAVEfmt "
	 *      16-19   format size
	 *      20-21   format tag
	 *      22-23   number of channels
	 *      24-27   sample rate
	 *      28-31   average bytes per second
	 *      32-33   block alignment
	 *      34-35   bits per sample
	 *
	 * then for ULAW and ALAW outputs, we have an extended chunk size
	 * and a WAV "fact" to add:
	 *
	 *      36-37   length of extension (== 0)
	 *      38-41   "fact"
	 *      42-45   fact size
	 *      46-49   number of samples written
	 *      50-53   "data"
	 *      54-57   data length
	 *      58-     raw audio data
	 *
	 * for PCM outputs we have just the data remaining:
	 *
	 *      36-39   "data"
	 *      40-43   data length
	 *      44-     raw audio data
	 *
	 *	RIFF\^@^C^@WAVEfmt ^P^@^@^@^A^@^B^@D<AC>^@^@^P<B1>^B^@^D^@^P^@data^@^@^C^@^@^@^@^@^@^@^@^@^@
	 */
	char	wavheaderbuf[64], *p = wavheaderbuf;
	char	*riff = "RIFF", *wavefmt = "WAVEfmt ", *fact = "fact", *data = "data";
	u_int32_t filelen, fmtsz, sps, abps, factsz = 4, nsample, datalen;
	u_int16_t fmttag, nchan, align, bps, extln = 0;

	if (header_info)
		warnx("header information not supported for WAV");
	*leftp = NULL;

	switch (precision) {
	case 8:
		bps = 8;
		break;
	case 16:
		bps = 16;
		break;
	case 32:
		bps = 32;
		break;
	default:
		{
			static int warned = 0;

			if (warned == 0) {
				warnx("can not support precision of %d\n", precision);
				warned = 1;
			}
		}
		return (-1);
	}

	switch (encoding) {
	case AUDIO_ENCODING_ULAW:
		fmttag = WAVE_FORMAT_MULAW;
		fmtsz = 18;
		align = channels;
		break;
	case AUDIO_ENCODING_ALAW:
		fmttag = WAVE_FORMAT_ALAW;
		fmtsz = 18;
		align = channels;
		break;
	/*
	 * we could try to support RIFX but it seems to be more portable
	 * to output little-endian data for WAV files.
	 *
	 * XXX signed vs unsigned matters probably!  cope!
	 */
	case AUDIO_ENCODING_ULINEAR_BE:
	case AUDIO_ENCODING_SLINEAR_BE:
		if (bps == 16)
			conv_func = swap16;
		else if (bps == 32)
			conv_func = swap32;
		/* FALLTHROUGH */
	case AUDIO_ENCODING_PCM16:
	case AUDIO_ENCODING_ULINEAR_LE:
	case AUDIO_ENCODING_SLINEAR_LE:
		fmttag = WAVE_FORMAT_PCM;
		fmtsz = 16;
		align = channels * (bps / 8);
		break;
	default:
		{
			static int warned = 0;

			if (warned == 0) {
				warnx("can not support encoding of %s\n", wav_enc_from_val(encoding));
				warned = 1;
			}
		}
		return (-1);
	}

	nchan = channels;
	sps = sample_rate;

	/* data length */
	if (outfd == STDOUT_FILENO)
		datalen = 0;
	else
		datalen = total_size;

	/* file length */
	filelen = 4 + (8 + fmtsz) + (8 + datalen);
	if (fmttag != WAVE_FORMAT_PCM)
		filelen += 8 + factsz;

	abps = (double)align*sample_rate / (double)1 + 0.5;

	nsample = (datalen / bps) / sample_rate;
	
	/*
	 * now we've calculated the info, write it out!
	 */
#define put32(x) do { \
	u_int32_t _f; \
	putle32(_f, (x)); \
	memcpy(p, &_f, 4); \
} while (0)
#define put16(x) do { \
	u_int16_t _f; \
	putle16(_f, (x)); \
	memcpy(p, &_f, 2); \
} while (0)
	memcpy(p, riff, 4);
	p += 4;				/* 4 */
	put32(filelen);
	p += 4;				/* 8 */
	memcpy(p, wavefmt, 8);
	p += 8;				/* 16 */
	put32(fmtsz);
	p += 4;				/* 20 */
	put16(fmttag);
	p += 2;				/* 22 */
	put16(nchan);
	p += 2;				/* 24 */
	put32(sps);
	p += 4;				/* 28 */
	put32(abps);
	p += 4;				/* 32 */
	put16(align);
	p += 2;				/* 34 */
	put16(bps);
	p += 2;				/* 36 */
	/* NON PCM formats have an extended chunk; write it */
	if (fmttag != WAVE_FORMAT_PCM) {
		put16(extln);
		p += 2;			/* 38 */
		memcpy(p, fact, 4);
		p += 4;			/* 42 */
		put32(factsz);
		p += 4;			/* 46 */
		put32(nsample);
		p += 4;			/* 50 */
	}
	memcpy(p, data, 4);
	p += 4;				/* 40/54 */
	put32(datalen);
	p += 4;				/* 44/58 */
#undef put32
#undef put16

	*hdrp = wavheaderbuf;
	*lenp = (p - wavheaderbuf);

	return 0;
}

void
write_header()
{
	struct iovec iv[3];
	int veclen, left, tlen;
	void *hdr;
	size_t hdrlen;

	switch (format) {
	case AUDIO_FORMAT_SUN:
		if (write_header_sun(&hdr, &hdrlen, &left) != 0)
			return;
		break;
	case AUDIO_FORMAT_WAV:
		if (write_header_wav(&hdr, &hdrlen, &left) != 0)
			return;
		break;
	case AUDIO_FORMAT_NONE:
		return;
	default:
		errx(1, "unknown audio format");
	}

	veclen = 0;
	tlen = 0;
		
	if (hdrlen != 0) {
		iv[veclen].iov_base = hdr;
		iv[veclen].iov_len = hdrlen;
		tlen += iv[veclen++].iov_len;
	}
	if (header_info) {
		iv[veclen].iov_base = header_info;
		iv[veclen].iov_len = (int)strlen(header_info) + 1;
		tlen += iv[veclen++].iov_len;
	}
	if (left) {
		iv[veclen].iov_base = default_info;
		iv[veclen].iov_len = left;
		tlen += iv[veclen++].iov_len;
	}

	if (tlen == 0)
		return;

	if (writev(outfd, iv, veclen) != tlen)
		err(1, "could not write audio header");
}

void
rewrite_header()
{

	/* can't do this here! */
	if (outfd == STDOUT_FILENO)
		return;

	if (lseek(outfd, SEEK_SET, 0) < 0)
		err(1, "could not seek to start of file for header rewrite");
	write_header();
}

void
usage()
{

	fprintf(stderr, "Usage: %s [-afhqV] [options] {files ...|-}\n",
	    getprogname());
	fprintf(stderr, "Options:\n\t"
	    "-C audio control device\n\t"
	    "-F format\n\t"
	    "-b balance (0-63)\n\t"
	    "-c channels\n\t"
	    "-d audio device\n\t"
	    "-e encoding\n\t"
	    "-i header information\n\t"
	    "-m monitor volume\n\t"
	    "-P precision bits (4, 8, 16, 24 or 32)\n\t"
	    "-p input port\n\t"
	    "-s sample rate\n\t"
	    "-t recording time\n\t"
	    "-v volume\n");
	exit(EXIT_FAILURE);
}
