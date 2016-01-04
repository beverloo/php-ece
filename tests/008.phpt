--TEST--
AES-GCM-128 translation round-trips
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
$original = "hello, world!";
$key = "foo";
$nonce = "bar";

$ciphertext = ece_aesgcm128_encrypt($original, $key, $nonce);
var_dump($ciphertext);

$plaintext = ece_aesgcm128_decrypt($ciphertext, $key, $nonce);
var_dump($plaintext);

?>
--EXPECT--
bool(false)
bool(false)
