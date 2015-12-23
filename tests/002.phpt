--TEST--
ece_p256_generate()
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
echo ece_p256_generate();
?>
--EXPECTF--
Resource id #%d
