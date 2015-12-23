# Encrypted Content-Encoding PHP extension

This PHP extension provides the primitives required to implement and use the
IETF Encrypted Content-Encoding draft in PHP.

https://tools.ietf.org/html/draft-thomson-http-encryption

This extension is not meant to be used for production usage.

## Functions

The following functions are available as part of this extension.

#### resource ece_p256_generate();

Generates a new P-256 key pair. Returns NULL and displays an error if the pair
could not be generated for any reason.

#### resource ece_p256_import(string public[, string private]);

Imports `public` (and optionally `private`) to a new P-256 key pair. Returns
NULL and displays an error if the pair could not be imported for any reason.

The `public` key must be given as a string holding the raw bytes of a P-256
public key in uncompressed form per SEC1 2.3.3.

The `private` key, when given, must be a string holding the raw bytes of the 32-
byte long field element representing the private value. It will be verified that
the `public` and `private` keys are indeed a pair.

#### array ece_p256_export(resource $pair);

Exports the `pair` to an array with two keys: `public` set to a string holding
the raw bytes of the P-256 public key in `pair` in uncompressed form per SEC1
2.3.3, and `private` set to a string holding the raw bytes of the private value.

If the `pair` does not have a private value, it will be set to NULL.

#### string ece_p256_compute_key(resource $local_pair, resource $peer_pair);

Computes the shared key between the `local_pair` and the `peer_pair`, returns
the key as a string holding the 32-byte long shared secret.

The `local_pair` must have both the public and private key set. Only the public
key of the `peer_pair` is required.
