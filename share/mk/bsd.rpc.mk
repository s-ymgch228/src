#	$NetBSD: bsd.rpc.mk,v 1.9 2003/10/19 06:10:41 lukem Exp $

.include <bsd.init.mk>

RPC_XDIR?=	${.CURDIR}/

# We don't use implicit suffix rules here to avoid dependencies in the
# Installed files.

.if defined(RPC_INCS)						# {

.for I in ${RPC_INCS}
${I}: ${I:.h=.x}
	${_MKMSGCREATE}
	${_MKCMD}\
	${TOOL_RPCGEN} -C -h ${RPC_XDIR}${I:.h=.x} -o ${.TARGET}
.endfor

DPSRCS+=	${RPC_INCS}
CLEANFILES+=	${RPC_INCS}

.endif								# }


.if defined(RPC_XDRFILES)					# {

.for I in ${RPC_XDRFILES}
${I}: ${RPC_XDIR}${I:_xdr.c=.x}
	${_MKMSGCREATE}
	${_MKCMD}\
	${TOOL_RPCGEN} -C -c ${RPC_XDIR}${I:_xdr.c=.x} -o ${.TARGET}
.endfor

DPSRCS+=	${RPC_XDRFILES}
CLEANFILES+=	${RPC_XDRFILES}

.endif								# }


.if defined(RPC_SVCFILES)					# {

.for I in ${RPC_SVCCLASS}
_RPCS += -s ${I}
.endfor

.for I in ${RPC_SVCFILES}

${I}: ${RPC_XDIR}${I:_svc.c=.x}
	${_MKMSGCREATE}
	${_MKCMD}\
	${TOOL_RPCGEN} -C ${_RPCS} ${RPC_SVCFLAGS} ${RPC_XDIR}${I:_svc.c=.x} \
		-o ${.TARGET}
.endfor

DPSRCS+=	${RPC_SVCFILES}
CLEANFILES+=	${RPC_SVCFILES}

.endif								# }


##### Pull in related .mk logic
.include <bsd.obj.mk>
.include <bsd.sys.mk>
