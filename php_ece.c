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

#include <stdlib.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_random_bytes, 0, 0, 1)
  ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ece_p256_generate, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_p256_import, 0, 0, 1)
  ZEND_ARG_INFO(0, public)
  ZEND_ARG_INFO(0, private)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_p256_export, 0, 0, 1)
  ZEND_ARG_INFO(0, pair)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_p256_compute_key, 0, 0, 2)
  ZEND_ARG_INFO(0, local_pair)
  ZEND_ARG_INFO(0, peer_pair)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_p256_free, 0, 0, 1)
  ZEND_ARG_INFO(0, pair)
ZEND_END_ARG_INFO()

const zend_function_entry ece_functions[] = {
  PHP_FE(ece_random_bytes,      arginfo_ece_random_bytes)

  PHP_FE(ece_p256_generate,     arginfo_ece_p256_generate)
  PHP_FE(ece_p256_import,       arginfo_ece_p256_import)
  PHP_FE(ece_p256_export,       arginfo_ece_p256_export)
  PHP_FE(ece_p256_compute_key,  arginfo_ece_p256_compute_key)
  PHP_FE(ece_p256_free,         arginfo_ece_p256_free)
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

// -----------------------------------------------------------------------------
// Error messages

const char kRandomBytesRangeError[] =
    "The number of cryptographically secure random bytes to generate must be "
    "in range of [1, 8192]";
const char kRandomBytesAllocationError[] =
    "Unable to allocate a buffer for the cryptographically secure random bytes";
const char kRandomBytesEntropyError[] =
    "Not sufficient entropy available to generate cryptographically secure "
    "random bytes";

const char kInvalidKeyParameter[] =
    "supplied resource is not a valid P-256 key pair resource";

// -----------------------------------------------------------------------------
// Type definitions (P-256 key)

static int le_ec_key;

static void php_ec_key_free(zend_resource* resource) {
  EC_KEY* key = (EC_KEY*) resource->ptr;
  
  // TODO: Clean up the OpenSSL types it owns.

  resource->ptr = NULL;
  EC_KEY_free(key);
}

static EC_KEY* php_ec_key_from_zval(zval* value) {
  EC_KEY* key = NULL;
  if (Z_TYPE_P(value) == IS_RESOURCE) {
    zend_resource* resource = Z_RES_P(value);

    // zend_fetch_resource will return NULL if the cast is unsuccessful.
    key = zend_fetch_resource(resource, "P-256 key pair", le_ec_key);

  } else {
    php_error_docref(NULL, E_WARNING, kInvalidKeyParameter);
  }

  return key;
}

// -----------------------------------------------------------------------------
// Extension lifetime functions

PHP_MINIT_FUNCTION(ece) {
  le_ec_key = zend_register_list_destructors_ex(
      php_ec_key_free, NULL, "P-256 key pair", module_number);

  return SUCCESS;
}

PHP_MINFO_FUNCTION(ece) {}

PHP_MSHUTDOWN_FUNCTION(ece) {
  return SUCCESS;
}

// -----------------------------------------------------------------------------
// Utility functions

// Creates a buffer of |length| cryptographically secure random bytes. An error
// will be thrown if the bytes could not be generated, for example because the
// PRNG doesn't have enough entropy. Ownership of the created string is passed.
static zend_string* GenerateCryptographicallySecureRandomBytes(size_t length) {
  zend_string* buffer = zend_string_alloc(length, 0);
  if (!buffer) {
    php_error_docref(NULL, E_ERROR, kRandomBytesAllocationError);
    return NULL;
  }

  if (RAND_bytes(buffer->val, length) != 1) {
    php_error_docref(NULL, E_ERROR, kRandomBytesEntropyError);
    zend_string_release(buffer);
    return NULL;
  }

  return buffer;
}

// -----------------------------------------------------------------------------
// Function implementations (utilities)

PHP_FUNCTION(ece_random_bytes) {
  zend_long length;
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &length) == FAILURE)
    return;

  if (length <= 0 || length > 8192) {
    php_error_docref(NULL, E_WARNING, kRandomBytesRangeError);
    RETURN_FALSE;
  }

  zend_string* buffer = GenerateCryptographicallySecureRandomBytes(length);
  if (buffer)
    RETVAL_STR(buffer);
  else
    RETURN_FALSE;
}

// -----------------------------------------------------------------------------
// Function implementations (P-256)

PHP_FUNCTION(ece_p256_generate) {
  EC_KEY* key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if (key)
    RETURN_RES(zend_register_resource(key, le_ec_key));

  RETURN_FALSE;
}

PHP_FUNCTION(ece_p256_import) {
  php_error_docref(NULL, E_WARNING, "Not implemented yet");
}

PHP_FUNCTION(ece_p256_export) {
  zval* value;
  EC_KEY* key;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE)
    return;

  key = php_ec_key_from_zval(value);
  if (key == NULL) 
    RETURN_FALSE;

  RETVAL_LONG(42);
}

PHP_FUNCTION(ece_p256_compute_key) {
  php_error_docref(NULL, E_WARNING, "Not implemented yet");
}

PHP_FUNCTION(ece_p256_free) {
  zval* value;
  EC_KEY* key;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE)
    return;

  key = php_ec_key_from_zval(value);
  if (key)
    zend_list_close(Z_RES_P(value));
}
