/*
----------------------------------------------------------------------------------------------
ARAMIS'S PUBLIC LICENSE

The CACHE SIMULATOR is protected by Copyright, and part of this project is licensed as
OpenSource, but, the AHM Library (LIST.C & LIST.H) is licensed under a restrictive license and
may not be redistributed, reproduced, modified, transmitted, broadcast, published
or adapted in any way, shape or form. cachefed, cachesim, and the main file are freely
available, modifiable and redistributed under the terms of the Aramis's Public License (APL).
----------------------------------------------------------------------------------------------

The Aramis's Public License is based on the Lesser GNU Public
License (LGPL) which provide a clear statement that distribution withproprietary
applications is accepted.

Aramis's Public License
The principles behind the Aramis's license entitles you to freely
modify and copy the source code and libraries built from the source code
under the terms of the license.
You may not distribute the source code or libraries under any other license,
unless authorised to do so by the Copyright holders (Aramis Hornung Moraes).

If you do not accept all the terms of the license then you do not have the right
to copy the source or libraries, such copying is illegal under the terms of
international Copyright law.

Like most software, be it closed or open source, the Aramis's is provided as
is, without any warranty of any kind, or fitness for use, no indemnification is
provided.

By accepting the license, you accept these disclaimers:

The Aramis's Public License is designed to be compatible with use of proprietary
applications, such applications can statically and dynamically link to the
CACHE SIMULATOR. and can be redistributed with the libraries free of charge.
Your applications can be distributed under any license, be it open source or
proprietary.

Modifications to the source code of the CACHE SIMULATOR must remain under
the APL and under the Copyright of the original code, this covers bug fixes
or feature enhancements to the source code. User written extensions to the
CACHE SIMULATOR, such as subclassing from CACHE SIMULATOR classes, are not
covered by the APL, and you are free to license these extensions as you wish,
under a proprietary or open source license.
----------------------------------------------------------------------------------------------
*/

#ifndef CACHEDEF_H
#define CACHEDEF_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FA, DM, SA array struct
typedef struct carr
{
	// Validade, referencida, dados
	int bv;
	int r;
	int d;

	int tag;

	int nature; // dado ou instrucao
}carr;

extern int sdmi, sdmd, sfa, ssa;// lsdmi, lsdmd, lsfa, lssa;
extern int nbdmi, nbdmd, nbfa, nbsa;
extern carr *DM_I, *DM_D, *FA;
extern carr **SA2W, **SA4W, **SA8W;

extern int cache_size;
extern int cache_line_size;

extern int ffa_lru,
	fsa2w_lru,
	fsa4w_lru,
	fsa8w_lru;


void define_cache();
int define_cache_size(int cs);
int define_cache_line_size(int cls);

#endif