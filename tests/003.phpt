--TEST--
ece_p256_import()
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
// Invalid usages.
ece_p256_import();
ece_p256_import(null);
ece_p256_import([]);
ece_p256_import("", null);
ece_p256_import("", []);
ece_p256_import("", "");
ece_p256_import("foo", "bar");

// Generate a new valid P-256 key pair, export it, then import it.
$keys = ece_p256_generate();
$pair = ece_p256_export($keys);

$keys2 = ece_p256_import($pair['public'], $pair['private']);
$pair2 = ece_p256_export($keys2);

echo ($pair['public'] === $pair2['public'] ? 'Equal' : 'Different') . PHP_EOL;
echo ($pair['private'] === $pair2['private'] ? 'Equal' : 'Different') . PHP_EOL;

$keys3 = ece_p256_import($pair['public']);
$pair3 = ece_p256_export($keys3);

echo ($pair['public'] === $pair3['public'] ? 'Equal' : 'Different') . PHP_EOL;
echo ($pair['private'] === $pair3['private'] ? 'Equal' : 'Different') . PHP_EOL;

if (is_null($pair3['private']))
  echo 'NULL';

ece_p256_free($keys3);
ece_p256_free($keys2);
ece_p256_free($keys);
?>
--EXPECTF--
Warning: ece_p256_import() expects at least 1 parameter, 0 given in %s on line 3

Warning: ece_p256_import(): unable to import the public key: must be 65 bytes and start with 0x04 in %s on line 4

Warning: ece_p256_import() expects parameter 1 to be string, array given in %s on line 5

Warning: ece_p256_import(): unable to import the public key: must be 65 bytes and start with 0x04 in %s on line 6

Warning: ece_p256_import() expects parameter 2 to be string, array given in %s on line 7

Warning: ece_p256_import(): unable to import the public key: must be 65 bytes and start with 0x04 in %s on line 8

Warning: ece_p256_import(): unable to import the public key: must be 65 bytes and start with 0x04 in %s on line 9
Equal
Equal
Equal
Different
NULL
