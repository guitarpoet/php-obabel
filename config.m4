PHP_ARG_ENABLE(obabel, whether to enable openbabel support, [ --enable-obabel Enable open babel support], yes, yes)

if test "$PHP_OBABEL" != "no"; then 
	if test "$PHP_OBABEL" != "yes"; then
		OBABEL_SEARCH_DIRS=$PHP_OBABEL
	else
		OBABEL_SEARCH_DIRS="/usr/local/include /usr/include /opt/local/include /usr/local/src /opt/local/src"
	fi

	AC_MSG_CHECKING(openbabel headers)
	for i in $OBABEL_SEARCH_DIRS; do
		if test -f $i/openbabel-2.0/openbabel/mol.h; then
			OBABEL_INCDIR=$i/openbabel-2.0/
		fi
	done

	AC_MSG_CHECKING(openbabel libs)
	OBABEL_LIB_SEARCH_DIRS="/usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64 /opt/local/lib /opt/local/lib64"
	for i in $OBABEL_LIB_SEARCH_DIRS; do
		if test -f $i/libopenbabel.dylib; then
			OBABEL_LD_DIR=$i
		fi
		if test -f $i/libopenbabel.so; then
			OBABEL_LD_DIR=$i
		fi
	done

	PHP_REQUIRE_CXX()
	PHP_ADD_LIBRARY(stdc++, 1, PHP5CPP_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(openbabel, $OBABEL_LD_DIR, OBABEL_SHARED_LIBADD)
	LDFLAGS="-L$OBABEL_LD_DIR -lopenbabel -lstdc++"
	PHP_ADD_INCLUDE($OBABEL_INCDIR)

	AC_MSG_CHECKING(openbabel version function)
	AC_LANG(C++)
	OLD_CPPFLAGS=$CPPFLAGS
	CPPFLAGS="$CPPFLAGS -I$OBABEL_INCDIR"
	AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([#include <openbabel/mol.h>],
	  [OpenBabel::OBReleaseVersion()])],
      [AC_DEFINE([HAVE_OBABEL_RELEASE_VERSION], [], [Openbabel have OBReleaseVersion function])],
      [AC_MSG_WARN([libopenbabel do not have OpenBabel::OBReleaseVersion function.])])
	CPPFLAGS=$OLD_CPPFLAGS

	if test -z "$OBABEL_INCDIR"; then
		AC_MSG_ERROR(Cannot find openbabel's header at /usr/local or /opt/local !)
	fi

	if test -z "$OBABEL_LD_DIR"; then
		AC_MSG_ERROR(Cannot find openbabel's library at /usr/lib, /usr/local/lib or /opt/local/lib !)
	fi

    PHP_NEW_EXTENSION(obabel, src/obabel.cc src/functions.cc src/fragment.cc, $ext_shared,,"",yes)
fi
