--TEST--
ece_p256_export()
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
$keys = ece_p256_generate();
if (!is_resource($keys))
  exit;

// Invalid usages.
ece_p256_export();
ece_p256_export(null);
ece_p256_export("hello");
ece_p256_export(42);

echo ece_p256_export($keys);
?>
--EXPECTF--
Warning: ece_p256_export() expects exactly 1 parameter, 0 given in %s on line %d

Warning: ece_p256_export(): expects parameter 1 to be a P-256 key resource in %s on line %d

Warning: ece_p256_export(): expects parameter 1 to be a P-256 key resource in %s on line %d

Warning: ece_p256_export(): expects parameter 1 to be a P-256 key resource in %s on line %d
42
