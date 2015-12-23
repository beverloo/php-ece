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

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_random_bytes, 0, 0, 1)
  ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

const zend_function_entry ece_functions[] = {
  PHP_FE(ece_p256_generate,     arginfo_ece_p256_generate)
  PHP_FE(ece_p256_import,       arginfo_ece_p256_import)
  PHP_FE(ece_p256_export,       arginfo_ece_p256_export)
  PHP_FE(ece_p256_compute_key,  arginfo_ece_p256_compute_key)
  PHP_FE(ece_p256_free,         arginfo_ece_p256_free)

  PHP_FE(ece_random_bytes,      arginfo_ece_random_bytes)
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
// Constants

// A P-256 field element consists of 32 bytes.
const size_t kFieldBytes = 32;

// A P-256 point in uncompressed form consists of 0x04 (to denote that the point
// is uncompressed per SEC1 2.3.3) followed by two, 32-byte field elements.
const size_t kUncompressedPointBytes = 65;

// -----------------------------------------------------------------------------
// Error messages

const char kInvalidKeyParameter[] =
    "supplied resource is not a valid P-256 key pair resource";

const char kGenerateKeyCurveError[] =
    "unable to create a new key using the P-256 curve";
const char kGenerateKeyGenerateError[] =
    "unable to generate a new key using the P-256 curve";

const char kExportAllocationError[] =
    "unable to allocate a buffer for exporting a key pair";
const char kExportPublicKeyBignumError[] =
    "unable to export the public key: cannot write the bignum";
const char kExportPublicKeyInvalidFields[] =
    "unable to export the public key: invalid field bytes";
const char kExportPublicKeyFailed[] =
    "unable to export the public key: cannot get affine coordinates";
const char kExportPrivateKeyBignumError[] =
    "unable to export the public key: cannot write the bignum";

const char kRandomBytesRangeError[] =
    "the length must be in range of [1, 8192]";
const char kRandomBytesAllocationError[] =
    "unable to allocate a buffer for the cryptographically secure random bytes";
const char kRandomBytesEntropyError[] =
    "not sufficient entropy available to generate cryptographically secure "
    "random bytes";

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
// Function implementations (P-256)

PHP_FUNCTION(ece_p256_generate) {
  EC_KEY* key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if (!key) {
    php_error_docref(NULL, E_ERROR, kGenerateKeyCurveError);
    return;
  }

  if (!EC_KEY_generate_key(key)) {
    php_error_docref(NULL, E_ERROR, kGenerateKeyGenerateError);
    return;
  }

  RETURN_RES(zend_register_resource(key, le_ec_key));
}

PHP_FUNCTION(ece_p256_import) {
  php_error_docref(NULL, E_WARNING, "Not implemented yet");
}

PHP_FUNCTION(ece_p256_export) {
  zval* value;
  EC_KEY* key;

  zend_string* public_key_string = NULL;
  zend_string* private_key_string = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE)
    return;

  key = php_ec_key_from_zval(value);
  if (key == NULL) 
    RETURN_FALSE;

  array_init(return_value);

  // Export the public key in |key| in uncompressed form per SEC1 2.3.3. It's a
  // 65-byte sequence of bytes that starts with 0x04, followed by two 32-byte
  // field points representing the x/y coordinates on the curve.
  {
    const EC_GROUP* group = EC_KEY_get0_group(key);
    const EC_POINT* public_key = EC_KEY_get0_public_key(key);
    
    BIGNUM* x = BN_new();
    BIGNUM* y = BN_new();

    int success = 0;

    if (!x || !y) {
      php_error_docref(NULL, E_ERROR, kExportAllocationError);
      return;
    }

    if (EC_POINT_get_affine_coordinates_GFp(group, public_key, x, y, NULL)) {
      if (BN_num_bytes(x) == kFieldBytes || BN_num_bytes(y) == kFieldBytes) {
        public_key_string = zend_string_alloc(kUncompressedPointBytes, 0);
        if (public_key_string) {
          unsigned char* buffer = (unsigned char*) public_key_string->val;

          buffer[0] = 0x04;
          if (BN_bn2bin(x, &buffer[1]) == kFieldBytes &&
              BN_bn2bin(y, &buffer[1 + kFieldBytes]) == kFieldBytes) {
            success = 1;
          } else {
            php_error_docref(NULL, E_ERROR, kExportPublicKeyBignumError);
          }
        } else {
          php_error_docref(NULL, E_ERROR, kExportAllocationError);
        }
      } else {
        php_error_docref(NULL, E_ERROR, kExportPublicKeyInvalidFields);
      }
    } else {
      php_error_docref(NULL, E_ERROR, kExportPublicKeyFailed);
    }

    BN_free(x);
    BN_free(y);

    if (!success) {
      if (public_key_string)
        zend_string_free(public_key_string);

      return;
    }
  }

  // Export the private key in |public|, when set, as the 32 bytes representing
  // the private field point (`w` if it were exported as a JWK key).
  {
    const BIGNUM* private_key = EC_KEY_get0_private_key(key);
    if (private_key) {
      private_key_string = zend_string_alloc(kFieldBytes, 0);
      if (private_key_string) {
        unsigned char* buffer = (unsigned char*) private_key_string->val;
        if (BN_bn2bin(private_key, buffer) != kFieldBytes) {
          php_error_docref(NULL, E_ERROR, kExportPrivateKeyBignumError);
          return;
        }
      } else {
        php_error_docref(NULL, E_ERROR, kExportAllocationError);
      }
    }
  }

  add_assoc_str(return_value, "public", public_key_string);
  if (private_key_string)
    add_assoc_str(return_value, "private", private_key_string);
  else
    add_assoc_null(return_value, "private");
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

// -----------------------------------------------------------------------------
// Function implementations (miscellaneous)

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
