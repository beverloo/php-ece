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
#include <openssl/ecdh.h>
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_aesgcm128_encrypt, 0, 0, 3)
  ZEND_ARG_INFO(0, plaintext)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, nonce)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ece_aesgcm128_decrypt, 0, 0, 3)
  ZEND_ARG_INFO(0, ciphertext)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, nonce)
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

  PHP_FE(ece_aesgcm128_encrypt, arginfo_ece_aesgcm128_encrypt)
  PHP_FE(ece_aesgcm128_decrypt, arginfo_ece_aesgcm128_decrypt)

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

// Directionality constants for translating aes-gcm-128 content.
const int TRANSLATE_ENCRYPT = 0;
const int TRANSLATE_DECRYPT = 1;

// A P-256 field element consists of 32 bytes.
const size_t kFieldBytes = 32;

// A P-256 point in uncompressed form consists of 0x04 (to denote that the point
// is uncompressed per SEC1 2.3.3) followed by two, 32-byte field elements.
const size_t kUncompressedPointBytes = 65;

// Number of bytes of keying material expected by the aesgcm128 functions.
const size_t kAesGcmKeyBytes = 16;

// Number of bytes of nonce expected by the aesgcm128 functions.
const size_t kAesGcmNonceBytes = 12;

// -----------------------------------------------------------------------------
// Error messages

const char kInvalidKeyParameter[] =
    "supplied resource is not a valid P-256 key pair resource";

const char kGenerateKeyCurveError[] =
    "unable to create a new key using the P-256 curve";
const char kGenerateKeyGenerateError[] =
    "unable to generate a new key using the P-256 curve";

const char kImportInvalidPublicSize[] =
    "unable to import the public key: must be 65 bytes and start with 0x04";
const char kImportInvalidPrivateSize[] =
    "unable to import the private key: must be 32 bytes long";
const char kImportAllocationError[] =
    "unable to allocate a buffer for importing the keys";
const char kImportInvalidPrivateKey[] =
    "unable to import the private key, is it a P-256 private value?";
const char kImportInvalidPublicKey[] =
    "unable to import and validate the public key";

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

const char kComputeKeyNoPrivate[] =
    "the local key-pair must have both a public and a private key";
const char kComputeKeyNoPublic[] =
    "cannot extract the public key from the peer's key pair";
const char kComputeKeyFailed[] =
    "cannot compute the shared secret between the two keys";

const char kAesGcmInvalidKey[] =
    "the $key must be exactly 16 bytes in size";
const char kAesGcmInvalidNonce[] =
    "the $nonce must be exactly 12 bytes in size";
const char kAesGcmInvalidCiphertext[] =
    "the $ciphertext must have at least 16 bytes of data";

const char kTranslateInitGcmError[] =
    "unable to initialize the OpenSSL AES-GCM-128 cipher";
const char kTranslateKeyNonceError[] =
    "unable to import the key and nonce in the AES-GCM-128 cipher";
const char kTranslateAllocationError[] =
    "unable to allocate a buffer for the result string";
const char kTranslateEncryptInputError[] = "unable to encrypt the input data";
const char kTranslateEncryptAuthError[] =
    "unable to export the authentication tag";
const char kTranslateDecryptAuthError[] =
    "unable to import the authentication tag";
const char kTranslateDecryptionWarning[] =
    "unable to decrypt or authenticate the ciphertext";

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

PHP_MINFO_FUNCTION(ece) {
  php_info_print_table_start();
  php_info_print_table_row(2, "Encrypted Content Encoding support", "enabled");
  php_info_print_table_row(2, "ECE version", PHP_ECE_VERSION);
  php_info_print_table_row(2, "OpenSSL Library Version", SSLeay_version(SSLEAY_VERSION));
  php_info_print_table_end();
}

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

// Translates |input| of |input_len| using aes-gcm-128. The |input| will either
// be encrypted or decrypted based on |direction|. The |key|, which must be of
// size |kAesGcmKeyBytes|, and the |nonce|, which must be of size
// |kAesGcmNonceBytes|, will be used for the translation. A new zend string will
// be returned on success, or NULL (with a visible warning) on failure.
static zend_string* AesGcm128Translate(
    int direction, char* input, size_t input_len, char* key, char* nonce) {
  const EVP_CIPHER* aead = EVP_aes_128_gcm();

  zend_string* result = NULL;

  int expected_len = 0;
  int result_len = 0;

  EVP_CIPHER_CTX context;
  EVP_CIPHER_CTX_init(&context);

  do {
    if (direction == TRANSLATE_ENCRYPT) {
      if (EVP_EncryptInit_ex(&context, aead, 0, 0, 0) != 1) {
        php_error_docref(NULL, E_ERROR, kTranslateInitGcmError);
        break;
      }

      if (EVP_CIPHER_CTX_ctrl(&context, EVP_CTRL_GCM_SET_IVLEN, 12, 0) != 1 ||
          EVP_EncryptInit_ex(&context, 0, 0, key, nonce) != 1) {
        php_error_docref(NULL, E_ERROR, kTranslateKeyNonceError);
        break;
      }

      expected_len = input_len + 16 /* authentication tag */;
      result = zend_string_alloc(expected_len, 0);
      if (!result) {
        php_error_docref(NULL, E_ERROR, kTranslateAllocationError);
        break;
      }

      if (EVP_EncryptUpdate(&context, result->val, &result_len, input, input_len) != 1 ||
          EVP_EncryptFinal_ex(&context, result->val, &result_len) != 1) {
        php_error_docref(NULL, E_ERROR, kTranslateEncryptInputError);
        zend_string_release(result);
        result = NULL;
        break;
      }

      if (EVP_CIPHER_CTX_ctrl(&context, EVP_CTRL_GCM_GET_TAG, 16, result->val + input_len) != 1) {
        php_error_docref(NULL, E_ERROR, kTranslateEncryptAuthError);
        zend_string_release(result);
        result = NULL;
        break;
      }

      // Encryption successful!

    } else {
      if (EVP_DecryptInit_ex(&context, aead, 0, 0, 0) != 1) {
        php_error_docref(NULL, E_ERROR, kTranslateInitGcmError);
        break;
      }

      expected_len = input_len - 16;

      if (EVP_CIPHER_CTX_ctrl(&context, EVP_CTRL_GCM_SET_TAG, 16, input + expected_len) != 1) {
        php_error_docref(NULL, E_ERROR, kTranslateDecryptAuthError);
        break;
      }

      if (EVP_CIPHER_CTX_ctrl(&context, EVP_CTRL_GCM_SET_IVLEN, 12, 0) != 1 ||
          EVP_DecryptInit_ex(&context, 0, 0, key, nonce) != 1) {
        php_error_docref(NULL, E_ERROR, kTranslateKeyNonceError);
        break;
      }

      result = zend_string_alloc(expected_len, 0);
      if (!result) {
        php_error_docref(NULL, E_ERROR, kTranslateAllocationError);
        break;
      }

      if (EVP_DecryptUpdate(&context, result->val, &result_len, input, expected_len) != 1 ||
          EVP_DecryptFinal_ex(&context, result->val + expected_len, &result_len) != 1) {
        php_error_docref(NULL, E_WARNING, kTranslateDecryptionWarning);

        zend_string_release(result);
        result = NULL;
        break;
      }

      // Decryption successful!
    }
  } while(0);

  EVP_CIPHER_CTX_cleanup(&context);

  return result;
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
  char* public_value, * private_value;
  size_t public_len = 0, private_len = 0;

  EC_KEY* key = NULL;
  int success = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &public_value, &public_len, &private_value,
                            &private_len) == FAILURE) {
    return;
  }

  // Verify that the public key is 65 bytes long and starts with 0x04.
  if (public_len != kUncompressedPointBytes || public_value[0] != 0x04) {
    php_error_docref(NULL, E_WARNING, kImportInvalidPublicSize);
    RETURN_FALSE;
  }

  // Verify that the private key is either empty, or 32 bytes long.
  if (private_len != 0 && private_len != kFieldBytes) {
    php_error_docref(NULL, E_WARNING, kImportInvalidPrivateSize);
    RETURN_FALSE;
  }

  key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if (!key) {
    php_error_docref(NULL, E_ERROR, kImportAllocationError);
    return;
  }

  // Import the |private_value|, when available, to the |key|.
  if (private_len == kFieldBytes) {
    unsigned char* private_buffer = (unsigned char*) private_value;

    BIGNUM* w = BN_bin2bn(private_buffer, kFieldBytes, NULL);
    if (!w) {
      php_error_docref(NULL, E_ERROR, kImportAllocationError);
      return;
    }

    if (!EC_KEY_set_private_key(key, w)) {
      php_error_docref(NULL, E_WARNING, kImportInvalidPrivateKey);
      return;
    }

    BN_free(w);
  }

  // Import the |public_value| to the |key|.
  {
    unsigned char* public_buffer = (unsigned char*) public_value;

    BIGNUM* x = BN_bin2bn(&public_buffer[1], kFieldBytes, NULL);
    BIGNUM* y = BN_bin2bn(&public_buffer[1 + kFieldBytes], kFieldBytes, NULL);

    if (!x || !y) {
      php_error_docref(NULL, E_ERROR, kImportAllocationError);

      if (x) BN_free(x);
      if (y) BN_free(y);
      return;
    }

    if (EC_KEY_set_public_key_affine_coordinates(key, x, y) != 1)
      php_error_docref(NULL, E_WARNING, kImportInvalidPublicKey);
    else
      success = 1;

    BN_free(x);
    BN_free(y);
  }

  if (!success) {
    EC_KEY_free(key);
    RETURN_FALSE;
  }

  RETURN_RES(zend_register_resource(key, le_ec_key));
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

      if (x) BN_free(x);
      if (y) BN_free(y);
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
  zval* local_value, * peer_value;
  EC_KEY* local_key, * peer_key;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &local_value, &peer_value)
          == FAILURE) {
    return;
  }

  local_key = php_ec_key_from_zval(local_value);
  peer_key = php_ec_key_from_zval(peer_value);

  if (local_key == NULL || peer_key == NULL) 
    RETURN_FALSE;

  {
    const BIGNUM* private_key = EC_KEY_get0_private_key(local_key);
    const EC_POINT* public_key = EC_KEY_get0_public_key(peer_key);
    zend_string* result = NULL;

    // Verify that the |local_key| has a private key. The |peer_key| only needs
    // the public key set (which will always be the case).
    if (!private_key) {
      php_error_docref(NULL, E_WARNING, kComputeKeyNoPrivate);
      RETURN_FALSE;
    }

    // This should never happen. Why would this happen?
    if (!public_key) {
      php_error_docref(NULL, E_ERROR, kComputeKeyNoPublic);
      RETURN_FALSE;
    }

    result = zend_string_alloc(kFieldBytes, 0);
    if (result) {
      unsigned char* buffer = (unsigned char*) result->val;
      if (ECDH_compute_key(buffer, kFieldBytes, public_key, local_key, NULL)
              == kFieldBytes) {
        RETURN_STR(result);
      }

      php_error_docref(NULL, E_WARNING, kComputeKeyFailed);
    }
  }

  RETURN_FALSE;
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
// Function implementations (AES-GCM-128)

PHP_FUNCTION(ece_aesgcm128_encrypt) {
  char* plaintext, * key, * nonce;
  size_t plaintext_len = 0, key_len = 0, nonce_len = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss", &plaintext, &plaintext_len,
                            &key, &key_len, &nonce, &nonce_len) == FAILURE) {
    RETURN_FALSE;
  }

  // Confirm that the |$key| has the right amount of data.
  if (key_len != kAesGcmKeyBytes) {
    php_error_docref(NULL, E_WARNING, kAesGcmInvalidKey);
    RETURN_FALSE;
  }

  // Confirm that the |$nonce| has the right amount of data.
  if (nonce_len != kAesGcmNonceBytes) {
    php_error_docref(NULL, E_WARNING, kAesGcmInvalidNonce);
    RETURN_FALSE;
  }

  zend_string* ciphertext = AesGcm128Translate(TRANSLATE_ENCRYPT, plaintext,
                                               plaintext_len, key, nonce);

  if (ciphertext)
    RETVAL_STR(ciphertext);
  else
    RETURN_FALSE;
}

PHP_FUNCTION(ece_aesgcm128_decrypt) {
  char* ciphertext, * key, * nonce;
  size_t ciphertext_len = 0, key_len = 0, nonce_len = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss", &ciphertext, &ciphertext_len,
                            &key, &key_len, &nonce, &nonce_len) == FAILURE) {
    RETURN_FALSE;
  }

  // Confirm that the |$key| has the right amount of data.
  if (key_len != kAesGcmKeyBytes) {
    php_error_docref(NULL, E_WARNING, kAesGcmInvalidKey);
    RETURN_FALSE;
  }

  // Confirm that the |$nonce| has the right amount of data.
  if (nonce_len != kAesGcmNonceBytes) {
    php_error_docref(NULL, E_WARNING, kAesGcmInvalidNonce);
    RETURN_FALSE;
  }

  // Confirm that the |$ciphertext| has at least 16 bytes of data, which is the
  // size of the mandatory authentication tag.
  if (ciphertext_len < 16) {
    php_error_docref(NULL, E_WARNING, kAesGcmInvalidCiphertext);
    RETURN_FALSE;
  }

  zend_string* plaintext = AesGcm128Translate(TRANSLATE_DECRYPT, ciphertext,
                                              ciphertext_len, key, nonce);

  if (plaintext)
    RETVAL_STR(plaintext);
  else
    RETURN_FALSE;
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
