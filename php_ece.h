/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Peter Beverloo <peter@lvp-media.com>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_ECE_H
#define PHP_ECE_H

#define PHP_ECE_VERSION "1.0"

PHP_MINIT_FUNCTION(ece);
PHP_MSHUTDOWN_FUNCTION(ece);
PHP_MINFO_FUNCTION(ece);

PHP_FUNCTION(ece_random_bytes);

PHP_FUNCTION(ece_p256_generate);
PHP_FUNCTION(ece_p256_import);
PHP_FUNCTION(ece_p256_export);
PHP_FUNCTION(ece_p256_compute_key);
PHP_FUNCTION(ece_p256_free);

#endif  // PHP_ECE_H
