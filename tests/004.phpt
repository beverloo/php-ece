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

// Valid key-pair.
$pair = ece_p256_export($keys);

if (!array_key_exists('public', $pair) || !is_string($pair['public']))
  die('The public key did not get exported.');

echo 'Public key: ' . strlen($pair['public']) . ' bytes' . PHP_EOL;
echo 'Private key: ';

if (array_key_exists('private', $pair) && is_string($pair['private']))
  echo strlen($pair['private']) . ' bytes' . NULL;
else
  echo 'NULL' . PHP_EOL;

ece_p256_free($keys);
?>
--EXPECTF--
Warning: ece_p256_export() expects exactly 1 parameter, 0 given in %s on line 7

Warning: ece_p256_export(): supplied resource is not a valid P-256 key pair resource in %s on line 8

Warning: ece_p256_export(): supplied resource is not a valid P-256 key pair resource in %s on line 9

Warning: ece_p256_export(): supplied resource is not a valid P-256 key pair resource in %s on line 10
Public key: 65 bytes
Private key: NULL
