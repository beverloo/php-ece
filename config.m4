dnl
dnl $Id$
dnl

PHP_ARG_WITH(openssl, for OpenSSL support,
[  --with-openssl[=DIR]      Include OpenSSL support (requires OpenSSL >= 0.9.8)])

if test "$PHP_OPENSSL" != "no"; then
  PHP_NEW_EXTENSION(ece, php_ece.c, $ext_shared)
  PHP_SUBST(OPENSSL_SHARED_LIBADD)

  PHP_SETUP_OPENSSL(OPENSSL_SHARED_LIBADD, 
  [
    AC_DEFINE(HAVE_ECE_EXT,1,[ ])
  ], [
    AC_MSG_ERROR([OpenSSL check failed. Please check config.log for more information.])
  ])
fi
