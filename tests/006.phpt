--TEST--
ece_p256_free()
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
// Invalid usages.
ece_p256_free();
ece_p256_free(null);
ece_p256_free("hello");
ece_p256_free(42);

ece_p256_free(ece_p256_generate());

// Freed keys cannot be used anymore.
$keys = ece_p256_generate();

ece_p256_export($keys);
ece_p256_free($keys);

ece_p256_export($keys);
ece_p256_free($keys);
?>
--EXPECTF--
Warning: ece_p256_free() expects exactly 1 parameter, 0 given in %s on line 3

Warning: ece_p256_free(): supplied resource is not a valid P-256 key pair resource in %s on line 4

Warning: ece_p256_free(): supplied resource is not a valid P-256 key pair resource in %s on line 5

Warning: ece_p256_free(): supplied resource is not a valid P-256 key pair resource in %s on line 6

Warning: ece_p256_export(): supplied resource is not a valid P-256 key pair resource in %s on line 16

Warning: ece_p256_free(): supplied resource is not a valid P-256 key pair resource in %s on line 17
