#ifndef lint
static char *rcsid = "$Id: ns32k.c,v 1.2 1994/10/06 15:40:09 mycroft Exp $";
#endif /* not lint */

#include "gprof.h"

/*
 * gprof -c isn't currently supported...
 */
findcall( parentp , p_lowpc , p_highpc )
    nltype		*parentp;
    unsigned long	p_lowpc;
    unsigned long	p_highpc;
{
}
