/* vgmstream123.c
 *
 * Simple player frontend for vgmstream
 * Copyright (c) 2017 Daniel Richard G. <skunk@iSKUNK.ORG>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#include <ao/ao.h>
#include <sys/time.h>
#ifdef WIN32
# include <io.h>
# include <fcntl.h>
#else
# include <signal.h>
# include <unistd.h>
# include <sys/wait.h>
#endif

#include "../src/vgmstream.h"

#ifndef VERSION
# include "version.h"
#endif
#ifndef VERSION
# define VERSION "(unknown version)"
#endif


//TODO: improve WIN32 builds (some features/behaviors are missing but works)
#ifdef WIN32
#define getline(line, line_mem, f)  0
#define mkdtemp(temp_dir)  0
#define signal(sig, interrupt_handler)  /*nothing*/
#define WIFSIGNALED(ret)  0
#define WTERMSIG(ret)  0
#define SIGQUIT  0
#define SIGINT  0
#define SIGHUP  0
#endif

/* If two interrupts (i.e. Ctrl-C) are received
 * within a span of this many seconds, then exit
 */
#define DOUBLE_INTERRUPT_TIME 1.0

/* TODO: Make sure this whole mess works for big-endian systems
 */
#define LITTLE_ENDIAN_OUTPUT

#undef  MIN
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

/* Stream playback parameters
 */
struct params {
    int loop_count;
    double min_time;
    double fade_time;
    double fade_delay;
    int stream_index;
};

#define DEFAULT_PARAMS { 2, -1, 10.0, 0.0, 0 }

static const char *out_filename = NULL;
static int driver_id;
static ao_device *device = NULL;
static ao_option *device_options = NULL;
static ao_sample_format current_sample_format;

static sample_t *buffer = NULL;
/* reportedly 1kb helps Raspberry Pi Zero play FFmpeg formats without stuttering
 * (presumably other low powered devices too), plus it's the default in other plugins */
static int buffer_size_kb = 1;

static int repeat = 0;
static int verbose = 0;

static volatile int interrupted = 0;
static double interrupt_time = 0.0;

static int play_file(const char *filename, struct params *par);

static void interrupt_handler(int signum) {
    interrupted = 1;
}

static int record_interrupt(void) {
    int ret = 0;
    struct timeval tv = { 0, 0 };
    double t;

    if (gettimeofday(&tv, NULL))
        return -1;

    t = (double)tv.tv_sec + (double)tv.tv_usec / 1.0e6;

    if (t - interrupt_time < DOUBLE_INTERRUPT_TIME)
        ret = 1;

    interrupt_time = t;
    interrupted = 0;

    return ret;
}

static void usage(const char *progname) {
    struct params default_par = DEFAULT_PARAMS;
    const char *default_driver = "???";

    {
        ao_info *info = ao_driver_info(driver_id);
        if (info)
            default_driver = info->short_name;
    }

    printf("vgmstream123 " VERSION ", built " __DATE__ "\n"
        "\n"
        "Usage: %s [options] INFILE ...\n"
        "Play streamed audio from video games.\n"
        "\n"
        "Options:\n"
        "    -d DRV      Use output driver DRV [%s]; available drivers:\n"
        "                ",
        progname,
        default_driver);

    {
        ao_info **info_list;
        int driver_count = 0;
        int i;

        info_list = ao_driver_info_list(&driver_count);

        for (i = 0; i < driver_count; i++)
            printf("%s ", info_list[i]->short_name);
    }

    printf("\n"
        "    -f OUTFILE  Set output filename for a file driver specified with -d\n"
        "    -o KEY:VAL  Pass option KEY with value VAL to the output driver\n"
        "                (see https://www.xiph.org/ao/doc/drivers.html)\n"
        "    -b N        Use an audio buffer of N kilobytes [%d]\n"
        "    -@ LSTFILE  Read playlist from LSTFILE\n"
        "    -h          Print this help\n"
        "    -r          Repeat playback indefinitely\n"
        "    -v          Display stream metadata and playback progress\n"
        "    -S N        Play substream with index N [%d]\n"
        "\n"
        "Options for looped streams:\n"
        "    -L N        Play loop N times [%d]\n"
        "    -M MINTIME  Loop for a playback time of at least MINTIME seconds\n"
        "    -F FTIME    End playback with a fade-out of FTIME seconds [%.1f]\n"
        "    -D FDELAY   Delay fade-out for an additional FDELAY seconds [%.1f]\n"
        "\n"
        "INFILE can be any stream file type supported by vgmstream, or an .m3u/.m3u8\n"
        "playlist referring to same. This program supports the \"EXT-X-VGMSTREAM\" tag\n"
        "in playlists, and files compressed with gzip/bzip2/xz.\n",
        buffer_size_kb,
        default_par.stream_index,
        default_par.loop_count,
        default_par.fade_time,
        default_par.fade_delay
    );
}

/* Opens the audio device with the appropriate parameters
 */
static int set_sample_format(VGMSTREAM *vgms) {
    ao_sample_format format;

    memset(&format, 0, sizeof(format));
    format.bits = 8 * sizeof(sample);
    format.channels = vgms->channels;
    format.rate = vgms->sample_rate;
    format.byte_format =
#ifdef LITTLE_ENDIAN_OUTPUT
        AO_FMT_LITTLE
#else
        AO_FMT_BIG
#endif
    ;

    if (memcmp(&format, &current_sample_format, sizeof(format))) {

        /* Sample format has changed, so (re-)open audio device */

        ao_info *info = ao_driver_info(driver_id);
        if (!info) return -1;

        if ((info->type == AO_TYPE_FILE) != !!out_filename) {
            if (out_filename)
                fprintf(stderr, "Live output driver \"%s\" does not take an output file\n", info->short_name);
            else
                fprintf(stderr, "File output driver \"%s\" requires an output filename\n", info->short_name);
            return -1;
        }

        if (device)
            ao_close(device);

        memcpy(&current_sample_format, &format, sizeof(format));

        if (out_filename)
            device = ao_open_file(driver_id, out_filename, 1, &format, device_options);
        else
            device = ao_open_live(driver_id, &format, device_options);

        if (!device) {
            fprintf(stderr, "Error opening \"%s\" audio device\n", info->short_name);
            return -1;
        }
    }

    return 0;
}

static int play_vgmstream(const char *filename, struct params *par) {
    int ret = 0;
    STREAMFILE *sf;
    VGMSTREAM *vgms;
    FILE *save_fps[4];
    int loop_count;
    int64_t total_samples;
    size_t buffer_size;
    int64_t buffer_samples;
    int64_t fade_time_samples, fade_start;
    int time_total_min;
    double time_total_sec;
    int64_t s;
    int i;

    sf = open_stdio_streamfile(filename);
    if (!sf) {
        fprintf(stderr, "%s: cannot open file\n", filename);
        return -1;
    }

    sf->stream_index = par->stream_index;
    vgms = init_vgmstream_from_STREAMFILE(sf);
    close_streamfile(sf);

    if (!vgms) {
        fprintf(stderr, "%s: error opening stream\n", filename);
        return -1;
    }

    printf("Playing stream: %s\n", filename);

    /* Print metadata in verbose mode
     */
    if (verbose) {
        char description[4096] = { '\0' };
        describe_vgmstream(vgms, description, sizeof(description));
        puts(description);
        putchar('\n');
    }

    /* If the audio device hasn't been opened yet, then describe it
     */
    if (!device) {
        ao_info *info = ao_driver_info(driver_id);
        printf("Audio device: %s\n", info->name);
        printf("Comment: %s\n", info->comment);
        putchar('\n');
    }

    /* Stupid hack to hang onto a few low-numbered file descriptors
     * so that play_compressed_file() doesn't break, due to POSIX
     * wackiness like https://bugs.debian.org/590920
     */
    for (i = 0; i < 4; i++)
        save_fps[i] = fopen("/dev/null", "r");

    ret = set_sample_format(vgms);
    if (ret) goto fail;

    loop_count = par->loop_count;

    if (vgms->loop_flag && loop_count < 0) {
        /*
         * Calculate how many loops are needed to achieve a minimum
         * playback time. Note: This calculation is derived from the
         * logic in get_vgmstream_play_samples().
         */
        double intro = (double)vgms->loop_start_sample / vgms->sample_rate;
        double loop = (double)(vgms->loop_end_sample - vgms->loop_start_sample) / vgms->sample_rate;
        double end = par->fade_time + par->fade_delay;
        if (loop < 1.0) loop = 1.0;
        loop_count = (int)((par->min_time - intro - end) / loop + 0.99);
        if (loop_count < 1) loop_count = 1;
    }

    total_samples = get_vgmstream_play_samples(loop_count, par->fade_time, par->fade_delay, vgms);

    {
        double total = (double)total_samples / vgms->sample_rate;
        time_total_min = (int)total / 60;
        time_total_sec = total - 60 * time_total_min;
    }

    /* Buffer size in bytes
     */
    buffer_size = 1024 * buffer_size_kb;

    if (!buffer) {
        if (buffer_size_kb < 1) {
            fprintf(stderr, "Invalid buffer size '%d'\n", buffer_size_kb);
            return -1;
        }

        buffer = malloc(buffer_size);
        if (!buffer) goto fail;
    }

    buffer_samples = buffer_size / (vgms->channels * sizeof(sample));

    fade_time_samples = (int64_t)(par->fade_time * vgms->sample_rate);
    fade_start = total_samples - fade_time_samples;
    if (fade_start < 0)
        fade_start = total_samples;

    for (s = 0; s < total_samples && !interrupted; s += buffer_samples) {
        int64_t buffer_used_samples = MIN(buffer_samples, total_samples - s);
        char *suffix = "";

        render_vgmstream(buffer, buffer_used_samples, vgms);

#ifdef LITTLE_ENDIAN_OUTPUT
        swap_samples_le(buffer, vgms->channels * buffer_used_samples);
#endif

        if (vgms->loop_flag && fade_time_samples > 0 && s >= fade_start) {
            /* Got to do the fade-out ourselves :p */
            int64_t b;
            for (b = 0; b < buffer_used_samples; b++) {
                double factor = 1.0 - (double)(s + b - fade_start) / fade_time_samples;
                int c;
                for (c = 0; c < vgms->channels; c++)
                    buffer[vgms->channels * b + c] *= factor;
            }
            suffix = " (fading)";
        }

        if (verbose && !out_filename) {
            double played = (double)s / vgms->sample_rate;
            double remain = (double)(total_samples - s) / vgms->sample_rate;

            int time_played_min = (int)played / 60;
            double time_played_sec = played - 60 * time_played_min;
            int time_remain_min = (int)remain / 60;
            double time_remain_sec = remain - 60 * time_remain_min;

            /* Time: 01:02.34 [08:57.66] of 10:00.00 */
            printf("\rTime: %02d:%05.2f [%02d:%05.2f] of %02d:%05.2f%s ",
                time_played_min, time_played_sec,
                time_remain_min, time_remain_sec,
                time_total_min,  time_total_sec,
                suffix);

            fflush(stdout);
        }

        if (!ao_play(device, (char *)buffer, buffer_used_samples * vgms->channels * sizeof(sample))) {
            fputs("\nAudio playback error\n", stderr);
            ao_close(device);
            device = NULL;
            ret = -1;
            break;
        }
    }

    if (verbose && !ret) {
        /* Clear time status line */
        putchar('\r');
        for (i = 0; i < 64; i++)
            putchar(' ');
        putchar('\r');
        fflush(stdout);
    }

    if (out_filename && !ret)
        printf("Wrote %02d:%05.2f of audio to %s\n\n",
            time_total_min, time_total_sec, out_filename);

    if (interrupted) {
        fputs("Playback terminated.\n\n", stdout);
        ret = record_interrupt();
        if (ret) fputs("Exiting...\n", stdout);
    }

    fail:

    close_vgmstream(vgms);

    for (i = 0; i < 4; i++)
        fclose(save_fps[i]);

    return ret;
}

static int play_playlist(const char *filename, struct params *default_par) {
    int ret = 0;
    FILE *f;
    char *line = NULL;
    size_t line_mem = 0;
    ssize_t line_len = 0;
    struct params par;

    memcpy(&par, default_par, sizeof(par));

    f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "%s: cannot open playlist file\n", filename);
        return -1;
    }

    while ((line_len = getline(&line, &line_mem, f)) >= 0) {

        /* Remove any leading whitespace
         */
        size_t ws_len = strspn(line, "\t ");
        if (ws_len > 0) {
            line_len -= ws_len;
            memmove(line, line + ws_len, line_len + 1);
        }

        /* Remove trailing whitespace
         */
        while (line_len >= 1 && (line[line_len - 1] == '\r' || line[line_len - 1] == '\n'))
            line[--line_len] = '\0';

#define EXT_PREFIX "#EXT-X-VGMSTREAM:"

        if (!strncmp(line, EXT_PREFIX, sizeof(EXT_PREFIX) - 1)) {

            /* Parse vgmstream-specific metadata */

            char *param = strtok(line + sizeof(EXT_PREFIX) - 1, ",");

#define PARAM_MATCHES(NAME) (!strncmp(param, NAME "=", sizeof(NAME)) && arg)

            while (param) {
                char *arg = strchr(param, '=');
                if (arg) arg++;

                if (PARAM_MATCHES("FADEDELAY"))
                    par.fade_delay = atof(arg);
                else if (PARAM_MATCHES("FADETIME"))
                    par.fade_time = atof(arg);
                else if (PARAM_MATCHES("LOOPCOUNT"))
                    par.loop_count = atoi(arg);
                else if (PARAM_MATCHES("STREAMINDEX"))
                    par.stream_index = atoi(arg);

                param = strtok(NULL, ",");
            }
        }

        /* Skip blank or comment lines
         */
        if (line[0] == '\0' || line[0] == '#')
            continue;

        ret = play_file(line, &par);
        if (ret) break;

        /* Reset playback options to default */
        memcpy(&par, default_par, sizeof(par));
    }

    free(line);
    fclose(f);

    return ret;
}

static int play_compressed_file(const char *filename, struct params *par, const char *expand_cmd) {
    int ret;
    char temp_dir[128] = "/tmp/vgmXXXXXX";
    const char *base_name;
    char *last_slash, *last_dot;
    char *cmd = NULL, *temp_file = NULL;
    FILE *in_fp, *out_fp;

    cmd = malloc(strlen(filename) + 1024);
    temp_file = malloc(strlen(filename) + 256);

    if (!cmd || !temp_file)
        return -2;

    if (!mkdtemp(temp_dir)) {
        fprintf(stderr, "%s: error creating temp dir for decompression\n", temp_dir);
        ret = -1;
        goto fail;
    }

    /* Get the base name of the file path
     */
    last_slash = strrchr(filename, '/');
    if (last_slash)
        base_name = last_slash + 1;
    else
        base_name = filename;

    sprintf(temp_file, "%s/%s", temp_dir, base_name);

    /* Chop off the compressed-file extension
     */
    last_dot = strrchr(temp_file, '.');
    if (last_dot) *last_dot = '\0';

    printf("Decompressing file: %s\n", filename);

    in_fp  = fopen(filename, "rb");
    out_fp = fopen(temp_file, "wb");

    if (in_fp && out_fp) {
        setbuf(in_fp, NULL);
        setbuf(out_fp, NULL);

        /* Don't put filenames into the system() arg; that's insecure!
         */
        sprintf(cmd, "%s <&%d >&%d ", expand_cmd, fileno(in_fp), fileno(out_fp));
        ret = system(cmd);

        if (WIFSIGNALED(ret) && (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
            interrupted = 1;
    }
    else
        ret = -1;

    if (in_fp && fclose(in_fp))
        ret = -1;
    if (out_fp && fclose(out_fp))
        ret = -1;

    if (ret) {
        if (interrupted) {
            putchar('\r');
            ret = record_interrupt();
            if (ret) fputs("Exiting...\n", stdout);
        }
        else
            fprintf(stderr, "%s: error decompressing file\n", filename);
    }
    else
        ret = play_file(temp_file, par);

    remove(temp_file);
    remove(temp_dir);

fail:
    free(cmd);
    free(temp_file);

    return ret;
}

static int play_file(const char *filename, struct params *par) {
    size_t len = strlen(filename);

#define ENDS_IN(EXT) !strcasecmp(EXT, filename + len - sizeof(EXT) + 1)

    if (ENDS_IN(".m3u") || ENDS_IN(".m3u8"))
        return play_playlist(filename, par);
    else if (ENDS_IN(".bz2"))
        return play_compressed_file(filename, par, "bzip2 -cd");
    else if (ENDS_IN(".gz"))
        return play_compressed_file(filename, par, "gzip -cd");
    else if (ENDS_IN(".lzma"))
        return play_compressed_file(filename, par, "lzma -cd");
    else if (ENDS_IN(".xz"))
        return play_compressed_file(filename, par, "xz -cd");
    else
        return play_vgmstream(filename, par);
}

static void add_driver_option(const char *key_value) {
    char buf[1024];
    char *value = NULL;
    char *sep;

    strncpy(buf, key_value, sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';

    sep = strchr(buf, ':');
    if (sep) {
        *sep = '\0';
        value = sep + 1;
    }

    ao_append_option(&device_options, buf, value);
}

int main(int argc, char **argv) {
    int status = 0;
    struct params par;
    int opt;

    signal(SIGHUP,  interrupt_handler);
    signal(SIGINT,  interrupt_handler);
    signal(SIGQUIT, interrupt_handler);

    ao_initialize();
    driver_id = ao_default_driver_id();
    memset(&current_sample_format, 0, sizeof(current_sample_format));

    if (argc == 1) {
        /* We were invoked with no arguments */
        usage(argv[0]);
        goto done;
    }

again:

    {
        struct params default_par = DEFAULT_PARAMS;
        memcpy(&par, &default_par, sizeof(par));
    }

    while ((opt = getopt(argc, argv, "-D:F:L:M:S:b:d:f:o:@:hrv")) != -1) {
        switch (opt) {
            case 1:
                if (play_file(optarg, &par)) {
                    status = 1;
                    goto done;
                }
                break;
            case '@':
                if (play_playlist(optarg, &par)) {
                    status = 1;
                    goto done;
                }
                break;
            case 'D':
                par.fade_delay = atof(optarg);
                break;
            case 'F':
                par.fade_time = atof(optarg);
                break;
            case 'L':
                par.loop_count = atoi(optarg);
                break;
            case 'M':
                par.min_time = atof(optarg);
                par.loop_count = -1;
                break;
            case 'S':
                par.stream_index = atoi(optarg);
                break;
            case 'b':
                if (!buffer)
                    buffer_size_kb = atoi(optarg);
                break;
            case 'd':
                driver_id = ao_driver_id(optarg);
                if (driver_id < 0) {
                    fprintf(stderr, "Invalid output driver \"%s\"\n", optarg);
                    status = 1;
                    goto done;
                }
                break;
            case 'f':
                out_filename = optarg;
                break;
            case 'h':
                usage(argv[0]);
                goto done;
            case 'o':
                add_driver_option(optarg);
                break;
            case 'r':
                repeat = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                VGM_LOG("vgmstream123: unknown opt %x", opt);
                goto done;
        }
    }

    /* try to read infile here in case getopt didn't pass "1" to the above switch I guess */
    argc -= optind;
    argv += optind;

    for (opt = 0; opt < argc; ++opt) {
        if (play_file(argv[opt], &par)) {
            status = 1;
            goto done;
        }
    }

    if (repeat) {
        optind = 0;
        goto again;
    }

done:

    if (device)
        ao_close(device);
    if (buffer)
        free(buffer);

    ao_free_options(device_options);
    ao_shutdown();

    return status;
}

/* end vgmstream123.c */
