--TEST--
Calling ece_p256_generate()!
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
ece_p256_generate();
?>
--EXPECTF--
Warning: ece_p256_generate(): Not implemented yet in %s on line %d
