--TEST--
Calling ece_hello_world()!
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
ece_hello_world();
?>
--EXPECTF--
Warning: ece_hello_world(): Not implemented yet in %s on line %d
