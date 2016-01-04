--TEST--
AES-GCM-128 argument validation
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
$plaintext = 'Hello, world!';
$ciphertext = ece_random_bytes(24);  // this length is not significant
$key = ece_random_bytes(16);
$nonce = ece_random_bytes(12);

// Not enough arguments.
ece_aesgcm128_encrypt();
ece_aesgcm128_decrypt();

ece_aesgcm128_encrypt($plaintext);
ece_aesgcm128_decrypt($ciphertext);

ece_aesgcm128_encrypt($plaintext, $key);
ece_aesgcm128_decrypt($ciphertext, $key);

// Invalid argument types.
ece_aesgcm128_encrypt($plaintext, $key, new stdClass());
ece_aesgcm128_decrypt($ciphertext, $key, new stdClass());

ece_aesgcm128_encrypt($plaintext, new stdClass(), $nonce);
ece_aesgcm128_decrypt($ciphertext, new stdClass(), $nonce);

ece_aesgcm128_encrypt([], $key, $nonce);
ece_aesgcm128_decrypt([], $key, $nonce);

// Invalid key and nonce sizes.
ece_aesgcm128_encrypt($plaintext, ece_random_bytes(42), $nonce);
ece_aesgcm128_decrypt($ciphertext, ece_random_bytes(42), $nonce);

ece_aesgcm128_encrypt($plaintext, $key, ece_random_bytes(42));
ece_aesgcm128_decrypt($ciphertext, $key, ece_random_bytes(42));

// Invalid ciphertext size whilst decrypting.
ece_aesgcm128_decrypt(ece_random_bytes(12), $key, $nonce);

?>
--EXPECTF--
Warning: ece_aesgcm128_encrypt() expects exactly 3 parameters, 0 given in %s on line 8

Warning: ece_aesgcm128_decrypt() expects exactly 3 parameters, 0 given in %s on line 9

Warning: ece_aesgcm128_encrypt() expects exactly 3 parameters, 1 given in %s on line 11

Warning: ece_aesgcm128_decrypt() expects exactly 3 parameters, 1 given in %s on line 12

Warning: ece_aesgcm128_encrypt() expects exactly 3 parameters, 2 given in %s on line 14

Warning: ece_aesgcm128_decrypt() expects exactly 3 parameters, 2 given in %s on line 15

Warning: ece_aesgcm128_encrypt() expects parameter 3 to be string, object given in %s on line 18

Warning: ece_aesgcm128_decrypt() expects parameter 3 to be string, object given in %s on line 19

Warning: ece_aesgcm128_encrypt() expects parameter 2 to be string, object given in %s on line 21

Warning: ece_aesgcm128_decrypt() expects parameter 2 to be string, object given in %s on line 22

Warning: ece_aesgcm128_encrypt() expects parameter 1 to be string, array given in %s on line 24

Warning: ece_aesgcm128_decrypt() expects parameter 1 to be string, array given in %s on line 25

Warning: ece_aesgcm128_encrypt(): the $key must be exactly 16 bytes in size in %s on line 28

Warning: ece_aesgcm128_decrypt(): the $key must be exactly 16 bytes in size in %s on line 29

Warning: ece_aesgcm128_encrypt(): the $nonce must be exactly 12 bytes in size in %s on line 31

Warning: ece_aesgcm128_decrypt(): the $nonce must be exactly 12 bytes in size in %s on line 32

Warning: ece_aesgcm128_decrypt(): the $ciphertext must have at least 16 bytes of data in %s on line 35
