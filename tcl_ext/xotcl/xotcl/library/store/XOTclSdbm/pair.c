/*
 * sdbm - ndbm work-alike hashed database library
 * based on Per-Aake Larson's Dynamic Hashing algorithms. BIT 18 (1978).
 * author: oz@nexus.yorku.ca
 * status: public domain.
 *
 * page-level routines
 */

#ifndef lint
static char rcsid[] = "$Id: pair.c,v 1.3 2006/09/27 08:12:40 neumann Exp $";
#endif

#include "sdbm.h"
#include "tune.h"
#include "pair.h"

#include <stdlib.h>
#include <string.h>

#define exhash(item)	sdbm_hash((item).dptr, (item).dsize)

/* 
 * forward 
 */
static int seepair proto((char *, int, char *, size_t));

/*
 * page format:
 *	+------------------------------+
 * ino	| n | keyoff | datoff | keyoff |
 * 	+------------+--------+--------+
 *	| datoff | - - - ---->	       |
 *	+--------+---------------------+
 *	|	 F R E E A R E A       |
 *	+--------------+---------------+
 *	|  <---- - - - | data          |
 *	+--------+-----+----+----------+
 *	|  key   | data     | key      |
 *	+--------+----------+----------+
 *
 * calculating the offsets for free area:  if the number
 * of entries (ino[0]) is zero, the offset to the END of
 * the free area is the block size. Otherwise, it is the
 * nth (ino[ino[0]]) entry's offset.
 */

int
fitpair(pag, need)
char *pag;
int need;
{
	register int n;
	register int off;
	register int free_space;
	/* this cast "increases the alignment". Its not the only one, though.*/
	register short *ino = (short *) pag;

	off = ((n = ino[0]) > 0) ? ino[n] : PBLKSIZ;
	free_space = off - (n + 1) * sizeof(short);
	need += 2 * sizeof(short);

	debug(("free %d need %d\n", free_space, need));

	return need <= free_space;
}

void
putpair(pag, key, val)
char *pag;
datum key;
datum val;
{
	register int n;
	register int off;
	register short *ino = (short *) pag;

	off = ((n = ino[0]) > 0) ? ino[n] : PBLKSIZ;
/*
 * enter the key first
 */
	off -= key.dsize;
	(void) memcpy(pag + off, key.dptr, key.dsize);
	ino[n + 1] = off;
/*
 * now the data
 */
	off -= val.dsize;
	(void) memcpy(pag + off, val.dptr, val.dsize);
	ino[n + 2] = off;
/*
 * adjust item count
 */
	ino[0] += 2;
}

datum
getpair(pag, key)
char *pag;
datum key;
{
	register int i;
	register int n;
	datum val;
	register short *ino = (short *) pag;

	if ((n = ino[0]) == 0)
		return nullitem;

	if ((i = seepair(pag, n, key.dptr, key.dsize)) == 0)
		return nullitem;

	val.dptr = pag + ino[i + 1];
	val.dsize = ino[i] - ino[i + 1];
	return val;
}

#ifdef SEEDUPS
int
duppair(pag, key)
char *pag;
datum key;
{
	register short *ino = (short *) pag;
	return ino[0] > 0 && seepair(pag, ino[0], key.dptr, key.dsize) > 0;
}
#endif

datum
getnkey(pag, num)
char *pag;
int num;
{
	datum key;
	register int off;
	register short *ino = (short *) pag;

	num = num * 2 - 1;
	if (ino[0] == 0 || num > ino[0])
		return nullitem;

	off = (num > 1) ? ino[num - 1] : PBLKSIZ;

	key.dptr = pag + ino[num];
	key.dsize = off - ino[num];

	return key;
}

int
delpair(pag, key)
char *pag;
datum key;
{
	register int n;
	register int i;
	register short *ino = (short *) pag;

	if ((n = ino[0]) == 0)
		return 0;

	if ((i = seepair(pag, n, key.dptr, key.dsize)) == 0)
		return 0;
/*
 * found the key. if it is the last entry
 * [i.e. i == n - 1] we just adjust the entry count.
 * hard case: move all data down onto the deleted pair,
 * shift offsets onto deleted offsets, and adjust them.
 * [note: 0 < i < n]
 */
	if (i < n - 1) {
		register int m;
		register char *dst = pag + (i == 1 ? PBLKSIZ : ino[i - 1]);
		register char *src = pag + ino[i + 1];
		register int   zoo = dst - src;

		debug(("free-up %d ", zoo));
/*
 * shift data/keys down
 */
		m = ino[i + 1] - ino[n];
#ifdef DUFF
#define MOVB 	*--dst = *--src

		if (m > 0) {
			register int loop = (m + 8 - 1) >> 3;

			switch (m & (8 - 1)) {
			case 0:	do {
				MOVB;	case 7:	MOVB;
			case 6:	MOVB;	case 5:	MOVB;
			case 4:	MOVB;	case 3:	MOVB;
			case 2:	MOVB;	case 1:	MOVB;
				} while (--loop);
			}
		}
#else
#ifdef MEMMOVE
		memmove(dst, src, m);
#else
		while (m--)
			*--dst = *--src;
#endif
#endif
/*
 * adjust offset index up
 */
		while (i < n - 1) {
			ino[i] = ino[i + 2] + zoo;
			i++;
		}
	}
	ino[0] -= 2;
	return 1;
}

/*
 * search for the key in the page.
 * return offset index in the range 0 < i < n.
 * return 0 if not found.
 */
static int
seepair(pag, n, key, siz)
char *pag;
register int n;
register char *key;
register size_t siz;
{
	register int i;
	register int off = PBLKSIZ;
	register short *ino = (short *) pag;

	for (i = 1; i < n; i += 2) {
          if ((int)siz == off - ino[i] &&
		    memcmp(key, pag + ino[i], siz) == 0)
			return i;
		off = ino[i + 1];
	}
	return 0;
}

void
splpage(pag, new, sbit)
char *pag;
char *new;
long sbit;
{
	datum key;
	datum val;

	register int n;
	register int off = PBLKSIZ;
	char cur[PBLKSIZ];
	register short *ino = (short *) cur;

	(void) memcpy(cur, pag, PBLKSIZ);
	(void) memset(pag, 0, PBLKSIZ);
	(void) memset(new, 0, PBLKSIZ);

	n = ino[0];
	for (ino++; n > 0; ino += 2) {
		key.dptr = cur + ino[0]; 
		key.dsize = off - ino[0];
		val.dptr = cur + ino[1];
		val.dsize = ino[0] - ino[1];
/*
 * select the page pointer (by looking at sbit) and insert
 */
		(void) putpair((exhash(key) & sbit) ? new : pag, key, val);

		off = ino[1];
		n -= 2;
	}

	debug(("%d split %d/%d\n", ((short *) cur)[0] / 2, 
	       ((short *) new)[0] / 2,
	       ((short *) pag)[0] / 2));
}

/*
 * check page sanity: 
 * number of entries should be something
 * reasonable, and all offsets in the index should be in order.
 * this could be made more rigorous.
 */
int
chkpage(pag)
char *pag;
{
	register int n;
	register int off;
	register short *ino = (short *) pag;

	if ((n = ino[0]) < 0 || n > PBLKSIZ / sizeof(short))
		return 0;

	if (n > 0) {
		off = PBLKSIZ;
		for (ino++; n > 0; ino += 2) {
			if (ino[0] > off || ino[1] > off ||
			    ino[1] > ino[0])
				return 0;
			off = ino[1];
			n -= 2;
		}
	}
	return 1;
}
