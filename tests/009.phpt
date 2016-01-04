--TEST--
AES-GCM-128 data round-trip
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
$input = 'Hello, world!';
$key = ece_random_bytes(16);
$nonce = ece_random_bytes(12);

$ciphertext = ece_aesgcm128_encrypt($input, $key, $nonce);
$plaintext = ece_aesgcm128_decrypt($ciphertext, $key, $nonce);
var_dump($plaintext);

?>
--EXPECT--
string(13) "Hello, world!"
