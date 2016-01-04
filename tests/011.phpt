--TEST--
AES-GCM-128 reference tests
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
// Reference vectors created using the Chrome Web Crypto implementation. The
// vectors contain the base64 encoded ciphertext, key and nonce and a plaintext.
// All vectors test both AES-GCM-128 encryption and decryption.
$vectors = [
  [ 'plaintext' => '',
    'key' => 'qY64UbVMga48RsY2NZWX5A==',
    'nonce' => 'xiqCl4hk5I4EsDA1',
    'ciphertext' => 'c9pHmgjYBws2icorKwL4Vg==' ],
  [ 'plaintext' => '',
    'key' => 'IaSmNZFtnVrz+XPxsetnSw==',
    'nonce' => 'zVJjTVxDE0DloG/2',
    'ciphertext' => 'Piq6Q+2wW3iG8MWiVhRICw==' ],
  [ 'plaintext' => 'SGVsbG8sIHdvcmxkIQ==',
    'key' => 'zUo6G8jYEwZ2YMxPiDgITA==',
    'nonce' => 'zXSFAnEgfHOYqOyu',
    'ciphertext' => '+NWF+uCPrFf9huZcLGNN5r+WebPQt/Pm0vslWqU=' ],
  [ 'plaintext' => 'SGVsbG8sIHdvcmxkIQ==',
    'key' => 'z6QC7lceRunyNQXJxlw0kw==',
    'nonce' => 'uBG5+WSCNXpILmIE',
    'ciphertext' => 'O3qhp596GtrlXbLbPVdnrXycfRcFGEJC2h6P7EQ=' ],
  [ 'plaintext' => '2YWdUV3pjzUoDBYmFttXjeWh',
    'key' => 'dcCKoKcPn5B1uPNsION/bg==',
    'nonce' => 'hgNt+hv0+EjKIyBX',
    'ciphertext' => 'Yw+bChEKAvtG2pUlZP++uIQbN/wYnPZ+cKMTFzKm5F50qw==' ],
  [ 'plaintext' => 'wCn71xRA9B0VYXGV6d7Ctqx6',
    'key' => '7W9r27HKfGKvwdc0nCc5bQ==',
    'nonce' => '/QoU0TdqO/eqlF53',
    'ciphertext' => '6aRPirkcqzrTvxM45Igo2FCSbesbj6psG6OFa2kCG8Dy3w==' ],
  [ 'plaintext' => 'RW5jcnlwdGVkIENvbnRlbnQgRW5jb2Rpbmc=',
    'key' => '8ZuvWK2ZCpMEv9A9208zXA==',
    'nonce' => 'hEYJQjgvL7LfYIPx',
    'ciphertext' => 'y7rXFyLq45U3ZPk+FrUaLkifjt8Ejl57cQF9PwWwakBi6XFrQVJlqjjY' ],
  [ 'plaintext' => 'RW5jcnlwdGVkIENvbnRlbnQgRW5jb2Rpbmc=',
    'key' => 'm+VRwBh+9UZ6tuISCYGU1w==',
    'nonce' => 'Y8rBYhPU+4uT8q6J',
    'ciphertext' => 'wFda0n7hRfBkKyfCD655Jz0WCFbHvdfB9w8cWD9QG14XOiEpGeRn7dDh' ],
  [ 'plaintext' => 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
    'key' => 'L9PMiTyB6dSYx252p0kotQ==',
    'nonce' => 'EaUs0YqrROKscYbW',
    'ciphertext' => 'P93JkK+zH45WoCW2y9NnOnRFBivsChPj932YbN8TUYnow471/lNP7w==' ],
  [ 'plaintext' => '////////////////////////////////',
    'key' => 'n55uO64JDuO5Y0qvLDmeFA==',
    'nonce' => 'HHKOPSBS9tpjE/NU',
    'ciphertext' => 'z6y5yDnI9O/UegPDqfYkR6f2Qv84d36JAJJycOiuImNbLMmYkkpNzA==' ],
];

foreach ($vectors as $i => $vector) {
  $expectedPlaintext = base64_decode($vector['plaintext']);
  $key = base64_decode($vector['key']);
  $nonce = base64_decode($vector['nonce']);
  $expectedCiphertext = base64_decode($vector['ciphertext']);

  $ciphertext = ece_aesgcm128_encrypt($expectedPlaintext, $key, $nonce);
  if ($ciphertext != $expectedCiphertext) {
    echo 'Unexpected ciphertext for vector #' . $i . PHP_EOL;
    continue;
  }

  $plaintext = ece_aesgcm128_decrypt($expectedCiphertext, $key, $nonce);
  if ($plaintext != $expectedPlaintext) {
    echo 'Unexpected plaintext for vector #' . $i . PHP_EOL;
    continue;
  }

  echo 'Success for vector #' . $i . PHP_EOL;
}
?>
--EXPECT--
Success for vector #0
Success for vector #1
Success for vector #2
Success for vector #3
Success for vector #4
Success for vector #5
Success for vector #6
Success for vector #7
Success for vector #8
Success for vector #9
