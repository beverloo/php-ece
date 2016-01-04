--TEST--
AES-GCM-128 invalid data
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
$input = 'Hello, world!';
$key = ece_random_bytes(16);
$nonce = ece_random_bytes(12);

$ciphertext = ece_aesgcm128_encrypt($input, $key, $nonce);

// Modify the encrypted content of the |$ciphertext|.
$invalidContentCiphertext = $ciphertext;
$invalidContentCiphertext[0] = 'A';

var_dump(ece_aesgcm128_decrypt($invalidContentCiphertext, $key, $nonce));

// Modify the authentication tag in |$ciphertext|.
$invalidAuthenticationTagCiphertext = $ciphertext;
$invalidAuthenticationTagCiphertext[strlen($ciphertext) - 1] = 'A';

var_dump(ece_aesgcm128_decrypt($invalidAuthenticationTagCiphertext, $key, $nonce));
?>
--EXPECTF--
Warning: ece_aesgcm128_decrypt(): unable to decrypt or authenticate the ciphertext in %s on line 12
bool(false)

Warning: ece_aesgcm128_decrypt(): unable to decrypt or authenticate the ciphertext in %s on line 18
bool(false)
