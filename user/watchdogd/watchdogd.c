#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <signal.h>
#include <paths.h>

int fd = -1;

/*
 * This function simply sends an IOCTL to the driver, which in turn ticks
 * the PC Watchdog card to reset its internal timer so it doesn't trigger
 * a computer reset.
 */
void keep_alive(void)
{
	int dummy;

	ioctl(fd, WDIOC_KEEPALIVE, &dummy);
}

void safe_exit()
{
	if (fd != -1) {
		write(fd, "V", 1);
		close(fd);
	}
	exit(0);
}

int set_wd_counter(int count)
{
	return ioctl(fd, WDIOC_SETTIMEOUT, &count);
}

int get_wd_counter()
{
	int count;
	int err;
	if ((err = ioctl(fd, WDIOC_GETTIMEOUT, &count))) {
		count = err;
	}
	return count;
}

#define FOREGROUND_FLAG "-f"

#define _PATH_DEVNULL "/dev/null"

void vfork_daemon_rexec(int nochdir, int noclose, int argc, char **argv, char *foreground_opt)
{
	int f;
	char **vfork_args;
	int a = 0;

	setsid();

	if (!nochdir)
		chdir("/");

	if (!noclose && (f = open(_PATH_DEVNULL, O_RDWR, 0)) != -1) {
		dup2(f, STDIN_FILENO);
		dup2(f, STDOUT_FILENO);
		dup2(f, STDERR_FILENO);
		if (f > 2)
			close(f);
	}

	vfork_args = malloc(sizeof(char *) * (argc + 2));
	while (*argv) {
		vfork_args[a++] = *argv;
		argv++;
	}
	vfork_args[a++] = foreground_opt;
	vfork_args[a++] = NULL;
	switch (vfork()) {
	case 0:		/* child */
		/* Make certain we are not a session leader, or else we
		 * might reacquire a controlling terminal */
		if (vfork())
			_exit(0);
		execvp(vfork_args[0], vfork_args);
#ifndef WATCHDOG_SILENT
		perror("execv");
#endif
		exit(-1);
	case -1:		/* error */
#ifndef WATCHDOG_SILENT
		perror("vfork");
#endif
		exit(-1);
	default:		/* parent */
		exit(0);
	}
}

#ifndef WATCHDOG_SILENT
static void usage(char *argv[])
{
	printf(
		"%s [-f] [-w <sec>] [-k <sec>] [-s] [-h|--help]\n"
		"A simple watchdog daemon that send WDIOC_KEEPALIVE ioctl every some\n"
		"\"heartbeat of keepalives\" seconds.\n"
		"Options:\n"
		"\t-f        start in foreground (background is default)\n"
		"\t-w <sec>  set the watchdog counter to <sec> in seconds\n"
		"\t-k <sec>  set the \"heartbeat of keepalives\" to <sec> in seconds\n"
		"\t-s        safe exit (disable Watchdog) for CTRL-c and kill -SIGTERM signals\n"
		"\t--help|-h write this help message and exit\n",
		argv[0]);
}
#endif

/*
 * The main program.
 */
int main(int argc, char *argv[])
{
	int wd_count = 20;
	int real_wd_count = 0;
	int wd_keep_alive = wd_count / 2;
	struct sigaction sa;
	int background = 1;
	int ac = argc;
	char **av = argv;


	memset(&sa, 0, sizeof(sa));

	/* TODO: rewrite this to use getopt() */
	while (--ac) {
		++av;
		if (strcmp(*av, "-w") == 0) {
			if (--ac) {
				wd_count = atoi(*++av);
#ifndef WATCHDOG_SILENT
				printf("-w switch: set watchdog counter to %d sec.\n", wd_count);
#endif
			} else {
#ifndef WATCHDOG_SILENT
				fprintf(stderr, "-w switch must be followed to seconds of watchdog counter.\n");
				fflush(stderr);
#endif
				break;
			}
		} else if (strcmp(*av, "-k") == 0) {
			if (--ac) {
				wd_keep_alive = atoi(*++av);
#ifndef WATCHDOG_SILENT
				printf("-k switch: set the heartbeat of keepalives in %d sec.\n", wd_keep_alive);
#endif
			} else {
#ifndef WATCHDOG_SILENT
				fprintf(stderr, "-k switch must be followed to seconds of heartbeat of keepalives.\n");
				fflush(stderr);
#endif
				break;
			}
		} else if (strcmp(*av, "-s") == 0) {
#ifndef WATCHDOG_SILENT
			printf("-s switch: safe exit (CTRL-C and kill).\n");
#endif
			sa.sa_handler = safe_exit;
			sigaction(SIGHUP, &sa, NULL);
			sigaction(SIGINT, &sa, NULL);
			sigaction(SIGTERM, &sa, NULL);
		} else if (strcmp(*av, FOREGROUND_FLAG) == 0) {
			background = 0;
#ifndef WATCHDOG_SILENT
			printf("Start in foreground mode.\n");
#endif
		}
#ifndef WATCHDOG_SILENT
		else if ((strcmp(*av, "-h") == 0) || (strcmp(*av, "--help") == 0)) {
			usage(argv);
			exit(0);
		}
#endif
		else {
#ifndef WATCHDOG_SILENT
			fprintf(stderr, "Unrecognized option \"%s\".\n", *av);
			usage(argv);
#endif
			exit(1);
		}
	}

	if (background) {
#ifndef WATCHDOG_SILENT
		printf("Start in daemon mode.\n");
#endif
		vfork_daemon_rexec(1, 0, argc, argv, FOREGROUND_FLAG);
	}

	fd = open("/dev/watchdog", O_WRONLY);

	if (fd == -1) {
#ifndef WATCHDOG_SILENT
		perror("Watchdog device not enabled");
		fflush(stderr);
#endif
		exit(-1);
	}

	if (set_wd_counter(wd_count)) {
#ifndef WATCHDOG_SILENT
		fprintf(stderr, "-w switch: wrong value. Please look at kernel log for more dettails.\n Continue with the old value\n");
		fflush(stderr);
#endif
	}

	real_wd_count = get_wd_counter();
	if (real_wd_count < 0) {
#ifndef WATCHDOG_SILENT
		perror("Error while issue IOCTL WDIOC_GETTIMEOUT");
#endif
	} else {
		if (real_wd_count <= wd_keep_alive) {
#ifndef WATCHDOG_SILENT
			fprintf(stderr,
				"Warning watchdog counter less or equal to the heartbeat of keepalives: %d <= %d\n",
				real_wd_count, wd_keep_alive);
			fflush(stderr);
#endif
		}
	}

	while (1) {
		keep_alive();
		sleep(wd_keep_alive);
	}
}
