--TEST--
ece_random_bytes()
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
// These are expected to throw errors.
ece_random_bytes();
ece_random_bytes(0);
ece_random_bytes("hello");
ece_random_bytes(999999);

// These are expected to return random bytes.
echo strlen(ece_random_bytes(32)) . "\n";
echo strlen(ece_random_bytes(64)) . "\n";
echo strlen(ece_random_bytes(50)) . "\n";
echo strlen(ece_random_bytes(1)) . "\n";

// Make sure that the returned values are not equal to each other.
echo ece_random_bytes(10) == ece_random_bytes(10) ? 'Equal' : 'Different';
?>
--EXPECTF--
Warning: ece_random_bytes() expects exactly 1 parameter, 0 given in %s on line %d

Warning: ece_random_bytes(): The number of cryptographically secure random bytes to generate must be in range of [1, 8192] in %s on line %d

Warning: ece_random_bytes() expects parameter 1 to be integer, string given in %s on line %d

Warning: ece_random_bytes(): The number of cryptographically secure random bytes to generate must be in range of [1, 8192] in %s on line %d
32
64
50
1
Different
