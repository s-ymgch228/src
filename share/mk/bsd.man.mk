#	$NetBSD: bsd.man.mk,v 1.15 1994/12/22 09:20:42 cgd Exp $
#	@(#)bsd.man.mk	5.2 (Berkeley) 5/11/90

.if !target(.MAIN)
.if exists(${.CURDIR}/../Makefile.inc)
.include "${.CURDIR}/../Makefile.inc"
.endif

.MAIN: all
.endif

.SUFFIXES: .1 .2 .3 .4 .5 .6 .7 .8 .cat1 .cat2 .cat3 .cat4 .cat5 .cat6 \
	.cat7 .cat8

.8.cat8 .7.cat7 .6.cat6 .5.cat5 .4.cat4 .3.cat3 .2.cat2 .1.cat1:
	@echo "nroff -mandoc ${.IMPSRC} > ${.TARGET}"
	@nroff -mandoc ${.IMPSRC} > ${.TARGET} || ( rm -f ${.TARGET} ; false )

MANALL=	${MAN:S/.1$/.cat1/g:S/.2$/.cat2/g:S/.3$/.cat3/g:S/.4$/.cat4/g:S/.5$/.cat5/g:S/.6$/.cat6/g:S/.7$/.cat7/g:S/.8$/.cat8/g}

MINSTALL=	install ${COPY} -o ${MANOWN} -g ${MANGRP} -m ${MANMODE}

maninstall:
.if defined(MAN) && !empty(MAN)
	@for page in ${MANALL}; do \
		dir=${DESTDIR}${MANDIR}`expr $$page : '.*\.cat\([1-8]\)'`; \
		instpage=`expr $$page : '\(.*\)\.cat[1-8]'`.0; \
		echo ${MINSTALL} $$page $$dir/$$instpage; \
		${MINSTALL} $$page $$dir/$$instpage; \
	done
.endif

.if defined(MLINKS) && !empty(MLINKS)
maninstall: manlinkinstall
manlinkinstall:
	@set ${MLINKS}; \
	while test $$# -ge 2; do \
		name=$$1; \
		shift; \
		dir=${DESTDIR}${MANDIR}`expr $$name : '.*\.\(.*\)'`; \
		l=$${dir}${MANSUBDIR}/`expr $$name : '\(.*\)\..*'`.0; \
		name=$$1; \
		shift; \
		dir=${DESTDIR}${MANDIR}`expr $$name : '.*\.\(.*\)'`; \
		t=$${dir}${MANSUBDIR}/`expr $$name : '\(.*\)\..*'`.0; \
		echo $$t -\> $$l; \
		rm -f $$t; \
		ln $$l $$t; \
	done; true
.endif

.if defined(MANALL)
all: ${MANALL}

cleandir: cleanman
cleanman:
	rm -f ${MANALL}
.endif
