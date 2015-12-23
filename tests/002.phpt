--TEST--
ece_p256_generate()
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
$keys = [
  ece_p256_generate(),
  ece_p256_generate()
];

echo $keys[0] . "\n";
echo $keys[1] . "\n";

echo $keys[0] === $keys[1] ? 'Equal' : 'Different';

foreach ($keys as $key)
  ece_p256_free($key);
?>
--EXPECTF--
Resource id #%d
Resource id #%d
Different
