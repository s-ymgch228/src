/*	$NetBSD: lexer.h,v 1.2 2004/05/09 03:53:23 christos Exp $	*/


typedef	struct	wordtab	{
	char	*w_word;
	int	w_value;
} wordtab_t;

#ifdef	NO_YACC
#define	YY_COMMENT	1000
#define	YY_CMP_NE	1001
#define	YY_CMP_LE	1002
#define	YY_RANGE_OUT	1003
#define	YY_CMP_GE	1004
#define	YY_RANGE_IN	1005
#define	YY_HEX		1006
#define	YY_NUMBER	1007
#define	YY_IPV6		1008
#define	YY_STR		1009
#define	YY_IPADDR	1010
#endif

#define	YYBUFSIZ	8192

extern	wordtab_t	*yysettab __P((wordtab_t *));
extern	int		yylex __P((void));
extern	void		yyerror __P((char *));
extern	char		*yykeytostr __P((int));

extern	FILE	*yyin;
extern	int	yylineNum;
extern	int	yyexpectaddr;
extern int     *yycont;
extern	int	yybreakondot;
extern	int	yyvarnext;

