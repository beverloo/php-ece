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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_ece.h"

ZEND_BEGIN_ARG_INFO(arginfo_ece_hello_world, 0)
ZEND_END_ARG_INFO()

const zend_function_entry ece_functions[] = {
   PHP_FE(ece_hello_world, arginfo_ece_hello_world)
   PHP_FE_END
};

zend_module_entry ece_module_entry = {
   STANDARD_MODULE_HEADER,
   "ece",
   ece_functions,
   PHP_MINIT(ece),
   PHP_MSHUTDOWN(ece),
   NULL,
   NULL,
   PHP_MINFO(ece),
   PHP_ECE_VERSION,
   STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ECE
ZEND_GET_MODULE(ece)
#endif

PHP_MINIT_FUNCTION(ece) {
   return SUCCESS;
}

PHP_MINFO_FUNCTION(ece) {}

PHP_MSHUTDOWN_FUNCTION(ece) {
   return SUCCESS;
}

PHP_FUNCTION(ece_hello_world) {
   php_error_docref(NULL, E_WARNING, "Not implemented yet");
}
