--TEST--
ece_p256_compute_key()
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
$local_keys = ece_p256_generate();
if (!is_resource($local_keys))
  exit;

$peer_keys = ece_p256_generate();
if (!is_resource($peer_keys))
  exit;

// Compute |local| -> |peer|.
$shared = ece_p256_compute_key($local_keys, $peer_keys);
if (!is_string($shared))
  exit;

echo 'Shared key: ' . strlen($shared) . ' bytes' . PHP_EOL;

// Compute |peer| -> |local|.
$shared2 = ece_p256_compute_key($peer_keys, $local_keys);
if (!is_string($shared))
  exit;

echo ($shared === $shared2) ? 'Equal' : 'Different';

ece_p256_free($local_keys);
ece_p256_free($peer_keys);
?>
--EXPECT--
Shared key: 32 bytes
Equal
