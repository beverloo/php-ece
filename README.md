# Encrypted Content-Encoding PHP extension

This PHP extension provides the primitives required to implement and use the
IETF Encrypted Content-Encoding draft in PHP.

https://tools.ietf.org/html/draft-thomson-http-encryption

This extension is not meant to be used for production usage.

## Functions

The following functions are available as part of this extension.

- **P-256 Functions**
  - [ece_p256_generate()](#resource-ece_p256_generate)
  - [ece_p256_import(string $public[, string $private])]
(#resource-ece_p256_importstring-public-string-private)
  - [ece_p256_export(resource $key)](#array-ece_p256_exportresource-key)
  - [ece_p256_compute_key(resource $local_pair, resource $peer_pair)]
(#string-ece_p256_compute_keyresource-local_pair-resource-peer_pair)
  - [ece_p256_free(resource $key)](#array-ece_p256_freeresource-key)
- **Miscellaneous**
  - [ece_random_bytes(int $length)](#string-ece_random_bytesint-length)


#### resource ece_p256_generate();

Generates and returns a new P-256 key pair.

Returns NULL and displays an error if the pair could not be generated for any
reason.

#### resource ece_p256_import(string $public[, string $private]);

Imports `$public` (and optionally `$private`) to a new P-256 key pair. Returns
NULL and displays an error if the pair could not be imported for any reason.

The `$public` key must be given as a string holding the raw bytes of a P-256
public key in uncompressed form per SEC1 2.3.3.

The `$private` key, when given, must be a string holding the raw bytes of the
32-byte long field element representing the private value. It will be verified
that the `$public` and `$private` keys are indeed a pair.

#### array ece_p256_export(resource $key);

Exports the `$key` to an array with two keys: `public` set to a string holding
the raw bytes of the P-256 public key in `$key` in uncompressed form per SEC1
2.3.3, and `private` set to a string holding the raw bytes of the private value.

If the `$key` does not have a private value, the `private` key in the returned
array will instead be set to NULL.

#### string ece_p256_compute_key(resource $local_pair, resource $peer_pair);

Computes the shared key between the `$local_pair` and the `$peer_pair`, returns
the key as a string holding the 32-byte long shared secret.

The `$local_pair` must have both the public and private key set. Only the public
key of the `$peer_pair` is required.

#### array ece_p256_free(resource $key);

Frees the P-256 key in `$key`.


#### string ece_random_bytes(int $length);

Creates and returns a string with `$length` cryptographically secure random
bytes. Will fail and display an error if there is not sufficient entropy
available.
