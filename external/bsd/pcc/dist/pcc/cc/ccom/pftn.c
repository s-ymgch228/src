/*	Id: pftn.c,v 1.280 2010/05/23 19:52:04 ragge Exp 	*/	
/*	$NetBSD: pftn.c,v 1.5 2010/06/03 19:07:59 plunky Exp $	*/
/*
 * Copyright (c) 2003 Anders Magnusson (ragge@ludd.luth.se).
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
/*
 * Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code and documentation must retain the above
 * copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditionsand the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 * 	This product includes software developed or owned by Caldera
 *	International, Inc.
 * Neither the name of Caldera International, Inc. nor the names of other
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
 * INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE LIABLE
 * FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OFLIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Many changes from the 32V sources, among them:
 * - New symbol table manager (moved to another file).
 * - Prototype saving/checks.
 */

# include "pass1.h"

#include "cgram.h"

struct symtab *cftnsp;
int arglistcnt, dimfuncnt;	/* statistics */
int symtabcnt, suedefcnt;	/* statistics */
int autooff,		/* the next unused automatic offset */
    maxautooff,		/* highest used automatic offset in function */
    argoff;		/* the next unused argument offset */
int retlab = NOLAB;	/* return label for subroutine */
int brklab;
int contlab;
int flostat;
int blevel;
int reached, prolab;

struct params;

#define ISSTR(ty) (ty == STRTY || ty == UNIONTY)
#define ISSOU(ty) (ty == STRTY || ty == UNIONTY)
#define MKTY(p, t, d, s) r = talloc(); *r = *p; \
	r = argcast(r, t, d, s); *p = *r; nfree(r);

/*
 * Linked list stack while reading in structs.
 */
struct rstack {
	struct	rstack *rnext;
	int	rsou;
	int	rstr;
	struct	symtab *rsym;
	struct	symtab *rb;
	NODE	*rgp;
	int	flags;
#define	LASTELM	1
} *rpole;

/*
 * Linked list for parameter (and struct elements) declaration.
 */
static struct params {
	struct params *next, *prev;
	struct symtab *sym;
} *lpole, *lparam;
static int nparams;

/* defines used for getting things off of the initialization stack */

NODE *arrstk[10];
int arrstkp;
static int intcompare;
NODE *parlink;

void fixtype(NODE *p, int class);
int fixclass(int class, TWORD type);
static void dynalloc(struct symtab *p, int *poff);
void inforce(OFFSZ n);
void vfdalign(int n);
static void ssave(struct symtab *);
static void alprint(union arglist *al, int in);
static void lcommadd(struct symtab *sp);
static NODE *mkcmplx(NODE *p, TWORD dt);
extern int fun_inline;
struct suedef *sueget(struct suedef *p);

int ddebug = 0;

/*
 * Declaration of an identifier.  Handles redeclarations, hiding,
 * incomplete types and forward declarations.
 */

void
defid(NODE *ap, int class)
{
	struct symtab *p;
	TWORD type, qual;
	TWORD stp, stq;
	int scl;
	union dimfun *dsym, *ddef;
	int slev, temp, changed;
	NODE *q = ap;

	if (q == NIL)
		return;  /* an error was detected */

#ifdef GCC_COMPAT
	if (q->n_op == CM)
		q = q->n_left;
#endif
	p = q->n_sp;

	if (p->sname == NULL)
		cerror("defining null identifier");

#ifdef PCC_DEBUG
	if (ddebug) {
		printf("defid(%s (%p), ", p->sname, p);
		tprint(stdout, q->n_type, q->n_qual);
		printf(", %s, (%p,%p)), level %d\n", scnames(class),
		    q->n_df, q->n_sue, blevel);
	}
#endif

	fixtype(q, class);

	type = q->n_type;
	qual = q->n_qual;
	class = fixclass(class, type);

	stp = p->stype;
	stq = p->squal;
	slev = p->slevel;

#ifdef PCC_DEBUG
	if (ddebug) {
		printf("	modified to ");
		tprint(stdout, type, qual);
		printf(", %s\n", scnames(class));
		printf("	previous def'n: ");
		tprint(stdout, stp, stq);
		printf(", %s, (%p,%p)), level %d\n",
		    scnames(p->sclass), p->sdf, p->ssue, slev);
	}
#endif

	if (blevel == 1) {
		switch (class) {
		default:
			if (!(class&FIELD) && !ISFTN(type))
				uerror("declared argument %s missing",
				    p->sname );
		case MOS:
		case MOU:
		case TYPEDEF:
		case PARAM:
			;
		}
	}

	if (stp == UNDEF)
		goto enter; /* New symbol */

	if (type != stp)
		goto mismatch;

	if (blevel > slev && (class == AUTO || class == REGISTER))
		/* new scope */
		goto mismatch;

	/*
	 * test (and possibly adjust) dimensions.
	 * also check that prototypes are correct.
	 */
	dsym = p->sdf;
	ddef = q->n_df;
	changed = 0;
	for (temp = type; temp & TMASK; temp = DECREF(temp)) {
		if (ISARY(temp)) {
			if (dsym->ddim == NOOFFSET) {
				dsym->ddim = ddef->ddim;
				changed = 1;
			} else if (ddef->ddim != NOOFFSET &&
			    dsym->ddim!=ddef->ddim) {
				goto mismatch;
			}
			++dsym;
			++ddef;
		} else if (ISFTN(temp)) {
			/* add a late-defined prototype here */
			if (cftnsp == NULL && dsym->dfun == NULL)
				dsym->dfun = ddef->dfun;
			if (!oldstyle && ddef->dfun != NULL &&
			    chkftn(dsym->dfun, ddef->dfun))
				uerror("declaration doesn't match prototype");
			dsym++, ddef++;
		}
	}
#ifdef STABS
	if (changed && gflag)
		stabs_chgsym(p); /* symbol changed */
#endif

	/* check that redeclarations are to the same structure */
	if (temp == STRTY || temp == UNIONTY) {
		struct suedef *sue1, *sue2;
		GETSUE(sue1, p->ssue);
		GETSUE(sue2, q->n_sue);
		if (sue1 != sue2)
			goto mismatch;
	}

	scl = p->sclass;

#ifdef PCC_DEBUG
	if (ddebug)
		printf("	previous class: %s\n", scnames(scl));
#endif

#ifdef GCC_COMPAT
	/* Its allowed to add attributes to existing declarations */
	if (ap != q) {
		p->ssue = sueget(p->ssue);
                p->ssue->suega = gcc_attr_parse(ap->n_right);
                ap->n_right = bcon(0);
        }
#endif

	if (class & FIELD)
		return;
	switch(class) {

	case EXTERN:
		switch( scl ){
		case STATIC:
		case USTATIC:
			if( slev==0 )
				goto done;
			break;
		case EXTDEF:
		case EXTERN:
		case FORTRAN:
		case UFORTRAN:
			goto done;
		case SNULL:
			if (p->sflags & SINLINE) {
				p->sclass = EXTDEF;
				inline_ref(p);
				goto done;
			}
			break;
		}
		break;

	case STATIC:
		if (scl==USTATIC || (scl==EXTERN && blevel==0)) {
			p->sclass = STATIC;
			goto done;
		}
		if (changed || (scl == STATIC && blevel == slev))
			goto done; /* identical redeclaration */
		break;

	case USTATIC:
		if (scl==STATIC || scl==USTATIC)
			goto done;
		break;

	case TYPEDEF:
		if (scl == class)
			goto done;
		break;

	case UFORTRAN:
		if (scl == UFORTRAN || scl == FORTRAN)
			goto done;
		break;

	case FORTRAN:
		if (scl == UFORTRAN) {
			p->sclass = FORTRAN;
			goto done;
		}
		break;

	case MOU:
	case MOS:
		goto done;

	case EXTDEF:
		switch (scl) {
		case EXTERN:
			p->sclass = EXTDEF;
			goto done;
		case USTATIC:
			p->sclass = STATIC;
			goto done;
		case SNULL:
			/*
			 * Handle redeclarations of inlined functions.
			 * This is allowed if the previous declaration is of
			 * type gnu_inline.
			 */
			if (gcc_get_attr(p->ssue, GCC_ATYP_GNU_INLINE))
				goto done;
			break;
		}
		break;

	case AUTO:
	case REGISTER:
		if (blevel == slev)
			goto redec;
		break;  /* mismatch.. */
	case SNULL:
		if (fun_inline && ISFTN(type))
			goto done;
		break;
	}

	mismatch:

	/*
	 * Only allowed for automatic variables.
	 */
	if (blevel == slev || class == EXTERN || class == FORTRAN ||
	    class == UFORTRAN) {
		if (ISSTR(class) && !ISSTR(p->sclass)) {
redec:			uerror("redeclaration of %s", p->sname);
			return;
		}
	}
	if (blevel == 0)
		goto redec;
	q->n_sp = p = hide(p);

	enter:  /* make a new entry */

#ifdef PCC_DEBUG
	if(ddebug)
		printf("	new entry made\n");
#endif
	p->stype = type;
	p->squal = qual;
	p->sclass = (char)class;
	p->slevel = (char)blevel;
	p->soffset = NOOFFSET;
	if (q->n_sue == NULL)
		cerror("q->n_sue == NULL");
#ifdef GCC_COMPAT
	if (ap != q) {
		struct gcc_attrib *ga;
		struct suedef *sue;

		sue = q->n_sue = sueget(q->n_sue);
		sue->suega = gcc_attr_parse(ap->n_right);
		if ((ga = gcc_get_attr(sue, GCC_ATYP_ALIGNED))) {
			sue->suealign = ga->a1.iarg;
			SETOFF(sue->suesize, sue->suealign);
		} else if ((ga = gcc_get_attr(sue, GCC_ATYP_MODE))) {
			if (ga->a1.iarg)
				p->stype = ga->a1.iarg;
		}
		ap->n_right = bcon(0);
	}
#endif
	p->ssue = q->n_sue;

	/* copy dimensions */
	p->sdf = q->n_df;
	/* Do not save param info for old-style functions */
	if (ISFTN(type) && oldstyle)
		p->sdf->dfun = NULL;

	/* allocate offsets */
	if (class&FIELD) {
		(void) falloc(p, class&FLDSIZ, NIL);  /* new entry */
	} else switch (class) {

	case REGISTER:
		cerror("register var");

	case AUTO:
		if (arrstkp)
			dynalloc(p, &autooff);
		else
			oalloc(p, &autooff);
		break;
	case PARAM:
		if (arrstkp) {
			dynalloc(p, &argoff);
		} else {
			if (ISARY(p->stype)) {
			/* remove array type on parameters before oalloc */
				p->stype += (PTR-ARY);
				p->sdf++;
			}
			oalloc(p, &argoff);
		}
		break;
		
	case STATIC:
	case EXTDEF:
	case EXTERN:
	case UFORTRAN:
	case FORTRAN:
		p->soffset = getlab();
		if (pragma_renamed)
			p->soname = pragma_renamed;
		pragma_renamed = NULL;
		break;

	case MOU:
		rpole->rstr = 0;
		/* FALLTHROUGH */
	case MOS:
		oalloc(p, &rpole->rstr);
		if (class == MOU)
			rpole->rstr = 0;
		break;
	case SNULL:
#ifdef notdef
		if (fun_inline) {
			p->slevel = 1;
			p->soffset = getlab();
		}
#endif
		break;
	}

#ifdef STABS
	if (gflag)
		stabs_newsym(p);
#endif

done:
	fixdef(p);	/* Leave last word to target */
#ifdef PCC_DEBUG
	if (ddebug)
		printf( "	sdf, ssue, offset: %p, %p, %d\n",
		    p->sdf, p->ssue, p->soffset);
#endif
}

void
ssave(struct symtab *sym)
{
	struct params *p;

	p = tmpalloc(sizeof(struct params));
	p->next = NULL;
	p->sym = sym;

	if ((p->prev = lparam) == NULL)
		lpole = p;
	else
		lparam->next = p;
	lparam = p;
}

/*
 * end of function
 */
void
ftnend()
{
	extern NODE *cftnod;
	extern struct savbc *savbc;
	extern struct swdef *swpole;
	extern int tvaloff;
	char *c;

	if (retlab != NOLAB && nerrors == 0) { /* inside a real function */
		plabel(retlab);
		if (cftnod)
			ecomp(buildtree(FORCE, cftnod, NIL));
		efcode(); /* struct return handled here */
		if ((c = cftnsp->soname) == NULL)
			c = addname(exname(cftnsp->sname));
		SETOFF(maxautooff, ALCHAR);
		send_passt(IP_EPILOG, maxautooff/SZCHAR, c,
		    cftnsp->stype, cftnsp->sclass == EXTDEF, retlab, tvaloff);
	}

	cftnod = NIL;
	tcheck();
	brklab = contlab = retlab = NOLAB;
	flostat = 0;
	if (nerrors == 0) {
		if (savbc != NULL)
			cerror("bcsave error");
		if (lparam != NULL)
			cerror("parameter reset error");
		if (swpole != NULL)
			cerror("switch error");
	}
	savbc = NULL;
	lparam = NULL;
	cftnsp = NULL;
	maxautooff = autooff = AUTOINIT;
	reached = 1;

	if (isinlining)
		inline_end();
	inline_prtout();

	tmpfree(); /* Release memory resources */
}

static struct symtab nulsym = {
	NULL, 0, 0, 0, 0, "null", "null", INT, 0, NULL, NULL
};

void
dclargs()
{
	union dimfun *df;
	union arglist *al, *al2, *alb;
	struct params *a;
	struct symtab *p, **parr = NULL; /* XXX gcc */
	int i;

	/*
	 * Deal with fun(void) properly.
	 */
	if (nparams == 1 && lparam->sym && lparam->sym->stype == VOID)
		goto done;

	/*
	 * Generate a list for bfcode().
	 * Parameters were pushed in reverse order.
	 */
	if (nparams != 0)
		parr = tmpalloc(sizeof(struct symtab *) * nparams);

	if (nparams)
	    for (a = lparam, i = 0; a != NULL; a = a->prev) {
		p = a->sym;
		parr[i++] = p;
		if (p == NULL) {
			uerror("parameter %d name missing", i);
			p = &nulsym; /* empty symtab */
		}
		if (p->stype == FARG) {
			p->stype = INT;
			p->ssue = MKSUE(INT);
		}
		if (ISARY(p->stype)) {
			p->stype += (PTR-ARY);
			p->sdf++;
		} else if (ISFTN(p->stype)) {
			werror("function declared as argument");
			p->stype = INCREF(p->stype);
		}
#ifdef STABS
		if (gflag)
			stabs_newsym(p);
#endif
	}
	if (oldstyle && (df = cftnsp->sdf) && (al = df->dfun)) {
		/*
		 * Check against prototype of oldstyle function.
		 */
		alb = al2 = tmpalloc(sizeof(union arglist) * nparams * 3 + 1);
		for (i = 0; i < nparams; i++) {
			TWORD type = parr[i]->stype;
			(al2++)->type = type;
			if (ISSTR(BTYPE(type)))
				(al2++)->sue = parr[i]->ssue;
			while (!ISFTN(type) && !ISARY(type) && type > BTMASK)
				type = DECREF(type);
			if (type > BTMASK)
				(al2++)->df = parr[i]->sdf;
		}
		al2->type = TNULL;
		intcompare = 1;
		if (chkftn(al, alb))
			uerror("function doesn't match prototype");
		intcompare = 0;

	}

	if (oldstyle && nparams) {
		/* Must recalculate offset for oldstyle args here */
		argoff = ARGINIT;
		for (i = 0; i < nparams; i++) {
			parr[i]->soffset = NOOFFSET;
			oalloc(parr[i], &argoff);
		}
	}

done:	cendarg();

	plabel(prolab); /* after prolog, used in optimization */
	retlab = getlab();
	bfcode(parr, nparams);
	if (fun_inline && xinline)
		inline_args(parr, nparams);
	plabel(getlab()); /* used when spilling */
	if (parlink)
		ecomp(parlink);
	parlink = NIL;
	lparam = NULL;
	nparams = 0;
	symclear(1);	/* In case of function pointer args */
}

/*
 * Alloc sue from either perm or tmp memory, depending on blevel.
 */
struct suedef *
sueget(struct suedef *p)
{
	struct suedef *sue;

	if (blevel == 0) {
		sue = permalloc(sizeof(struct suedef));
		suedefcnt++;
	} else
		sue = tmpalloc(sizeof(struct suedef));
	sue = memset(sue, 0, sizeof(struct suedef));
	sue->suep = p;
	return sue;
}

/*
 * Struct/union/enum symtab construction.
 */
static void
defstr(struct symtab *sp, int class)
{
	sp->ssue = permalloc(sizeof(struct suedef));
	memset(sp->ssue, 0, sizeof(struct suedef));
	sp->sclass = (char)class;
	if (class == STNAME)
		sp->stype = STRTY;
	else if (class == UNAME)
		sp->stype = UNIONTY;
	else if (class == ENAME)
		sp->stype = ENUMTY;
}

/*
 * Declare a struct/union/enum tag.
 * If not found, create a new tag with UNDEF type.
 */
static struct symtab *
deftag(char *name, int class)
{
	struct symtab *sp;

	if ((sp = lookup(name, STAGNAME))->ssue == NULL) {
		/* New tag */
		defstr(sp, class);
	} else if (sp->sclass != class)
		uerror("tag %s redeclared", name);
	return sp;
}

/*
 * reference to a structure or union, with no definition
 */
NODE *
rstruct(char *tag, int soru)
{
	struct symtab *sp;

	sp = deftag(tag, soru);
	return mkty(sp->stype, 0, sp->ssue);
}

static int enumlow, enumhigh;
int enummer;

/*
 * Declare a member of enum.
 */
void
moedef(char *name)
{
	struct symtab *sp;

	sp = lookup(name, SNORMAL);
	if (sp->stype == UNDEF || (sp->slevel < blevel)) {
		if (sp->stype != UNDEF)
			sp = hide(sp);
		sp->stype = INT; /* always */
		sp->ssue = MKSUE(INT);
		sp->sclass = MOE;
		sp->soffset = enummer;
	} else
		uerror("%s redeclared", name);
	if (enummer < enumlow)
		enumlow = enummer;
	if (enummer > enumhigh)
		enumhigh = enummer;
	enummer++;
}

/*
 * Declare an enum tag.  Complain if already defined.
 */
struct symtab *
enumhd(char *name)
{
	struct symtab *sp;

	enummer = enumlow = enumhigh = 0;
	if (name == NULL)
		return NULL;

	sp = deftag(name, ENAME);
	if (sp->stype != ENUMTY) {
		if (sp->slevel == blevel)
			uerror("%s redeclared", name);
		sp = hide(sp);
		defstr(sp, ENAME);
	}
	sp->ssue->suem = sp;	/* ourselves XXX */
	return sp;
}

/*
 * finish declaration of an enum
 */
NODE *
enumdcl(struct symtab *sp)
{
	struct suedef *sue;
	NODE *p;
	TWORD t;

#ifdef ENUMSIZE
	t = ENUMSIZE(enumhigh, enumlow);
#else
	if (enumhigh <= MAX_CHAR && enumlow >= MIN_CHAR)
		t = ctype(CHAR);
	else if (enumhigh <= MAX_SHORT && enumlow >= MIN_SHORT)
		t = ctype(SHORT);
	else
		t = ctype(INT);
#endif
	if (sp) {
		sp->stype = t;
		sue = sp->ssue;
		sue->suesize = (MKSUE(t))->suesize;
		sue->suealign = (MKSUE(t))->suealign;
	} else
		sue = MKSUE(t);
	p = mkty(t, 0, sue);
	p->n_sp = sp;
	return p;
}

/*
 * Handle reference to an enum
 */
NODE *
enumref(char *name)
{
	struct symtab *sp;
	NODE *p;

	sp = lookup(name, STAGNAME);

#ifdef notdef
	/*
	 * 6.7.2.3 Clause 2:
	 * "A type specifier of the form 'enum identifier' without an
	 *  enumerator list shall only appear after the type it specifies
	 *  is complete."
	 */
	if (sp->sclass != ENAME)
		uerror("enum %s undeclared", name);
#endif
	if (sp->sclass == SNULL) {
		/* declare existence of enum */
		sp = enumhd(name);
		sp->stype = ENUMTY;
	}

	p = mkty(sp->stype, 0, sp->ssue);
	p->n_sp = sp;
	return p;
}

/*
 * begining of structure or union declaration
 */
struct rstack *
bstruct(char *name, int soru, NODE *gp)
{
	struct rstack *r;
	struct symtab *sp;

	if (name != NULL) {
		sp = deftag(name, soru);
		if (sp->ssue->suealign != 0) {
			if (sp->slevel < blevel) {
				sp = hide(sp);
				defstr(sp, soru);
			} else
				uerror("%s redeclared", name);
		}
	} else
		sp = NULL;

	r = tmpcalloc(sizeof(struct rstack));
	r->rsou = soru;
	r->rsym = sp;
	r->rb = NULL;
	r->rgp = gp;
	r->rnext = rpole;
	rpole = r;

	return r;
}

/*
 * Called after a struct is declared to restore the environment.
 * - If ALSTRUCT is defined, this will be the struct alignment and the
 *   struct size will be a multiple of ALSTRUCT, otherwise it will use
 *   the alignment of the largest struct member.
 */
NODE *
dclstruct(struct rstack *r)
{
	NODE *n;
	struct suedef *sue;
	struct symtab *sp;
	int al, sa, sz, coff;

	sue = r->rsym ? r->rsym->ssue : sueget(NULL);

#ifdef ALSTRUCT
	al = ALSTRUCT;
#else
	al = ALCHAR;
#endif

	/*
	 * extract size and alignment, recalculate offsets
	 * if struct should be packed.
	 */
	coff = 0;
	sue->suem = r->rb;
	for (sp = r->rb; sp; sp = sp->snext) {
		sa = talign(sp->stype, sp->ssue);
		if (sp->sclass & FIELD)
			sz = sp->sclass&FLDSIZ;
		else
			sz = (int)tsize(sp->stype, sp->sdf, sp->ssue);
		if (sz > rpole->rstr)
			rpole->rstr = sz;  /* for use with unions */
		/*
		 * set al, the alignment, to the lcm of the alignments
		 * of the members.
		 */
		SETOFF(al, sa);
	}

	SETOFF(rpole->rstr, al);

	sue->suesize = rpole->rstr;
	sue->suealign = al;

#ifdef PCC_DEBUG
	if (ddebug) {
		printf("dclstruct(%s): size=%d, align=%d\n",
		    r->rsym ? r->rsym->sname : "??",
		    sue->suesize, sue->suealign);
	}
	if (ddebug>1) {
		printf("\tsize %d align %d link %p\n",
		    sue->suesize, sue->suealign, sue->suem);
		for (sp = sue->suem; sp != NULL; sp = sp->snext) {
			printf("\tmember %s(%p)\n", sp->sname, sp);
		}
	}
#endif

#ifdef STABS
	if (gflag)
		stabs_struct(r->rsym, sue);
#endif

	rpole = r->rnext;
	n = mkty(r->rsou == STNAME ? STRTY : UNIONTY, 0, sue);
	n->n_qual |= 1; /* definition place */
	n->n_left = r->rgp;
	return n;
}

/*
 * Add a new member to the current struct or union being declared.
 */
void
soumemb(NODE *n, char *name, int class)
{
	struct symtab *sp, *lsp;
	int incomp;
 
	if (rpole == NULL)
		cerror("soumemb");
 
	/* check if tag name exists */
	lsp = NULL;
	for (sp = rpole->rb; sp != NULL; lsp = sp, sp = sp->snext)
		if (*name != '*' && sp->sname == name)
			uerror("redeclaration of %s", name);

	sp = getsymtab(name, SMOSNAME);
	if (rpole->rb == NULL)
		rpole->rb = sp;
	else
		lsp->snext = sp;
	n->n_sp = sp;
	if ((class & FIELD) == 0)
		class = rpole->rsou == STNAME ? MOS : MOU;
	defid(n, class);

	/*
	 * 6.7.2.1 clause 16:
	 * "...the last member of a structure with more than one
	 *  named member may have incomplete array type;"
	 */
	if (ISARY(sp->stype) && sp->sdf->ddim == NOOFFSET)
		incomp = 1;
	else
		incomp = 0;
	if ((rpole->flags & LASTELM) || (rpole->rb == sp && incomp == 1))
		uerror("incomplete array in struct");
	if (incomp == 1)
		rpole->flags |= LASTELM;

	/*
	 * 6.7.2.1 clause 2:
	 * "...such a structure shall not be a member of a structure
	 *  or an element of an array."
	 */
	if (rpole->rsou == STNAME && sp->ssue->suem && !ISPTR(sp->stype)) {
		struct symtab *lnk;

		for (lnk = sp->ssue->suem; lnk->snext; lnk = lnk->snext)
			;
		if (ISARY(lnk->stype) && lnk->sdf->ddim == NOOFFSET)
			uerror("incomplete struct in struct");
	}
}

/*
 * error printing routine in parser
 */
void yyerror(char *s);
void
yyerror(char *s)
{
	uerror(s);
}

void yyaccpt(void);
void
yyaccpt(void)
{
	ftnend();
}

/*
 * p is top of type list given to tymerge later.
 * Find correct CALL node and declare parameters from there.
 */
void
ftnarg(NODE *p)
{
	NODE *q;

#ifdef PCC_DEBUG
	if (ddebug > 2)
		printf("ftnarg(%p)\n", p);
#endif
	/*
	 * Push argument symtab entries onto param stack in reverse order,
	 * due to the nature of the stack it will be reclaimed correct.
	 */
	for (; p->n_op != NAME; p = p->n_left) {
		if (p->n_op == UCALL && p->n_left->n_op == NAME)
			return;	/* Nothing to enter */
		if (p->n_op == CALL && p->n_left->n_op == NAME)
			break;
	}

	p = p->n_right;
	while (p->n_op == CM) {
		q = p->n_right;
		if (q->n_op != ELLIPSIS) {
			ssave(q->n_sp);
			nparams++;
#ifdef PCC_DEBUG
			if (ddebug > 2)
				printf("	saving sym %s (%p) from (%p)\n",
				    q->n_sp->sname, q->n_sp, q);
#endif
		}
		p = p->n_left;
	}
	ssave(p->n_sp);
	if (p->n_type != VOID)
		nparams++;

#ifdef PCC_DEBUG
	if (ddebug > 2)
		printf("	saving sym %s (%p) from (%p)\n",
		    nparams ? p->n_sp->sname : "<noname>", p->n_sp, p);
#endif
}

/*
 * compute the alignment of an object with type ty, sizeoff index s
 */
int
talign(unsigned int ty, struct suedef *sue)
{
	int i;

	if (ISPTR(ty))
		return(ALPOINT); /* shortcut */

	if(sue == NULL && ty!=INT && ty!=CHAR && ty!=SHORT &&
	    ty!=UNSIGNED && ty!=UCHAR && ty!=USHORT) {
		return(fldal(ty));
	}

	for( i=0; i<=(SZINT-BTSHIFT-1); i+=TSHIFT ){
		switch( (ty>>i)&TMASK ){

		case PTR:
			return(ALPOINT);
		case ARY:
			continue;
		case FTN:
			cerror("compiler takes alignment of function");
		case 0:
			break;
			}
		}

	GETSUE(sue, sue)
		if (sue->suealign)
			break;
	if (sue->suealign == 0)
		uerror("no alignment");
	return sue->suealign;
}

/* compute the size associated with type ty,
 *  dimoff d, and sizoff s */
/* BETTER NOT BE CALLED WHEN t, d, and s REFER TO A BIT FIELD... */
OFFSZ
tsize(TWORD ty, union dimfun *d, struct suedef *sue)
{

	int i;
	OFFSZ mult, sz;

	mult = 1;

	for( i=0; i<=(SZINT-BTSHIFT-1); i+=TSHIFT ){
		switch( (ty>>i)&TMASK ){

		case FTN:
			uerror( "cannot take size of function");
		case PTR:
			return( SZPOINT(ty) * mult );
		case ARY:
			if (d->ddim == NOOFFSET)
				return 0;
			mult *= d->ddim;
			d++;
			continue;
		case 0:
			break;

			}
		}

	if (sue == NULL)
		cerror("bad tsize sue");
	GETSUE(sue, sue);
	sz = sue->suesize;
#ifdef GCC_COMPAT
	if (ty == VOID)
		sz = SZCHAR;
#endif
	if (!ISSOU(BTYPE(ty))) {
		if (sz == 0) {
			uerror("unknown size");
			return(SZINT);
		}
	} else {
		if (sue->suealign == 0)
			uerror("unknown structure/union/enum");
	}

	return((unsigned int)sz * mult);
}

/*
 * Save string (and print it out).  If wide then wide string.
 */
NODE *
strend(int wide, char *str)
{
	struct symtab *sp;
	NODE *p;

	/* If an identical string is already emitted, just forget this one */
	if (wide) {
		/* Do not save wide strings, at least not now */
		sp = getsymtab(str, SSTRING|STEMP);
	} else {
		str = addstring(str);	/* enter string in string table */
		sp = lookup(str, SSTRING);	/* check for existance */
	}

	if (sp->soffset == 0) { /* No string */
		char *wr;
		int i;

		sp->sclass = STATIC;
		sp->slevel = 1;
		sp->soffset = getlab();
		sp->squal = (CON >> TSHIFT);
		sp->sdf = permalloc(sizeof(union dimfun));
		if (wide) {
			sp->stype = WCHAR_TYPE+ARY;
			sp->ssue = MKSUE(WCHAR_TYPE);
		} else {
			if (funsigned_char) {
				sp->stype = UCHAR+ARY;
				sp->ssue = MKSUE(UCHAR);
			} else {
				sp->stype = CHAR+ARY;
				sp->ssue = MKSUE(CHAR);
			}
		}
		for (wr = sp->sname, i = 1; *wr; i++)
			if (*wr++ == '\\')
				(void)esccon(&wr);

		sp->sdf->ddim = i;
		if (wide)
			inwstring(sp);
		else
			instring(sp);
	}

	p = block(NAME, NIL, NIL, sp->stype, sp->sdf, sp->ssue);
	p->n_sp = sp;
	return(clocal(p));
}

/*
 * Print out a wide string by calling ninval().
 */
void
inwstring(struct symtab *sp)
{
	char *s = sp->sname;
	NODE *p;

	defloc(sp);
	p = xbcon(0, NULL, WCHAR_TYPE);
	do {
		if (*s++ == '\\')
			p->n_lval = esccon(&s);
		else
			p->n_lval = (unsigned char)s[-1];
		ninval(0, (MKSUE(WCHAR_TYPE))->suesize, p);
	} while (s[-1] != 0);
	nfree(p);
}

/*
 * update the offset pointed to by poff; return the
 * offset of a value of size `size', alignment `alignment',
 * given that off is increasing
 */
int
upoff(int size, int alignment, int *poff)
{
	int off;

	off = *poff;
	SETOFF(off, alignment);
	if (off < 0)
		cerror("structure or stack overgrown"); /* wrapped */
	*poff = off+size;
	return (off);
}

/*
 * allocate p with offset *poff, and update *poff
 */
int
oalloc(struct symtab *p, int *poff )
{
	int al, off, tsz;
	int noff;

	/*
	 * Only generate tempnodes if we are optimizing,
	 * and only for integers, floats or pointers,
	 * and not if the type on this level is volatile.
	 */
	if (xtemps && ((p->sclass == AUTO) || (p->sclass == REGISTER)) &&
	    (p->stype < STRTY || ISPTR(p->stype)) &&
	    !(cqual(p->stype, p->squal) & VOL) && cisreg(p->stype)) {
		NODE *tn = tempnode(0, p->stype, p->sdf, p->ssue);
		p->soffset = regno(tn);
		p->sflags |= STNODE;
		nfree(tn);
		return 0;
	}

	al = talign(p->stype, p->ssue);
	noff = off = *poff;
	tsz = (int)tsize(p->stype, p->sdf, p->ssue);
#ifdef BACKAUTO
	if (p->sclass == AUTO) {
		noff = off + tsz;
		if (noff < 0)
			cerror("stack overflow");
		SETOFF(noff, al);
		off = -noff;
	} else
#endif
	if (p->sclass == PARAM && (p->stype == CHAR || p->stype == UCHAR ||
	    p->stype == SHORT || p->stype == USHORT || p->stype == BOOL)) {
		off = upoff(SZINT, ALINT, &noff);
#ifndef RTOLBYTES
		off = noff - tsz;
#endif
	} else {
		off = upoff(tsz, al, &noff);
	}

	if (p->sclass != REGISTER) {
	/* in case we are allocating stack space for register arguments */
		if (p->soffset == NOOFFSET)
			p->soffset = off;
		else if(off != p->soffset)
			return(1);
	}

	*poff = noff;
	return(0);
}

/*
 * Delay emission of code generated in argument headers.
 */
static void
edelay(NODE *p)
{
	if (blevel == 1) {
		/* Delay until after declarations */
		if (parlink == NULL)
			parlink = p;
		else
			parlink = block(COMOP, parlink, p, 0, 0, 0);
	} else
		ecomp(p);
}

/*
 * Allocate space on the stack for dynamic arrays (or at least keep track
 * of the index).
 * Strategy is as follows:
 * - first entry is a pointer to the dynamic datatype.
 * - if it's a one-dimensional array this will be the only entry used.
 * - if it's a multi-dimensional array the following (numdim-1) integers
 *   will contain the sizes to multiply the indexes with.
 * - code to write the dimension sizes this will be generated here.
 * - code to allocate space on the stack will be generated here.
 */
static void
dynalloc(struct symtab *p, int *poff)
{
	union dimfun *df;
	NODE *n, *nn, *tn, *pol;
	TWORD t;
	int astkp, no;

	/*
	 * The pointer to the array is not necessarily stored in a
	 * TEMP node, but if it is, its number is in the soffset field;
	 */
	t = p->stype;
	astkp = 0;
	if (ISARY(t) && blevel == 1) {
		/* must take care of side effects of dynamic arg arrays */
		if (p->sdf->ddim < 0 && p->sdf->ddim != NOOFFSET) {
			/* first-level array will be indexed correct */
			edelay(arrstk[astkp++]);
		}
		p->sdf++;
		p->stype += (PTR-ARY);
		t = p->stype;
	}
	if (ISARY(t)) {
		p->sflags |= (STNODE|SDYNARRAY);
		p->stype = INCREF(p->stype); /* Make this an indirect pointer */
		tn = tempnode(0, p->stype, p->sdf, p->ssue);
		p->soffset = regno(tn);
	} else {
		oalloc(p, poff);
		tn = NIL;
	}

	df = p->sdf;

	pol = NIL;
	for (; t > BTMASK; t = DECREF(t)) {
		if (!ISARY(t))
			continue;
		if (df->ddim < 0) {
			n = arrstk[astkp++];
			do {
				nn = tempnode(0, INT, 0, MKSUE(INT));
				no = regno(nn);
			} while (no == -NOOFFSET);
			edelay(buildtree(ASSIGN, nn, n));

			df->ddim = -no;
			n = tempnode(no, INT, 0, MKSUE(INT));
		} else
			n = bcon(df->ddim);

		pol = (pol == NIL ? n : buildtree(MUL, pol, n));
		df++;
	}
	/* Create stack gap */
	if (blevel == 1) {
		if (tn)
			tfree(tn);
		if (pol)
			tfree(pol);
	} else {
		if (pol == NIL)
			uerror("aggregate dynamic array not allowed");
		if (tn)
			spalloc(tn, pol, tsize(t, 0, p->ssue));
	}
	arrstkp = 0;
}

/*
 * allocate a field of width w
 * new is 0 if new entry, 1 if redefinition, -1 if alignment
 */
int
falloc(struct symtab *p, int w, NODE *pty)
{
	int al,sz,type;

	type = p ? p->stype : pty->n_type;

	if (type < CHAR || type > ULONGLONG) {
		uerror("illegal field type");
		type = INT;
	}

	al = btdims[type].suealign;
	sz = btdims[type].suesize;

	if (w > sz) {
		uerror("field too big");
		w = sz;
	}

	if (w == 0) { /* align only */
		SETOFF(rpole->rstr, al);
		if (p != NULL)
			uerror("zero size field");
		return(0);
	}

	if (rpole->rstr%al + w > sz)
		SETOFF(rpole->rstr, al);
	if (p == NULL) {
		rpole->rstr += w;  /* we know it will fit */
		return(0);
	}

	/* establish the field */

	p->soffset = rpole->rstr;
	rpole->rstr += w;
	p->stype = type;
	fldty(p);
	return(0);
}

/*
 * handle unitialized declarations assumed to be not functions:
 * int a;
 * extern int a;
 * static int a;
 */
void
nidcl(NODE *p, int class)
{
	struct symtab *sp;
	int commflag = 0;

	/* compute class */
	if (class == SNULL) {
		if (blevel > 1)
			class = AUTO;
		else if (blevel != 0 || rpole)
			cerror( "nidcl error" );
		else /* blevel = 0 */
			commflag = 1, class = EXTERN;
	}

	defid(p, class);

#ifdef GCC_COMPAT
	if (p->n_op == CM)
		p = p->n_left;
#endif

	sp = p->n_sp;
	/* check if forward decl */
	if (ISARY(sp->stype) && sp->sdf->ddim == NOOFFSET)
		return;

	if (sp->sflags & SASG)
		return; /* already initialized */

	switch (class) {
	case EXTDEF:
		/* simulate initialization by 0 */
		simpleinit(p->n_sp, bcon(0));
		break;
	case EXTERN:
		if (commflag)
			lcommadd(p->n_sp);
		else
			extdec(p->n_sp);
		break;
	case STATIC:
		if (blevel == 0)
			lcommadd(p->n_sp);
		else
			defzero(p->n_sp);
		break;
	}
}

struct lcd {
	SLIST_ENTRY(lcd) next;
	struct symtab *sp;
};

static SLIST_HEAD(, lcd) lhead = { NULL, &lhead.q_forw};

/*
 * Add a local common statement to the printout list.
 */
void
lcommadd(struct symtab *sp)
{
	struct lcd *lc, *lcp;

	lcp = NULL;
	SLIST_FOREACH(lc, &lhead, next) {
		if (lc->sp == sp)
			return; /* already exists */
		if (lc->sp == NULL && lcp == NULL)
			lcp = lc;
	}
	if (lcp == NULL) {
		lc = permalloc(sizeof(struct lcd));
		lc->sp = sp;
		SLIST_INSERT_LAST(&lhead, lc, next);
	} else
		lcp->sp = sp;
}

/*
 * Delete a local common statement.
 */
void
lcommdel(struct symtab *sp)
{
	struct lcd *lc;

	SLIST_FOREACH(lc, &lhead, next) {
		if (lc->sp == sp) {
			lc->sp = NULL;
			return;
		}
	}
}

/*
 * Print out the remaining common statements.
 */
void
lcommprint(void)
{
	struct lcd *lc;

	SLIST_FOREACH(lc, &lhead, next) {
		if (lc->sp != NULL)
			defzero(lc->sp);
	}
}

/*
 * Merge given types to a single node.
 * Any type can end up here.
 * p is the old node, q is the old (if any).
 * CLASS is AUTO, EXTERN, REGISTER, STATIC or TYPEDEF.
 * QUALIFIER is VOL or CON
 * TYPE is CHAR, SHORT, INT, LONG, SIGNED, UNSIGNED, VOID, BOOL, FLOAT,
 * 	DOUBLE, STRTY, UNIONTY.
 */
struct typctx {
	int class, qual, sig, uns, cmplx, imag, err;
	TWORD type;
	NODE *saved, *prea, *posta;
};

static void
typwalk(NODE *p, void *arg)
{
	struct typctx *tc = arg;

#define	cmop(x,y) block(CM, x, y, INT, 0, MKSUE(INT))
	switch (p->n_op) {
	case ATTRIB:
		if (tc->saved && (tc->saved->n_qual & 1)) {
			if (tc->posta == NULL)
				tc->posta = p->n_left;
			else
				tc->posta = cmop(tc->posta, p->n_left);
		} else {
			if (tc->prea == NULL)
				tc->prea = p->n_left;
			else
				tc->prea = cmop(tc->prea, p->n_left);
		}
		p->n_left = bcon(0); /* For tfree() */
		break;
	case CLASS:
		if (tc->class)
			tc->err = 1; /* max 1 class */
		tc->class = p->n_type;
		break;

	case QUALIFIER:
		tc->qual |= p->n_type >> TSHIFT;
		break;

	case TYPE:
		if (p->n_sp != NULL || ISSOU(p->n_type)) {
			/* typedef, enum or struct/union */
			if (tc->saved || tc->type)
				tc->err = 1;
#ifdef GCC_COMPAT
			if (ISSOU(p->n_type) && p->n_left) {
				if (tc->posta)
					cerror("typwalk");
				tc->posta = p->n_left;
			}
#endif
			tc->saved = ccopy(p);
			break;
		}

		switch (p->n_type) {
		case BOOL:
		case CHAR:
		case FLOAT:
		case VOID:
			if (tc->type)
				tc->err = 1;
			tc->type = p->n_type;
			break;
		case DOUBLE:
			if (tc->type == 0)
				tc->type = DOUBLE;
			else if (tc->type == LONG)
				tc->type = LDOUBLE;
			else
				tc->err = 1;
			break;
		case SHORT:
			if (tc->type == 0 || tc->type == INT)
				tc->type = SHORT;
			else
				tc->err = 1;
			break;
		case INT:
			if (tc->type == SHORT || tc->type == LONG ||
			    tc->type == LONGLONG)
				break;
			else if (tc->type == 0)
				tc->type = INT;
			else
				tc->err = 1;
			break;
		case LONG:
			if (tc->type == 0)
				tc->type = LONG;
			else if (tc->type == INT)
				break;
			else if (tc->type == LONG)
				tc->type = LONGLONG;
			else if (tc->type == DOUBLE)
				tc->type = LDOUBLE;
			else
				tc->err = 1;
			break;
		case SIGNED:
			if (tc->sig || tc->uns)
				tc->err = 1;
			tc->sig = 1;
			break;
		case UNSIGNED:
			if (tc->sig || tc->uns)
				tc->err = 1;
			tc->uns = 1;
			break;
		case COMPLEX:
			tc->cmplx = 1;
			break;
		case IMAG:
			tc->imag = 1;
			break;
		default:
			cerror("typwalk");
		}
	}

}

NODE *
typenode(NODE *p)
{
	struct symtab *sp;
	struct typctx tc;
	NODE *q;
	char *c;

	memset(&tc, 0, sizeof(struct typctx));

	flist(p, typwalk, &tc);
	tfree(p);

	if (tc.err)
		goto bad;

	if (tc.cmplx || tc.imag) {
		if (tc.type == 0)
			tc.type = DOUBLE;
		if ((tc.cmplx && tc.imag) || tc.sig || tc.uns ||
		    !ISFTY(tc.type))
			goto bad;
		if (tc.cmplx) {
			c = tc.type == DOUBLE ? "0d" :
			    tc.type == FLOAT ? "0f" : "0l";
			sp = lookup(addname(c), 0);
			tc.type = STRTY;
			tc.saved = mkty(tc.type, sp->sdf, sp->ssue);
			tc.saved->n_sp = sp;
			tc.type = 0;
		} else
			tc.type += (FIMAG-FLOAT);
	}

	if (tc.saved && tc.type)
		goto bad;
	if (tc.sig || tc.uns) {
		if (tc.type == 0)
			tc.type = tc.sig ? INT : UNSIGNED;
		if (tc.type > ULONGLONG)
			goto bad;
		if (tc.uns)
			tc.type = ENUNSIGN(tc.type);
	}

	if (funsigned_char && tc.type == CHAR && tc.sig == 0)
		tc.type = UCHAR;

#ifdef GCC_COMPAT
	if (pragma_allpacked && tc.saved && ISSOU(tc.saved->n_type)) {
		/* Only relevant for structs and unions */
		q = bdty(CALL, bdty(NAME, "packed"), bcon(pragma_allpacked));
		tc.posta = (tc.posta == NIL ? q : cmop(tc.posta, q));
	} else if (pragma_packed) {
		q = bdty(CALL, bdty(NAME, "packed"), bcon(pragma_packed));
		tc.posta = (tc.posta == NIL ? q : cmop(tc.posta, q));
	}
	if (pragma_aligned) {
		/* Deal with relevant pragmas */
		q = bdty(CALL, bdty(NAME, "aligned"), bcon(pragma_aligned));
		tc.posta = (tc.posta == NIL ? q : cmop(tc.posta, q));
	}
	pragma_aligned = pragma_packed = 0;
	if (tc.posta) {
		/* Can only occur for TYPEDEF, STRUCT or UNION */
		if (tc.saved == NULL)
			cerror("typenode");
		gcc_tcattrfix(tc.saved, tc.posta);
	}
#endif
	q = (tc.saved ? tc.saved : mkty(tc.type, 0, 0));
	q->n_qual = tc.qual;
	q->n_lval = tc.class;
	if (BTYPE(q->n_type) == UNDEF)
		MODTYPE(q->n_type, INT);
#ifdef GCC_COMPAT
	if (tc.prea)
		q = cmop(q, tc.prea);
#endif
	return q;

bad:	uerror("illegal type combination");
	return mkty(INT, 0, 0);
}

struct tylnk {
	struct tylnk *next;
	union dimfun df;
};

static void tyreduce(NODE *p, struct tylnk **, int *);

static void
tylkadd(union dimfun dim, struct tylnk **tylkp, int *ntdim)
{
	(*tylkp)->next = tmpalloc(sizeof(struct tylnk));
	*tylkp = (*tylkp)->next;
	(*tylkp)->next = NULL;
	(*tylkp)->df = dim;
	(*ntdim)++;
}

/*
 * merge type typ with identifier idp.
 * idp is returned as a NAME node with correct types.
 * typ is untouched.
 */
NODE *
tymerge(NODE *typ, NODE *idp)
{
	NODE *p;
	union dimfun *j;
	struct tylnk *base, tylnk, *tylkp;
	struct suedef *sue;
	unsigned int t;
	int ntdim, i;

#ifdef GCC_COMPAT
	NODE *gcs;

	if (typ->n_op == CM) {
		/* has attributes */
		gcs = ccopy(typ->n_right);
		typ = typ->n_left;
	} else
		gcs = NULL;
#endif

	if (typ->n_op != TYPE)
		cerror("tymerge: arg 1");

#ifdef PCC_DEBUG
	if (ddebug > 2) {
		printf("tymerge(%p,%p)\n", typ, idp);
		fwalk(typ, eprint, 0);
		fwalk(idp, eprint, 0);
	}
#endif

//if (idp && idp->n_sue) { printf("idp "); dump_attr(idp->n_sue->suega); }
//if (typ && typ->n_sue) { printf("typ "); dump_attr(typ->n_sue->suega); }
	sue = idp->n_sue;

	idp->n_type = typ->n_type;
	idp->n_qual |= typ->n_qual;

	tylkp = &tylnk;
	tylkp->next = NULL;
	ntdim = 0;

	tyreduce(idp, &tylkp, &ntdim);
	idp->n_sue = typ->n_sue;

	for (t = typ->n_type, j = typ->n_df; t&TMASK; t = DECREF(t))
		if (ISARY(t) || ISFTN(t))
			tylkadd(*j++, &tylkp, &ntdim);

	if (ntdim) {
		union dimfun *a = permalloc(sizeof(union dimfun) * ntdim);
		dimfuncnt += ntdim;
		for (i = 0, base = tylnk.next; base; base = base->next, i++)
			a[i] = base->df;
		idp->n_df = a;
	} else
		idp->n_df = NULL;

	/* now idp is a single node: fix up type */

/* Start: ensure that this des not cause any problem */
	idp->n_type = ctype(idp->n_type);

	/* in case ctype has rewritten things */
	if ((t = BTYPE(idp->n_type)) != STRTY && t != UNIONTY && t != ENUMTY)
		idp->n_sue = MKSUE(t);
/* End: ensure that this des not cause any problem */

	if (idp->n_op != NAME) {
		for (p = idp->n_left; p->n_op != NAME; p = p->n_left)
			nfree(p);
		nfree(p);
		idp->n_op = NAME;
	}

#ifdef GCC_COMPAT
	if (sue && sue->suega) {
		idp->n_sue = sueget(idp->n_sue);
		idp->n_sue->suega = sue->suega;
	}
#endif

//if (idp && idp->n_sue) { printf("residp ");
//	GETSUE(sue, idp->n_sue) dump_attr(sue->suega); }

#ifdef GCC_COMPAT
	if (gcs)
		idp = cmop(idp, gcs);
#endif

	return(idp);
}

/*
 * Retrieve all CM-separated argument types, sizes and dimensions and
 * put them in an array.
 * XXX - can only check first type level, side effects?
 */
static union arglist *
arglist(NODE *n)
{
	union arglist *al;
	NODE *w = n, **ap;
	int num, cnt, i, j, k;
	TWORD ty;

#ifdef PCC_DEBUG
	if (pdebug) {
		printf("arglist %p\n", n);
		fwalk(n, eprint, 0);
	}
#endif
	/* First: how much to allocate */
	for (num = cnt = 0, w = n; w->n_op == CM; w = w->n_left) {
		cnt++;	/* Number of levels */
		num++;	/* At least one per step */
		if (w->n_right->n_op == ELLIPSIS)
			continue;
		ty = w->n_right->n_type;
		if (BTYPE(ty) == STRTY || BTYPE(ty) == UNIONTY)
			num++;
		while (ISFTN(ty) == 0 && ISARY(ty) == 0 && ty > BTMASK)
			ty = DECREF(ty);
		if (ty > BTMASK)
			num++;
	}
	cnt++;
	ty = w->n_type;
	if (BTYPE(ty) == STRTY || BTYPE(ty) == UNIONTY)
		num++;
	while (ISFTN(ty) == 0 && ISARY(ty) == 0 && ty > BTMASK)
		ty = DECREF(ty);
	if (ty > BTMASK)
		num++;
	num += 2; /* TEND + last arg type */

	/* Second: Create list to work on */
	ap = tmpalloc(sizeof(NODE *) * cnt);
	al = permalloc(sizeof(union arglist) * num);
	arglistcnt += num;

	for (w = n, i = 0; w->n_op == CM; w = w->n_left)
		ap[i++] = w->n_right;
	ap[i] = w;

	/* Third: Create actual arg list */
	for (k = 0, j = i; j >= 0; j--) {
		if (ap[j]->n_op == ELLIPSIS) {
			al[k++].type = TELLIPSIS;
			ap[j]->n_op = ICON; /* for tfree() */
			continue;
		}
		/* Convert arrays to pointers */
		if (ISARY(ap[j]->n_type)) {
			ap[j]->n_type += (PTR-ARY);
			ap[j]->n_df++;
		}
		/* Convert (silently) functions to pointers */
		if (ISFTN(ap[j]->n_type))
			ap[j]->n_type = INCREF(ap[j]->n_type);
		ty = ap[j]->n_type;
		al[k++].type = ty;
		if (BTYPE(ty) == STRTY || BTYPE(ty) == UNIONTY)
			al[k++].sue = ap[j]->n_sue;
		while (ISFTN(ty) == 0 && ISARY(ty) == 0 && ty > BTMASK)
			ty = DECREF(ty);
		if (ty > BTMASK)
			al[k++].df = ap[j]->n_df;
	}
	al[k++].type = TNULL;
	if (k > num)
		cerror("arglist: k%d > num%d", k, num);
	tfree(n);
#ifdef PCC_DEBUG
	if (pdebug)
		alprint(al, 0);
#endif
	return al;
}

/*
 * build a type, and stash away dimensions,
 * from a parse tree of the declaration
 * the type is build top down, the dimensions bottom up
 */
void
tyreduce(NODE *p, struct tylnk **tylkp, int *ntdim)
{
	union dimfun dim;
	NODE *r = NULL;
	int o;
	TWORD t, q;

	o = p->n_op;
	if (o == NAME) {
		p->n_qual = DECQAL(p->n_qual);
		return;
	}

	t = INCREF(p->n_type);
	q = p->n_qual;
	switch (o) {
	case CALL:
		t += (FTN-PTR);
		dim.dfun = arglist(p->n_right);
		break;
	case UCALL:
		t += (FTN-PTR);
		dim.dfun = NULL;
		break;
	case LB:
		t += (ARY-PTR);
		if (p->n_right->n_op != ICON) {
			r = p->n_right;
			o = RB;
		} else {
			dim.ddim = (int)p->n_right->n_lval;
			nfree(p->n_right);
#ifdef notdef
	/* XXX - check dimensions at usage time */
			if (dim.ddim == NOOFFSET && p->n_left->n_op == LB)
				uerror("null dimension");
#endif
		}
		break;
	}

	p->n_left->n_type = t;
	p->n_left->n_qual = INCQAL(q) | p->n_left->n_qual;
	tyreduce(p->n_left, tylkp, ntdim);

	if (o == LB || o == (UCALL) || o == CALL)
		tylkadd(dim, tylkp, ntdim);
	if (o == RB) {
		dim.ddim = -1;
		tylkadd(dim, tylkp, ntdim);
		arrstk[arrstkp++] = r;
	}

	p->n_sp = p->n_left->n_sp;
	p->n_type = p->n_left->n_type;
	p->n_qual = p->n_left->n_qual;
}

static NODE *
argcast(NODE *p, TWORD t, union dimfun *d, struct suedef *sue)
{
	NODE *u, *r = talloc();

	r->n_op = NAME;
	r->n_type = t;
	r->n_qual = 0; /* XXX */
	r->n_df = d;
	r->n_sue = sue;

	u = buildtree(CAST, r, p);
	nfree(u->n_left);
	r = u->n_right;
	nfree(u);
	return r;
}

#ifdef PCC_DEBUG
/*
 * Print a prototype.
 */
static void
alprint(union arglist *al, int in)
{
	TWORD t;
	int i = 0, j;

	for (; al->type != TNULL; al++) {
		for (j = in; j > 0; j--)
			printf("  ");
		printf("arg %d: ", i++);
		t = al->type;
		tprint(stdout, t, 0);
		while (t > BTMASK) {
			if (ISARY(t)) {
				al++;
				printf(" dim %d ", al->df->ddim);
			} else if (ISFTN(t)) {
				al++;
				alprint(al->df->dfun, in+1);
			}
			t = DECREF(t);
		}
		if (ISSTR(t)) {
			al++;
			printf(" (size %d align %d)", al->sue->suesize,
			    al->sue->suealign);
		}
		printf("\n");
	}
	if (in == 0)
		printf("end arglist\n");
}
#endif
static int
suemeq(struct suedef *s1, struct suedef *s2)
{

	GETSUE(s1, s1);
	GETSUE(s2, s2);

	return (s1->suem == s2->suem);
}

/*
 * Do prototype checking and add conversions before calling a function.
 * Argument f is function and a is a CM-separated list of arguments.
 * Returns a merged node (via buildtree() of function and arguments.
 */
NODE *
doacall(struct symtab *sp, NODE *f, NODE *a)
{
	NODE *w, *r;
	union arglist *al;
	struct ap {
		struct ap *next;
		NODE *node;
	} *at, *apole = NULL;
	int argidx/* , hasarray = 0*/;
	TWORD type, arrt;

#ifdef PCC_DEBUG
	if (ddebug) {
		printf("doacall.\n");
		fwalk(f, eprint, 0);
		if (a)
			fwalk(a, eprint, 0);
	}
#endif

	/* First let MD code do something */
	calldec(f, a);
/* XXX XXX hack */
	if ((f->n_op == CALL) &&
	    f->n_left->n_op == ADDROF &&
	    f->n_left->n_left->n_op == NAME &&
	    (f->n_left->n_left->n_type & 0x7e0) == 0x4c0)
		goto build;
/* XXX XXX hack */

#ifndef NO_C_BUILTINS
	/* check for builtins. function pointers are not allowed */
	if (f->n_op == NAME &&
	    f->n_sp->sname[0] == '_' && f->n_sp->sname[1] == '_')
		if ((w = builtin_check(f, a)) != NIL)
			return w;
#endif

	/* Check for undefined or late defined enums */
	if (BTYPE(f->n_type) == ENUMTY) {
		/* not-yet check if declared enum */
		if (f->n_sue->suem->stype != ENUMTY)
			MODTYPE(f->n_type, f->n_sue->suem->stype);
		if (BTYPE(f->n_type) == ENUMTY)
			uerror("enum %s not declared", f->n_sue->suem->sname);
	}

	/*
	 * Do some basic checks.
	 */
	if (f->n_df == NULL || (al = f->n_df[0].dfun) == NULL) {
		if (Wimplicit_function_declaration) {
			if (f->n_sp != NULL) {
				if (strncmp(f->n_sp->sname,
				    "__builtin", 9) != 0)
					werror("no prototype for function "
					    "'%s()'", f->n_sp->sname);
			} else {
				werror("no prototype for function pointer");
			}
		}
		/* floats must be cast to double */
		if (a == NULL)
			goto build;
		for (w = a; w->n_op == CM; w = w->n_left) {
			if (w->n_right->n_op == TYPE)
				uerror("type is not an argument");
			if (w->n_right->n_type != FLOAT)
				continue;
			w->n_right = argcast(w->n_right, DOUBLE,
			    NULL, MKSUE(DOUBLE));
		}
		if (a->n_op == TYPE)
			uerror("type is not an argument");
		if (a->n_type == FLOAT) {
			MKTY(a, DOUBLE, 0, 0);
		}
		goto build;
	}
	if (al->type == VOID) {
		if (a != NULL)
			uerror("function takes no arguments");
		goto build; /* void function */
	} else {
		if (a == NULL) {
			uerror("function needs arguments");
			goto build;
		}
	}
#ifdef PCC_DEBUG
	if (pdebug) {
		printf("arglist for %s\n",
		    f->n_sp != NULL ? f->n_sp->sname : "function pointer");
		alprint(al, 0);
	}
#endif

	/*
	 * Create a list of pointers to the nodes given as arg.
	 */
	for (w = a; w->n_op == CM; w = w->n_left) {
		at = tmpalloc(sizeof(struct ap));
		at->node = w->n_right;
		at->next = apole;
		apole = at;
	}
	at = tmpalloc(sizeof(struct ap));
	at->node = w;
	at->next = apole;
	apole = at;

	/*
	 * Do the typechecking by walking up the list.
	 */
	argidx = 1;
	while (al->type != TNULL) {
		if (al->type == TELLIPSIS) {
			/* convert the rest of float to double */
			for (; apole; apole = apole->next) {
				if (apole->node->n_type != FLOAT)
					continue;
				MKTY(apole->node, DOUBLE, 0, 0);
			}
			goto build;
		}
		if (apole == NULL) {
			uerror("too few arguments to function");
			goto build;
		}
/* al = prototyp, apole = argument till ftn */
/* type = argumentets typ, arrt = prototypens typ */
		type = apole->node->n_type;
		arrt = al->type;
#if 0
		if ((hasarray = ISARY(arrt)))
			arrt += (PTR-ARY);
#endif
		/* Taking addresses of arrays are meaningless in expressions */
		/* but people tend to do that and also use in prototypes */
		/* this is mostly a problem with typedefs */
		if (ISARY(type)) {
			if (ISPTR(arrt) && ISARY(DECREF(arrt)))
				type = INCREF(type);
			else
				type += (PTR-ARY);
		} else if (ISPTR(type) && !ISARY(DECREF(type)) &&
		    ISPTR(arrt) && ISARY(DECREF(arrt))) {
			type += (ARY-PTR);
			type = INCREF(type);
		}

		/* Check structs */
		if (type <= BTMASK && arrt <= BTMASK) {
			if (type != arrt) {
				if (ISSOU(BTYPE(type)) || ISSOU(BTYPE(arrt))) {
incomp:					uerror("incompatible types for arg %d",
					    argidx);
				} else {
					MKTY(apole->node, arrt, 0, 0)
				}
#ifndef NO_COMPLEX
			} else if (type == STRTY &&
			    gcc_get_attr(apole->node->n_sue, ATTR_COMPLEX) &&
			    gcc_get_attr(al[1].sue, ATTR_COMPLEX)) {
				/* Both are complex */
				if (apole->node->n_sue->suem->stype !=
				    al[1].sue->suem->stype) {
					/* must convert to correct type */
					w = talloc();
					*w = *apole->node;
					w = mkcmplx(w, al[1].sue->suem->stype);
					*apole->node = *w;
					nfree(w);
				}
				goto out;
#endif
			} else if (ISSOU(BTYPE(type))) {
				if (!suemeq(apole->node->n_sue, al[1].sue))
					goto incomp;
			}
			goto out;
		}

		/* XXX should (recusively) check return type and arg list of
		   func ptr arg XXX */
		if (ISFTN(DECREF(arrt)) && ISFTN(type))
			type = INCREF(type);

		/* Hereafter its only pointers (or arrays) left */
		/* Check for struct/union intermixing with other types */
		if (((type <= BTMASK) && ISSOU(BTYPE(type))) ||
		    ((arrt <= BTMASK) && ISSOU(BTYPE(arrt))))
			goto incomp;

		/* Check for struct/union compatibility */
		if (type == arrt) {
			if (ISSOU(BTYPE(type))) {
				if (suemeq(apole->node->n_sue, al[1].sue))
					goto out;
			} else
				goto out;
		}
		if (BTYPE(arrt) == VOID && type > BTMASK)
			goto skip; /* void *f = some pointer */
		if (arrt > BTMASK && BTYPE(type) == VOID)
			goto skip; /* some *f = void pointer */
		if (apole->node->n_op == ICON && apole->node->n_lval == 0)
			goto skip; /* Anything assigned a zero */

		if ((type & ~BTMASK) == (arrt & ~BTMASK)) {
			/* do not complain for pointers with signedness */
			if (!Wpointer_sign &&
			    DEUNSIGN(BTYPE(type)) == DEUNSIGN(BTYPE(arrt)))
				goto skip;
		}

		werror("implicit conversion of argument %d due to prototype",
		    argidx);

skip:		if (ISSTR(BTYPE(arrt))) {
			MKTY(apole->node, arrt, 0, al[1].sue)
		} else {
			MKTY(apole->node, arrt, 0, MKSUE(BTYPE(arrt)))
		}

out:		al++;
		if (ISSTR(BTYPE(arrt)))
			al++;
#if 0
		while (arrt > BTMASK && !ISFTN(arrt))
			arrt = DECREF(arrt);
		if (ISFTN(arrt) || hasarray)
			al++;
#else
		while (arrt > BTMASK) {
			if (ISARY(arrt) || ISFTN(arrt)) {
				al++;
				break;
			}
			arrt = DECREF(arrt);
		}
#endif
		apole = apole->next;
		argidx++;
	}
	if (apole != NULL)
		uerror("too many arguments to function");

build:	if (sp != NULL && (sp->sflags & SINLINE) && (w = inlinetree(sp, f, a)))
		return w;
	return buildtree(a == NIL ? UCALL : CALL, f, a);
}

static int
chk2(TWORD type, union dimfun *dsym, union dimfun *ddef)
{
	while (type > BTMASK) {
		switch (type & TMASK) {
		case ARY:
			/* may be declared without dimension */
			if (dsym->ddim == NOOFFSET)
				dsym->ddim = ddef->ddim;
			if (dsym->ddim < 0 && ddef->ddim < 0)
				; /* dynamic arrays as arguments */
			else if (ddef->ddim > 0 && dsym->ddim != ddef->ddim)
				return 1;
			dsym++, ddef++;
			break;
		case FTN:
			/* old-style function headers with function pointers
			 * will most likely not have a prototype.
			 * This is not considered an error.  */
			if (ddef->dfun == NULL) {
#ifdef notyet
				werror("declaration not a prototype");
#endif
			} else if (chkftn(dsym->dfun, ddef->dfun))
				return 1;
			dsym++, ddef++;
			break;
		}
		type = DECREF(type);
	}
	return 0;
}

/*
 * Compare two function argument lists to see if they match.
 */
int
chkftn(union arglist *usym, union arglist *udef)
{
	TWORD t2;
	int ty, tyn;

	if (usym == NULL)
		return 0;
	if (cftnsp != NULL && udef == NULL && usym->type == VOID)
		return 0; /* foo() { function with foo(void); prototype */
	if (udef == NULL && usym->type != TNULL)
		return 1;
	while (usym->type != TNULL) {
		if (usym->type == udef->type)
			goto done;
		/*
		 * If an old-style declaration, then all types smaller than
		 * int are given as int parameters.
		 */
		if (intcompare) {
			ty = BTYPE(usym->type);
			tyn = BTYPE(udef->type);
			if (ty == tyn || ty != INT)
				return 1;
			if (tyn == CHAR || tyn == UCHAR ||
			    tyn == SHORT || tyn == USHORT)
				goto done;
			return 1;
		} else
			return 1;

done:		ty = BTYPE(usym->type);
		t2 = usym->type;
		if (ISSTR(ty)) {
			usym++, udef++;
			if (usym->sue->suem != udef->sue->suem)
				return 1;
		}

		while (ISFTN(t2) == 0 && ISARY(t2) == 0 && t2 > BTMASK)
			t2 = DECREF(t2);
		if (t2 > BTMASK) {
			usym++, udef++;
			if (chk2(t2, usym->df, udef->df))
				return 1;
		}
		usym++, udef++;
	}
	if (usym->type != udef->type)
		return 1;
	return 0;
}

void
fixtype(NODE *p, int class)
{
	unsigned int t, type;
	int mod1, mod2;
	/* fix up the types, and check for legality */

	/* forward declared enums */
	if (BTYPE(p->n_sp->stype) == ENUMTY)
		MODTYPE(p->n_sp->stype, p->n_sp->ssue->suem->stype);

	if( (type = p->n_type) == UNDEF ) return;
	if ((mod2 = (type&TMASK))) {
		t = DECREF(type);
		while( mod1=mod2, mod2 = (t&TMASK) ){
			if( mod1 == ARY && mod2 == FTN ){
				uerror( "array of functions is illegal" );
				type = 0;
				}
			else if( mod1 == FTN && ( mod2 == ARY || mod2 == FTN ) ){
				uerror( "function returns illegal type" );
				type = 0;
				}
			t = DECREF(t);
			}
		}

	/* detect function arguments, watching out for structure declarations */
	if (rpole && ISFTN(type)) {
		uerror("function illegal in structure or union");
		type = INCREF(type);
	}
	p->n_type = type;
}

/*
 * give undefined version of class
 */
int
uclass(int class)
{
	if (class == SNULL)
		return(EXTERN);
	else if (class == STATIC)
		return(USTATIC);
	else if (class == FORTRAN)
		return(UFORTRAN);
	else
		return(class);
}

int
fixclass(int class, TWORD type)
{
	extern int fun_inline;

	/* first, fix null class */
	if (class == SNULL) {
		if (fun_inline && ISFTN(type))
			return SNULL;
		if (rpole)
			class = rpole->rsou == STNAME ? MOS : MOU;
		else if (blevel == 0)
			class = EXTDEF;
		else
			class = AUTO;
	}

	/* now, do general checking */

	if( ISFTN( type ) ){
		switch( class ) {
		default:
			uerror( "function has illegal storage class" );
		case AUTO:
			class = EXTERN;
		case EXTERN:
		case EXTDEF:
		case FORTRAN:
		case TYPEDEF:
		case STATIC:
		case UFORTRAN:
		case USTATIC:
			;
			}
		}

	if (class & FIELD) {
		if (rpole && rpole->rsou != STNAME && rpole->rsou != UNAME)
			uerror("illegal use of field");
		return(class);
	}

	switch (class) {

	case MOS:
	case MOU:
		if (rpole == NULL)
			uerror("illegal member class");
		return(class);

	case REGISTER:
		if (blevel == 0)
			uerror("illegal register declaration");
		if (blevel == 1)
			return(PARAM);
		else
			return(AUTO);

	case AUTO:
		if( blevel < 2 ) uerror( "illegal ULABEL class" );
		return( class );

	case UFORTRAN:
	case FORTRAN:
# ifdef NOFORTRAN
		NOFORTRAN;    /* a condition which can regulate the FORTRAN usage */
# endif
		if( !ISFTN(type) ) uerror( "fortran declaration must apply to function" );
		else {
			type = DECREF(type);
			if( ISFTN(type) || ISARY(type) || ISPTR(type) ) {
				uerror( "fortran function has wrong type" );
				}
			}
	case EXTERN:
	case STATIC:
	case EXTDEF:
	case TYPEDEF:
	case USTATIC:
	case PARAM:
		return( class );

	default:
		cerror( "illegal class: %d", class );
		/* NOTREACHED */

	}
	return 0; /* XXX */
}

/*
 * Generates a goto statement; sets up label number etc.
 */
void
gotolabel(char *name)
{
	struct symtab *s = lookup(name, SLBLNAME);

	if (s->soffset == 0)
		s->soffset = -getlab();
	branch(s->soffset < 0 ? -s->soffset : s->soffset);
}

/*
 * Sets a label for gotos.
 */
void
deflabel(char *name)
{
	struct symtab *s = lookup(name, SLBLNAME);

	if (s->soffset > 0)
		uerror("label '%s' redefined", name);
	if (s->soffset == 0)
		s->soffset = getlab();
	if (s->soffset < 0)
		s->soffset = -s->soffset;
	plabel( s->soffset);
}

struct symtab *
getsymtab(char *name, int flags)
{
	struct symtab *s;

	if (flags & STEMP) {
		s = tmpalloc(sizeof(struct symtab));
	} else {
		s = permalloc(sizeof(struct symtab));
		symtabcnt++;
	}
	s->sname = name;
	s->soname = NULL;
	s->snext = NULL;
	s->stype = UNDEF;
	s->squal = 0;
	s->sclass = SNULL;
	s->sflags = (short)(flags & SMASK);
	s->soffset = 0;
	s->slevel = (char)blevel;
	s->sdf = NULL;
	s->ssue = NULL;
	return s;
}

int
fldchk(int sz)
{
	if (rpole->rsou != STNAME && rpole->rsou != UNAME)
		uerror("field outside of structure");
	if (sz < 0 || sz >= FIELD) {
		uerror("illegal field size");
		return 1;
	}
	return 0;
}

#ifdef PCC_DEBUG
static char *
ccnames[] = { /* names of storage classes */
	"SNULL",
	"AUTO",
	"EXTERN",
	"STATIC",
	"REGISTER",
	"EXTDEF",
	"LABEL",
	"ULABEL",
	"MOS",
	"PARAM",
	"STNAME",
	"MOU",
	"UNAME",
	"TYPEDEF",
	"FORTRAN",
	"ENAME",
	"MOE",
	"UFORTRAN",
	"USTATIC",
	};

char *
scnames(int c)
{
	/* return the name for storage class c */
	static char buf[12];
	if( c&FIELD ){
		snprintf( buf, sizeof(buf), "FIELD[%d]", c&FLDSIZ );
		return( buf );
		}
	return( ccnames[c] );
	}
#endif

static char *stack_chk_fail = "__stack_chk_fail";
static char *stack_chk_guard = "__stack_chk_guard";
static char *stack_chk_canary = "__stack_chk_canary";

void
sspinit()
{
	NODE *p;

	p = block(NAME, NIL, NIL, FTN+VOID, 0, MKSUE(VOID));
	p->n_sp = lookup(stack_chk_fail, SNORMAL);
	defid(p, EXTERN);
	nfree(p);

	p = block(NAME, NIL, NIL, INT, 0, MKSUE(INT));
	p->n_sp = lookup(stack_chk_guard, SNORMAL);
	defid(p, EXTERN);
	nfree(p);
}

void
sspstart()
{
	NODE *p, *q;

	q = block(NAME, NIL, NIL, INT, 0, MKSUE(INT));
 	q->n_sp = lookup(stack_chk_guard, SNORMAL);
	q = clocal(q);

	p = block(REG, NIL, NIL, INT, 0, 0);
	p->n_lval = 0;
	p->n_rval = FPREG;
	q = block(ER, p, q, INT, 0, MKSUE(INT));
	q = clocal(q);

	p = block(NAME, NIL, NIL, INT, 0, MKSUE(INT));
	p->n_qual = VOL >> TSHIFT;
	p->n_sp = lookup(stack_chk_canary, SNORMAL);
	defid(p, AUTO);
	p = clocal(p);
	ecomp(buildtree(ASSIGN, p, q));
}

void
sspend()
{
	NODE *p, *q;
	TWORD t;
	int tmpnr = 0;
	int lab;

	if (retlab != NOLAB) {
		plabel(retlab);
		retlab = getlab();
	}

	t = DECREF(cftnsp->stype);
	if (t == BOOL)
		t = BOOL_TYPE;

	if (t != VOID && !ISSOU(t)) {
		p = tempnode(0, t, cftnsp->sdf, cftnsp->ssue);
		tmpnr = regno(p);
		q = block(REG, NIL, NIL, t, cftnsp->sdf, cftnsp->ssue);
		q->n_rval = RETREG(t);
		ecomp(buildtree(ASSIGN, p, q));
	}

	p = block(NAME, NIL, NIL, INT, 0, MKSUE(INT));
	p->n_sp = lookup(stack_chk_canary, SNORMAL);
	p = clocal(p);

	q = block(REG, NIL, NIL, INT, 0, 0);
	q->n_lval = 0;
	q->n_rval = FPREG;
	q = block(ER, p, q, INT, 0, MKSUE(INT));

	p = block(NAME, NIL, NIL, INT, 0, MKSUE(INT));
	p->n_sp = lookup(stack_chk_guard, SNORMAL);
	p = clocal(p);

	lab = getlab();
	cbranch(buildtree(EQ, p, q), bcon(lab));

	p = block(NAME, NIL, NIL, FTN+VOID, 0, MKSUE(VOID));
	p->n_sp = lookup(stack_chk_fail, SNORMAL);
	p = clocal(p);

	ecomp(buildtree(UCALL, p, NIL));

	plabel(lab);

	if (t != VOID && !ISSOU(t)) {
		p = tempnode(tmpnr, t, cftnsp->sdf, cftnsp->ssue);
		q = block(REG, NIL, NIL, t, cftnsp->sdf, cftnsp->ssue);
		q->n_rval = RETREG(t);
		ecomp(buildtree(ASSIGN, q, p));
	}
}

/*
 * Allocate on the permanent heap for inlines, otherwise temporary heap.
 */
void *
inlalloc(int size)
{
	return isinlining ?  permalloc(size) : tmpalloc(size);
}

#ifndef NO_COMPLEX

static char *real, *imag;
static struct symtab *cxsp[3];
/*
 * As complex numbers internally are handled as structs, create
 * these by hand-crafting them.
 */
void
complinit()
{
	struct rstack *rp;
	NODE *p, *q;
	char *n[] = { "0f", "0d", "0l" };
	int i, odebug;

	odebug = ddebug;
	ddebug = 0;
	real = addname("__real");
	imag = addname("__imag");
	p = block(NAME, NIL, NIL, FLOAT, 0, MKSUE(FLOAT));
	for (i = 0; i < 3; i++) {
		p->n_type = FLOAT+i;
		p->n_sue = MKSUE(FLOAT+i);
		rp = bstruct(NULL, STNAME, NULL);
		soumemb(p, real, 0);
		soumemb(p, imag, 0);
		q = dclstruct(rp);
		cxsp[i] = q->n_sp = lookup(addname(n[i]), 0);
		defid(q, TYPEDEF);
		q->n_sp->ssue->suega = permalloc(sizeof(struct gcc_attr_pack) +
		    sizeof(struct gcc_attrib));
		q->n_sp->ssue->suega->num = 1;
		q->n_sp->ssue->suega->ga[0].atype = ATTR_COMPLEX;
		nfree(q);
	}
	nfree(p);
	ddebug = odebug;
}

/*
 * Return the highest real floating point type.
 * Known that at least one type is complex or imaginary.
 */
static TWORD
maxtyp(NODE *l, NODE *r)
{
	TWORD tl, tr, t;

#define ANYCX(p) (p->n_type == STRTY && gcc_get_attr(p->n_sue, ATTR_COMPLEX))
	tl = ANYCX(l) ? l->n_sue->suem->stype : l->n_type;
	tr = ANYCX(r) ? r->n_sue->suem->stype : r->n_type;
	if (ISITY(tl))
		tl -= (FIMAG - FLOAT);
	if (ISITY(tr))
		tr -= (FIMAG - FLOAT);
	t = tl > tr ? tl : tr;
	if (!ISFTY(t))
		cerror("maxtyp");
	return t;
}

/*
 * Fetch space on stack for complex struct.
 */
static NODE *
cxstore(TWORD t)
{
	struct symtab s;

	s = *cxsp[t - FLOAT];
	s.sclass = AUTO;
	s.soffset = NOOFFSET;
	oalloc(&s, &autooff);
	return nametree(&s);
}

#define	comop(x,y) buildtree(COMOP, x, y)

static NODE *
mkcmplx(NODE *p, TWORD dt)
{
	NODE *q, *r, *i, *t;

	if (!ANYCX(p)) {
		/* Not complex, convert to complex on stack */
		q = cxstore(dt);
		if (ISITY(p->n_type)) {
			p->n_type = p->n_type - FIMAG + FLOAT;
			r = bcon(0);
			i = p;
		} else {
			r = p;
			i = bcon(0);
		}
		p = buildtree(ASSIGN, structref(ccopy(q), DOT, real), r);
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, imag), i));
		p = comop(p, q);
	} else if (p->n_sue->suem->stype != dt) {
		q = cxstore(dt);
		p = buildtree(ADDROF, p, NIL);
		t = tempnode(0, p->n_type, p->n_df, p->n_sue);
		p = buildtree(ASSIGN, ccopy(t), p);
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, real),
		    structref(ccopy(t), STREF, real)));
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, imag),
		    structref(t, STREF, imag)));
		p = comop(p, q);
	}
	return p;
}

static NODE *
cxasg(NODE *l, NODE *r)
{

	if (ANYCX(l) && ANYCX(r) &&
	    l->n_sue->suem->stype != r->n_sue->suem->stype) {
		/* different types in structs */
		r = mkcmplx(r, l->n_sue->suem->stype);
	} else if (!ANYCX(l))
		r = structref(r, DOT, ISITY(l->n_type) ? imag : real);
	else if (!ANYCX(r))
		r = mkcmplx(r, l->n_sue->suem->stype);
	return buildtree(ASSIGN, l, r);
}

/*
 * Fixup complex operations.
 * At least one operand is complex.
 */
NODE *
cxop(int op, NODE *l, NODE *r)
{
	TWORD mxtyp;
	NODE *p, *q;
	NODE *ltemp, *rtemp;
	NODE *real_l, *imag_l;
	NODE *real_r, *imag_r;

	if (op == ASSIGN)
		return cxasg(l, r);

	mxtyp = maxtyp(l, r);
	l = mkcmplx(l, mxtyp);
	r = mkcmplx(r, mxtyp);


	/* put a pointer to left and right elements in a TEMP */
	l = buildtree(ADDROF, l, NIL);
	ltemp = tempnode(0, l->n_type, l->n_df, l->n_sue);
	l = buildtree(ASSIGN, ccopy(ltemp), l);

	r = buildtree(ADDROF, r, NIL);
	rtemp = tempnode(0, r->n_type, r->n_df, r->n_sue);
	r = buildtree(ASSIGN, ccopy(rtemp), r);

	p = comop(l, r);

	/* create the four trees needed for calculation */
	real_l = structref(ccopy(ltemp), STREF, real);
	real_r = structref(ccopy(rtemp), STREF, real);
	imag_l = structref(ltemp, STREF, imag);
	imag_r = structref(rtemp, STREF, imag);

	/* get storage on stack for the result */
	q = cxstore(mxtyp);

	switch (op) {
	case PLUS:
	case MINUS:
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, real), 
		    buildtree(op, real_l, real_r)));
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, imag), 
		    buildtree(op, imag_l, imag_r)));
		break;

	case MUL:
		/* Complex mul is "complex" */
		/* (u+iv)*(x+iy)=((u*x)-(v*y))+i(v*x+y*u) */
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, real),
		    buildtree(MINUS,
		    buildtree(MUL, ccopy(real_r), ccopy(real_l)),
		    buildtree(MUL, ccopy(imag_r), ccopy(imag_l)))));
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, imag),
		    buildtree(PLUS,
		    buildtree(MUL, real_r, imag_l),
		    buildtree(MUL, imag_r, real_l))));
		break;

	case DIV:
		/* Complex div is even more "complex" */
		/* (u+iv)/(x+iy)=(u*x+v*y)/(x*x+y*y)+i((v*x-u*y)/(x*x+y*y)) */
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, real),
		    buildtree(DIV,
		      buildtree(PLUS,
			buildtree(MUL, ccopy(real_r), ccopy(real_l)),
			buildtree(MUL, ccopy(imag_r), ccopy(imag_l))),
		      buildtree(PLUS,
			buildtree(MUL, ccopy(real_r), ccopy(real_r)),
			buildtree(MUL, ccopy(imag_r), ccopy(imag_r))))));
		p = comop(p, buildtree(ASSIGN, structref(ccopy(q), DOT, real),
		    buildtree(DIV,
		      buildtree(MINUS,
			buildtree(MUL, ccopy(imag_l), ccopy(real_r)),
			buildtree(MUL, ccopy(real_l), ccopy(imag_r))),
		      buildtree(PLUS,
			buildtree(MUL, ccopy(real_r), ccopy(real_r)),
			buildtree(MUL, ccopy(imag_r), ccopy(imag_r))))));
		tfree(real_r);
		tfree(real_l);
		tfree(imag_r);
		tfree(imag_l);
		break;
	}
	return comop(p, q);
}

/*
 * Fixup imaginary operations.
 * At least one operand is imaginary, none is complex.
 */
NODE *
imop(int op, NODE *l, NODE *r)
{
	NODE *p, *q;
	TWORD mxtyp;
	int li, ri;

	li = ri = 0;
	if (ISITY(l->n_type))
		li = 1, l->n_type = l->n_type - (FIMAG-FLOAT);
	if (ISITY(r->n_type))
		ri = 1, r->n_type = r->n_type - (FIMAG-FLOAT);

	if (op == ASSIGN)
		cerror("imop ASSIGN");

	mxtyp = maxtyp(l, r);
	switch (op) {
	case PLUS:
		if (li && ri) {
			p = buildtree(PLUS, l, r);
			p->n_type = p->n_type += (FIMAG-FLOAT);
		} else {
			/* If one is imaginary and one is real, make complex */
			if (li)
				q = l, l = r, r = q; /* switch */
			q = cxstore(mxtyp);
			p = buildtree(ASSIGN,
			    structref(ccopy(q), DOT, real), l);
			p = comop(p, buildtree(ASSIGN,
			    structref(ccopy(q), DOT, imag), r));
			p = comop(p, q);
		}
		break;

	case MINUS:
		if (li && ri) {
			p = buildtree(MINUS, l, r);
			p->n_type = p->n_type += (FIMAG-FLOAT);
		} else if (li) {
			q = cxstore(mxtyp);
			p = buildtree(ASSIGN, structref(ccopy(q), DOT, real),
			    buildtree(UMINUS, r, NIL));
			p = comop(p, buildtree(ASSIGN,
			    structref(ccopy(q), DOT, imag), l));
			p = comop(p, q);
		} else /* if (ri) */ {
			q = cxstore(mxtyp);
			p = buildtree(ASSIGN,
			    structref(ccopy(q), DOT, real), l);
			p = comop(p, buildtree(ASSIGN,
			    structref(ccopy(q), DOT, imag),
			    buildtree(UMINUS, r, NIL)));
			p = comop(p, q);
		}
		break;

	case MUL:
		p = buildtree(MUL, l, r);
		if (li && ri)
			p = buildtree(UMINUS, p, NIL);
		if (li ^ ri)
			p->n_type = p->n_type += (FIMAG-FLOAT);
		break;

	case DIV:
		p = buildtree(DIV, l, r);
		if (ri && !li)
			p = buildtree(UMINUS, p, NIL);
		if (li ^ ri)
			p->n_type = p->n_type += (FIMAG-FLOAT);
		break;
	default:
		cerror("imop");
		p = NULL;
	}
	return p;
}

NODE *
cxelem(int op, NODE *p)
{

	if (ANYCX(p)) {
		p = structref(p, DOT, op == XREAL ? real : imag);
	} else if (op == XIMAG) {
		/* XXX  sanitycheck? */
		tfree(p);
		p = bcon(0);
	}
	return p;
}

NODE *
cxconj(NODE *p)
{
	NODE *q, *r;

	/* XXX side effects? */
	q = cxstore(p->n_sue->suem->stype);
	r = buildtree(ASSIGN, structref(ccopy(q), DOT, real),
	    structref(ccopy(p), DOT, real));
	r = comop(r, buildtree(ASSIGN, structref(ccopy(q), DOT, imag),
	    buildtree(UMINUS, structref(p, DOT, imag), NIL)));
	return comop(r, q);
}

/*
 * Prepare for return.
 */
NODE *
cxret(NODE *p, NODE *q)
{
	/* XXX what if cast the other way? */
	return mkcmplx(p, q->n_sue->suem->stype);
}
#endif
