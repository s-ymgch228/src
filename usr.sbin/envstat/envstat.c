/* $NetBSD: envstat.c,v 1.59 2008/04/17 20:51:48 xtraeme Exp $ */

/*-
 * Copyright (c) 2007, 2008 Juan Romero Pardines.
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: envstat.c,v 1.59 2008/04/17 20:51:48 xtraeme Exp $");
#endif /* not lint */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <syslog.h>
#include <sys/envsys.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <prop/proplib.h>

#include "envstat.h"

#define _PATH_DEV_SYSMON	"/dev/sysmon"

#define ENVSYS_DFLAG	0x00000001	/* list registered devices */
#define ENVSYS_FFLAG	0x00000002	/* show temp in farenheit */
#define ENVSYS_LFLAG	0x00000004	/* list sensors */
#define ENVSYS_XFLAG	0x00000008	/* externalize dictionary */
#define ENVSYS_IFLAG 	0x00000010	/* skips invalid sensors */
#define ENVSYS_SFLAG	0x00000020	/* removes all properties set */

typedef struct envsys_sensor {
	SIMPLEQ_ENTRY(envsys_sensor) entries;
	int32_t	cur_value;
	int32_t	max_value;
	int32_t	min_value;
	int32_t	avg_value;
	int32_t critcap_value;
	int32_t	critmin_value;
	int32_t	critmax_value;
	char	desc[ENVSYS_DESCLEN];
	char	type[ENVSYS_DESCLEN];
	char	drvstate[ENVSYS_DESCLEN];
	char	battcap[ENVSYS_DESCLEN];
	char 	dvname[ENVSYS_DESCLEN];
	bool	invalid;
	bool	visible;
	bool	percentage;
	bool	dvnprinted;
} *sensor_t;

typedef struct envsys_dvprops {
	uint64_t	refresh_timo;
	char 		refresh_units[ENVSYS_DESCLEN];
	/* more values could be added in the future */
} *dvprops_t;

/* A simple queue to manage all sensors */
static SIMPLEQ_HEAD(, envsys_sensor) sensors_list = 
    SIMPLEQ_HEAD_INITIALIZER(sensors_list);

static unsigned int 	interval, flags, width;
static char 		*mydevname, *sensors;

static int 		parse_dictionary(int);
static int 		send_dictionary(FILE *, int);
static int 		find_sensors(prop_array_t, const char *, dvprops_t);
static void 		print_sensors(void);
static int 		check_sensors(char *);
static int 		usage(void);


int main(int argc, char **argv)
{
	prop_dictionary_t dict;
	int c, fd, rval = 0;
	char *endptr, *configfile = NULL;
	FILE *cf;

	setprogname(argv[0]);

	while ((c = getopt(argc, argv, "c:Dd:fIi:lrSs:w:x")) != -1) {
		switch (c) {
		case 'c':	/* configuration file */
			configfile = strdup(optarg);
			if (configfile == NULL)
				err(EXIT_FAILURE, "strdup");
			break;
		case 'D':	/* list registered devices */
			flags |= ENVSYS_DFLAG;
			break;
		case 'd':	/* show sensors of a specific device */
			mydevname = strdup(optarg);
			if (mydevname == NULL)
				err(EXIT_FAILURE, "strdup");
			break;
		case 'f':	/* display temperature in Farenheit */
			flags |= ENVSYS_FFLAG;
			break;
		case 'I':	/* Skips invalid sensors */
			flags |= ENVSYS_IFLAG;
			break;
		case 'i':	/* wait time between intervals */
			interval = (unsigned int)strtoul(optarg, &endptr, 10);
			if (*endptr != '\0')
				errx(EXIT_FAILURE, "bad interval '%s'", optarg);
			break;
		case 'l':	/* list sensors */
			flags |= ENVSYS_LFLAG;
			break;
		case 'r':
			/* 
			 * This flag is noop.. it's only here for
			 * compatibility with the old implementation.
			 */
			break;
		case 'S':
			flags |= ENVSYS_SFLAG;
			break;
		case 's':	/* only show specified sensors */
			sensors = strdup(optarg);
			if (sensors == NULL)
				err(EXIT_FAILURE, "strdup");
			break;
		case 'w':	/* width value for the lines */
			width = strtoul(optarg, &endptr, 10);
			if (*endptr != '\0')
				errx(EXIT_FAILURE, "bad width '%s'", optarg);
			break;
		case 'x':	/* print the dictionary in raw format */
			flags |= ENVSYS_XFLAG;
			break;
		case '?':
		default:
			usage();
			/* NOTREACHED */
		}
	}

	argc -= optind;
	argv += optind;

	if (argc > 0)
		usage();

	if (mydevname && sensors)
		errx(EXIT_FAILURE, "-d flag cannot be used with -s");

	/* Open the device in ro mode */
	if ((fd = open(_PATH_DEV_SYSMON, O_RDONLY)) == -1)
		err(EXIT_FAILURE, "%s", _PATH_DEV_SYSMON);

	/* Print dictionary in raw mode */
	if (flags & ENVSYS_XFLAG) {
		rval = prop_dictionary_recv_ioctl(fd,
						  ENVSYS_GETDICTIONARY,
						  &dict);
		if (rval)
			errx(EXIT_FAILURE, "%s", strerror(rval));

		config_dict_dump(dict);

	/* Remove all properties set in dictionary */
	} else if (flags & ENVSYS_SFLAG) {
		/* Close the ro descriptor */
		(void)close(fd);

		/* open the fd in rw mode */
		if ((fd = open(_PATH_DEV_SYSMON, O_RDWR)) == -1)
			err(EXIT_FAILURE, "%s", _PATH_DEV_SYSMON);

		dict = prop_dictionary_create();
		if (!dict)
			err(EXIT_FAILURE, "prop_dictionary_create");
		
		rval = prop_dictionary_set_bool(dict,
						"envsys-remove-props",
					        true);
		if (!rval)
			err(EXIT_FAILURE, "prop_dict_set_bool");

		/* send the dictionary to the kernel now */
		rval = prop_dictionary_send_ioctl(dict, fd, ENVSYS_REMOVEPROPS);
		if (rval)
			warnx("%s", strerror(rval));

	/* Set properties in dictionary */
	} else if (configfile) {
		/*
		 * Parse the configuration file.
		 */
		if ((cf = fopen(configfile, "r")) == NULL) {
			syslog(LOG_ERR, "fopen failed: %s", strerror(errno));
			errx(EXIT_FAILURE, "%s", strerror(errno));
		}

		rval = send_dictionary(cf, fd);
		(void)fclose(cf);

	/* Show sensors with interval */
	} else if (interval) {
		for (;;) {
			rval = parse_dictionary(fd);
			if (rval)
				break;

			(void)fflush(stdout);
			(void)sleep(interval);
		}
	/* Show sensors without interval */
	} else {
		rval = parse_dictionary(fd);
	}

	if (sensors)
		free(sensors);
	if (mydevname)
		free(mydevname);
	(void)close(fd);

	return rval ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int
send_dictionary(FILE *cf, int fd)
{
	prop_dictionary_t kdict, udict;
	int error = 0;

	/* Retrieve dictionary from kernel */
	error = prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &kdict);
      	if (error)
		return error;

	config_parse(cf, kdict);

	/*
	 * Dictionary built by the parser from the configuration file.
	 */
	udict = config_dict_parsed();

	/*
	 * Close the read only descriptor and open a new one read write.
	 */
	(void)close(fd);
	if ((fd = open(_PATH_DEV_SYSMON, O_RDWR)) == -1) {
		error = errno;
		warn("%s", _PATH_DEV_SYSMON);
		return error;
	}

	/* 
	 * Send our sensor properties dictionary to the kernel then.
	 */
	error = prop_dictionary_send_ioctl(udict, fd, ENVSYS_SETDICTIONARY);
	if (error)
		warnx("%s", strerror(error));

	prop_object_release(udict);
	return error;
}

static int
parse_dictionary(int fd)
{
	sensor_t sensor = NULL;
	dvprops_t edp = NULL;
	prop_array_t array;
	prop_dictionary_t dict;
	prop_object_iterator_t iter;
	prop_object_t obj;
	const char *dnp = NULL;
	int rval = 0;

	/* receive dictionary from kernel */
	rval = prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &dict);
	if (rval)
		return rval;

	/* No drivers registered? */
	if (prop_dictionary_count(dict) == 0) {
		warnx("no drivers registered");
		goto out;
	}

	if (mydevname) {
		/* -d flag specified, print sensors only for this device */
		obj = prop_dictionary_get(dict, mydevname);
		if (prop_object_type(obj) != PROP_TYPE_ARRAY) {
			warnx("unknown device `%s'", mydevname);
			rval = EINVAL;
			goto out;
		}

		rval = find_sensors(obj, mydevname, NULL);
		if (rval)
			goto out;

	} else {
		/* print sensors for all devices registered */
		iter = prop_dictionary_iterator(dict);
		if (iter == NULL) {
			rval = EINVAL;
			goto out;
		}

		/* iterate over the dictionary returned by the kernel */
		while ((obj = prop_object_iterator_next(iter)) != NULL) {
			array = prop_dictionary_get_keysym(dict, obj);
			if (prop_object_type(array) != PROP_TYPE_ARRAY) {
				warnx("no sensors found");
				rval = EINVAL;
				goto out;
			}

			edp = calloc(1, sizeof(*edp));
			if (!edp) {
				rval = ENOMEM;
				goto out;
			}

			dnp = prop_dictionary_keysym_cstring_nocopy(obj);
			rval = find_sensors(array, dnp, edp);
			if (rval)
				goto out;

			if (((flags & ENVSYS_LFLAG) == 0) &&
			    (flags & ENVSYS_DFLAG)) {
				(void)printf("%s (checking events every ",
				    dnp);
				if (edp->refresh_timo == 1)
					(void)printf("second)\n");
				else
					(void)printf("%d seconds)\n",
					    (int)edp->refresh_timo);
			}
			
			free(edp);
			edp = NULL;
		}
		prop_object_iterator_release(iter);
	}

	/* print sensors now */
	if (sensors) {
		char *str = strdup(sensors);
		if (!str) {
			rval = ENOMEM;
			goto out;
		}
		rval = check_sensors(str);
		if (rval) {
			free(str);
			goto out;
		}
		free(str);
	}
	if ((flags & ENVSYS_LFLAG) == 0 && (flags & ENVSYS_DFLAG) == 0)
		print_sensors();
	if (interval)
		(void)printf("\n");

out:
	while ((sensor = SIMPLEQ_FIRST(&sensors_list))) {
		SIMPLEQ_REMOVE_HEAD(&sensors_list, entries);
		free(sensor);
	}
	if (edp)
		free(edp);
	prop_object_release(dict);
	return rval;
}

static int
find_sensors(prop_array_t array, const char *dvname, dvprops_t edp)
{
	prop_object_iterator_t iter;
	prop_object_t obj, obj1, obj2;
	prop_string_t state, desc = NULL;
	sensor_t sensor = NULL;

	iter = prop_array_iterator(array);
	if (!iter)
		return ENOMEM;

	/* iterate over the array of dictionaries */
	while ((obj = prop_object_iterator_next(iter)) != NULL) {
		/* get the refresh-timeout property */
		obj2 = prop_dictionary_get(obj, "device-properties");
		if (obj2) {
			if (!edp)
				continue;
			if (!prop_dictionary_get_uint64(obj2,
							"refresh-timeout",
							&edp->refresh_timo))
				continue;
		}

		/* new sensor coming */
		sensor = calloc(1, sizeof(*sensor));
		if (sensor == NULL)
			return ENOMEM;

		/* copy device name */
		(void)strlcpy(sensor->dvname, dvname, sizeof(sensor->dvname));

		/* description string */
		desc = prop_dictionary_get(obj, "description");
		if (desc) {
			/* copy description */
			(void)strlcpy(sensor->desc,
			    prop_string_cstring_nocopy(desc),
		    	    sizeof(sensor->desc));
		} else {
			free(sensor);
			continue;
		}

		/* type string */
		obj1  = prop_dictionary_get(obj, "type");
		if (obj1) {
			/* copy type */
			(void)strlcpy(sensor->type,
		    	    prop_string_cstring_nocopy(obj1),
		    	    sizeof(sensor->type));
		} else {
			free(sensor);
			continue;
		}

		/* check sensor's state */
		state = prop_dictionary_get(obj, "state");

		/* mark sensors with invalid/unknown state */
		if ((prop_string_equals_cstring(state, "invalid") ||
		     prop_string_equals_cstring(state, "unknown")))
			sensor->invalid = true;

		/* get current drive state string */
		obj1 = prop_dictionary_get(obj, "drive-state");
		if (obj1) {
			(void)strlcpy(sensor->drvstate,
			    prop_string_cstring_nocopy(obj1),
			    sizeof(sensor->drvstate));
		}

		/* get current battery capacity string */
		obj1 = prop_dictionary_get(obj, "battery-capacity");
		if (obj1) {
			(void)strlcpy(sensor->battcap,
			    prop_string_cstring_nocopy(obj1),
			    sizeof(sensor->battcap));
		}

		/* get current value */
		obj1 = prop_dictionary_get(obj, "cur-value");
		if (obj1)
			sensor->cur_value = prop_number_integer_value(obj1);

		/* get max value */
		obj1 = prop_dictionary_get(obj, "max-value");
		if (obj1)
			sensor->max_value = prop_number_integer_value(obj1);

		/* get min value */
		obj1 = prop_dictionary_get(obj, "min-value");
		if (obj1)
			sensor->min_value = prop_number_integer_value(obj1);

		/* get avg value */
		obj1 = prop_dictionary_get(obj, "avg-value");
		if (obj1)
			sensor->avg_value = prop_number_integer_value(obj1);

		/* get percentage flag */
		obj1 = prop_dictionary_get(obj, "want-percentage");
		if (obj1)
			sensor->percentage = prop_bool_true(obj1);

		/* get critical max value if available */
		obj1 = prop_dictionary_get(obj, "critical-max");
		if (obj1)
			sensor->critmax_value = prop_number_integer_value(obj1);

		/* get critical min value if available */
		obj1 = prop_dictionary_get(obj, "critical-min");
		if (obj1)
			sensor->critmin_value = prop_number_integer_value(obj1);

		/* get critical capacity value if available */
		obj1 = prop_dictionary_get(obj, "critical-capacity");
		if (obj1)
			sensor->critcap_value = prop_number_integer_value(obj1);

		/* print sensor names if -l was given */
		if (flags & ENVSYS_LFLAG) {
			if (width)
				(void)printf("%*s\n", width,
				    prop_string_cstring_nocopy(desc));
			else
				(void)printf("%s\n",
				    prop_string_cstring_nocopy(desc));
		}

		/* Add the sensor into the list */
		SIMPLEQ_INSERT_TAIL(&sensors_list, sensor, entries);
	}

	/* free memory */
	prop_object_iterator_release(iter);
	return 0;
}

static int
check_sensors(char *str)
{
	sensor_t sensor = NULL;
	char *dvstring, *sstring, *p, *last;
	bool sensor_found = false;

	/*
	 * Parse device name and sensor description and find out
	 * if the sensor is valid.
	 */
	for ((p = strtok_r(str, ",", &last)); p;
	     (p = strtok_r(NULL, ",", &last))) {
		/* get device name */
		dvstring = strtok(p, ":");
		if (dvstring == NULL) {
			warnx("missing device name");
			return EINVAL;
		}

		/* get sensor description */
		sstring = strtok(NULL, ":");
		if (sstring == NULL) {
			warnx("missing sensor description");
			return EINVAL;
		}

		SIMPLEQ_FOREACH(sensor, &sensors_list, entries) {
			/* skip until we match device */
			if (strcmp(dvstring, sensor->dvname))
				continue;
			if (strcmp(sstring, sensor->desc) == 0) {
				sensor->visible = true;
				sensor_found = true;
				break;
			}
		}
		if (sensor_found == false) {
			warnx("unknown sensor `%s' for device `%s'",
		       	    sstring, dvstring);
			return EINVAL;
		}
		sensor_found = false;
	}

	/* check if all sensors were ok, and error out if not */
	SIMPLEQ_FOREACH(sensor, &sensors_list, entries)
		if (sensor->visible)
			return 0;

	warnx("no sensors selected to display");
	return EINVAL;
}

static void
print_sensors(void)
{
	sensor_t sensor;
	size_t maxlen = 0;
	double temp = 0;
	const char *invalid = "N/A", *degrees = NULL, *tmpstr = NULL;

	/* find the longest description */
	SIMPLEQ_FOREACH(sensor, &sensors_list, entries)
		if (strlen(sensor->desc) > maxlen)
			maxlen = strlen(sensor->desc);

	if (width)
		maxlen = width;

	/* print the sensors */
	SIMPLEQ_FOREACH(sensor, &sensors_list, entries) {
		/* skip sensors that were not marked as visible */
		if (sensors && !sensor->visible)
			continue;

		/* skip invalid sensors if -I is set */
		if ((flags & ENVSYS_IFLAG) && sensor->invalid)
			continue;

		/* print device name */
		if (!mydevname) {
			if (tmpstr == NULL || strcmp(tmpstr, sensor->dvname))
				printf("[%s]\n", sensor->dvname);

			tmpstr = sensor->dvname;
		}

		/* print sensor description */
		(void)printf("%s%*.*s", mydevname ? "" : "  ", (int)maxlen,
		    (int)maxlen, sensor->desc);

		/* print invalid string */
		if (sensor->invalid) {
			(void)printf(": %10s\n", invalid);
			continue;
		}

		/*
		 * Indicator and Battery charge sensors.
		 */
		if ((strcmp(sensor->type, "Indicator") == 0) ||
		    (strcmp(sensor->type, "Battery charge") == 0)) {

			(void)printf(": %10s", sensor->cur_value ? "ON" : "OFF");

/* converts the value to degC or degF */
#define CONVERTTEMP(a, b, c)					\
do {								\
	if (b) 							\
		(a) = ((b) / 1000000.0) - 273.15;		\
	if (flags & ENVSYS_FFLAG) {				\
		if (b)						\
			(a) = (9.0 / 5.0) * (a) + 32.0;		\
		(c) = "degF";					\
	} else							\
		(c) = "degC";					\
} while (/* CONSTCOND */ 0)


		/* temperatures */
		} else if (strcmp(sensor->type, "Temperature") == 0) {

			CONVERTTEMP(temp, sensor->cur_value, degrees);
			(void)printf(": %10.3f %s", temp, degrees);
			
			if (sensor->critmax_value || sensor->critmin_value)
				(void)printf("  ");

			if (sensor->critmax_value) {
				CONVERTTEMP(temp, sensor->critmax_value,
				    degrees);
				(void)printf("max: %8.3f %s  ", temp, degrees);
			}

			if (sensor->critmin_value) {
				CONVERTTEMP(temp, sensor->critmin_value,
				    degrees);
				(void)printf("min: %8.3f %s", temp, degrees);
			}
#undef CONVERTTEMP

		/* fans */
		} else if (strcmp(sensor->type, "Fan") == 0) {

			(void)printf(": %10u RPM", sensor->cur_value);

			if (sensor->critmax_value || sensor->critmin_value)
				(void)printf("   ");
			if (sensor->critmax_value)
				(void)printf("max: %8u RPM   ",
				    sensor->critmax_value);
			if (sensor->critmin_value)
				(void)printf("min: %8u RPM",
				    sensor->critmin_value);

		/* integers */
		} else if (strcmp(sensor->type, "Integer") == 0) {

			(void)printf(": %10d", sensor->cur_value);

		/* drives  */
		} else if (strcmp(sensor->type, "Drive") == 0) {

			(void)printf(": %10s", sensor->drvstate);

		/* Battery capacity */
		} else if (strcmp(sensor->type, "Battery capacity") == 0) {

			(void)printf(": %10s", sensor->battcap);

		/* everything else */
		} else {
			const char *type;

			if (strcmp(sensor->type, "Voltage DC") == 0)
				type = "V";
			else if (strcmp(sensor->type, "Voltage AC") == 0)
				type = "VAC";
			else if (strcmp(sensor->type, "Ampere") == 0)
				type = "A";
			else if (strcmp(sensor->type, "Watts") == 0)
				type = "W";
			else if (strcmp(sensor->type, "Ohms") == 0)
				type = "Ohms";
			else if (strcmp(sensor->type, "Watt hour") == 0)
				type = "Wh";
			else if (strcmp(sensor->type, "Ampere hour") == 0)
				type = "Ah";
			else
				type = NULL;

			(void)printf(": %10.3f %s",
			    sensor->cur_value / 1000000.0, type);

			if (sensor->percentage && sensor->max_value) {
				(void)printf(" (%5.2f%%)",
				    (sensor->cur_value * 100.0) /
				    sensor->max_value);
			}

			if (sensor->critcap_value) {
				(void)printf(" critical (%5.2f%%)",
				    (sensor->critcap_value * 100.0) /
				    sensor->max_value);
			}

			if (sensor->critmax_value || sensor->critmin_value)
				(void)printf("     ");
			if (sensor->critmax_value)
				(void)printf("max: %8.3f %s     ",
				    sensor->critmax_value / 1000000.0,
				    type);
			if (sensor->critmin_value)
				(void)printf("min: %8.3f %s",
				    sensor->critmin_value / 1000000.0,
				    type);

		}

		(void)printf("\n");
	}
}

static int
usage(void)
{
	(void)fprintf(stderr, "Usage: %s [-DfIlrSx] ", getprogname());
	(void)fprintf(stderr, "[-c file] [-d device] [-i interval] ");
	(void)fprintf(stderr, "[-s device:sensor,...] [-w width]\n");
	exit(EXIT_FAILURE);
	/* NOTREACHED */
}
