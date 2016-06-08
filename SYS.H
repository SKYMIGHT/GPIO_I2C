/*
 * sys.h
 *
 * (C) Copyright 2001 to 2003 VIA VPSD
 *
 * Dec 11, 2001 Lucas  Lin      File created.
 *
 * System definition
 */

#ifndef _SYS_H_
#define _SYS_H_

#define DEBUG_MODULE		1
/************************************************************************
 * Basic data type definition
 ************************************************************************/
typedef	unsigned char	tU1;
typedef	unsigned int	tU2;
typedef	unsigned long	tU4;
typedef unsigned int	tUi;

/************************************************************************
 * I/O Port Access routine declaration
 ************************************************************************/
/* #include <conio.h> */
#define  SysPortGetU1(addr)			inp(addr)
#define	 SysPortPutU1(addr, data)	outp(addr, data)

#define  SysPortGetU2(addr)			inpw(addr)
#define	 SysPortPutU2(addr, data)	outpw(addr, data)

#define  SysPortGetU4(addr)			inpd(addr)
#define	 SysPortPutU4(addr, data)	outpd(addr, data)

/************************************************************************
 * I/O Port Access routine declaration
 ************************************************************************/
tU1  inp(tU4 addr);
tU2  inpw(tU4 addr);
tU4  inpd(tU4 addr);
void outp(tU4 addr, tU1 data);
void outpw(tU4 addr, tU2 data);
void outpd(tU4 addr, tU4 data);

#if     DEBUG_MODULE
#define PRINT(x)        printf x;
#else
#define PRINT(x)        do {} while(0);
#endif
#endif  /* _SYS_H_ */
