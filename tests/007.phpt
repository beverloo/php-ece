--TEST--
NIST P-256 reference tests
--SKIPIF--
<?php if (!extension_loaded("ece")) echo 'skip'; ?>
--FILE--
<?php
// Reference vectors created using the Chrome Web Crypto implementation. The
// vectors contain the base64 encoded public and private keys of both the local
// and peer keys, and the shared secret that should be the resulting key.
$vectors = [
  [ 'local' => [
      'BKrd+A8moL20GX7vSj18I+7Pdjpb8P7bpsPUrmOSbki6Sfnks5OC6UjGS2fbFqiql8jsU4oKmFBQoQh8mTDanak=',
      'NPWe25X9dasjrjdThj12ngF64JFxlwGehWverFTU+Fo=' ],
    'peer' => [
      'BAYdrGr+OtVn/0xt8SOMTtn5ZmT/RMi3DX7/6yhSJRCRzKHQVPdMxjkb+ETdKuilKUe2+p0y4N/u3ZqRxXDMpP8=',
      '6vxgByqBVXgt/5D1SeagpL0oCw0ry9ZBiYipDX5rEZA=' ],
    'shared' => 'QktyfLTId8zLSRlBfM2Li7c+NzsXUH9iKJaCA0uXUIU=' ],
  [ 'local' => [
      'BHU3G5aInVGdGTffKkfhpOz3KFXDDyc3yaJPDjnRz+50dRXs/mDrBnhHSWaZ53701IZo7PqYEc0dQdiTqQcAXvU=',
      '1dWY+yZcoRmOxV3fwX5jN0zRYuyLVj+z8zs309TRBq8=' ],
    'peer' => [
      'BLQRZaiZSgw7q3U1ZPOFXy5W665zMRTVhxKf7fpq6SgLQreUZ+32N9lSyE9rNBxB4pvAvZ/svtmhsdZO27f3y6c=',
      'E87zxpsF7aX2hA7zsrqoBB3xvrOzKFctr9FHHsV0/YI=' ],
    'shared' => 'mW2obeYvlRRmE606GlEDV0x18D6RCYkj2JiZHO0BMZg=' ],
  [ 'local' => [
      'BNN05pXDRedNCugLm2LiG/Fz4HxbqsWY3jjk+Im4HQycByj2PQLGkITKqatgxk3Hh2hlKXIxExQ4Says/k+fe/s=',
      'hvji6qFadXYcqM+oRhiHz9sEsBR9r0WQIsSCqS0U4XE=' ],
    'peer' => [
      'BF0HxMtPhJ2TFuQh01fh7xrjOccFp1ohGNQxQ1nuNNR47brRfD5GbuXDLvcOWCa5MW/yq9JIfcltBjlCfciwLNw=',
      'Y3ad32e+BRRoqrGA3+DqRalTJsZuJK7oC/MeDgBP9b0=' ],
    'shared' => 'VDqV4FQdqyapjpEdP32LFUXBKt06h/HGFA+0PUHN/pY=' ],
  [ 'local' => [
      'BIzzH3uWu5yDN3cic+NwJ+QzcCxcRYSlEHryJP87qQHhehR7lFNa0swTKyzGJcAAw1e/D+7NNkxCFIuXOsVRg/U=',
      'c7F2Glr+EBT8TAJ398Y0Lc0wXKg3dbqh5v21greJdaA=' ],
    'peer' => [
      'BKyjyYtoaIRX0GKranSBxQ88jDthroZdZ5fcv2vXnNiIaJUXgb8UCT8oJPv81tUZKcPuCgbyVglx8+tqjT2kHyM=',
      'zp/gXIulaKAX3N4Bqi0FlteE6Z9ds+zRtrm72aqwwPM=' ],
    'shared' => '4+AuiaAdpO95mKHeZ+iMmzUo85j03OM5pXUCNCv5qWE=' ],
  [ 'local' => [
      'BDAoAfZ1xwF7wqwW3rf2f2beavjZpW0dRFqxiS+vnZTQDdG1jEnhO6VKPdXDnqMCNjfDBeDsg5AGnatH6BNB+HI=',
      'LoD2FQTM00NwHReq8aADKyUWY1HPXxRQSqM1wBOxjtc=' ],
    'peer' => [
      'BCguZbehYXK69TlplN/Jburu8svRLMPJ8U0bAy/8fTm9TMSHg7FW01ehmw3vt0VMhXXYIgClOqe0qNeVF2QcoS8=',
      'bhncgEOy+Cvs9Neg7Yv2xTXnawoWSDe+YK3sjtKuGDM=' ],
    'shared' => 'UKEJfdUa8u4UQq2xrHtwLzLGcyYCVnvkZ/y1TGa4Tig=' ],
  [ 'local' => [
      'BP39yBvi1/MFRLM1JZn0BCdkY2VVOyRFd+V7ZKv1XscTpUZiVMsCrOzVjiZsM8Fbpdfl3H+Z95nZB0owLoNgtgM=',
      '8ZPNifhRueyGVvOOfp8cE27/fCibeJLeRQEDLd7+ixA=' ],
    'peer' => [
      'BInFq+9zvYMEVO34TOl+8Gc6wfu9YlM7YwlLTNOS7ern5pmpvz8zcwDuK5aggGYKrtBS4b7qWCNzR49VeCG6Vmk=',
      'MbO/AzOW00r5t2eC7Uvf1dwj9HQPcS2vQDJ+Kw9Px2U=' ],
    'shared' => 'Nc4LG/1oYfEc01xs+/McWlzMLH6+fW0sqGEV6I2Bnb4=' ],
  [ 'local' => [
      'BE9gBsV7nX49Amc1mNa/Jr8/tStoP8Nx5PQGD1dB2IMpvYXhme5+myOgxqmlqFjFRft99xEgUhJF/t48C1mLV5I=',
      'MeEt0jpzXD+ePmlxddcXOl7rtVBCIdTZGOvgulRR2n8=' ],
    'peer' => [
      'BEnvNdE9uE4HZ0OKUcWXK7+ulkATaTJLJ9KV7ByynAFlakvWDMCuYIa/6GJ+Jw+2ylqjow7kAZphOlD2qvC93og=',
      'lSG07NeJ8UCALvdhuHKzbG5Ti53KzixPPVy/0XDobGQ=' ],
    'shared' => 'pzfk/X4ysfJAhhXF/q3RWFbtZ0I9B6F78Ej4mHlHjUE=' ],
  [ 'local' => [
      'BCYEMCXnNfAaOewRm4HhZf3MfV8h59kCXY5U1whacKfmpSmR6jgDN+LRduLmN2fOKNOAywQ2Wy1Y0IbyiJg6BEA=',
      'Ls0GdHyb9EHxWoQsnCFrV+g6xDupeERlpImn7xWEjXU=' ],
    'peer' => [
      'BMQhwmOGhXCLZ/F1JjnO9ZBcdqeEnX3i5QRwXtols40bnOYzFp1Nn+svR1FLtN+zCE1C2gBVYwGdISMZjqWX0jM=',
      '3ukitWFpudqzn42vPPr8nqU8lKzUlgitLgSkF/74uJo=' ],
    'shared' => 'wYZLKRaWPfYKGJULXuWpBrBag0uoN3Pm8NnG9NzETYc=' ],
  [ 'local' => [
      'BNEBf5EGSzSYs/27jKVON0VQwUWLzC0XWZUeAdrL8rN/QvNTO5sUI2SpZT8Wm2+hbPo/zu6Y5KsoFrwiDJgCTD0=',
      'FQ0jxY2LeVuD6TpHsZLIOV3D92DyrFP/iU0385Ku6Kc=' ],
    'peer' => [
      'BPkeiO5JzUT/HrqNpsq6sNzCU95b9YLAiQMOGzWwO4mxtGJlLh4WZfnCpaL5nqYBFAv3YkEEg92jmRuJvrz9mFE=',
      'CqcS9gB4EshvOu3VcwfoUnxI/Y1lOYS75tun+rSYsHE=' ],
    'shared' => 'h+sLnRN7y/D7zyRt5BiVRy1iMBXz83BO3DaJ9PJg57c=' ],
  [ 'local' => [
      'BDQiJs1zFz/MeVB7TRqFlR0XCiamhesrUOD1/ynNPXQnxES/bjivNetYaEDiQAi8kh0NTnbht99gVg0Z7C6NEEw=',
      'c4M8dGuoHHYmPnsi0W0w49JWv3BQncxGg18hOHzf2+Y=' ],
    'peer' => [
      'BHgSFrV+xXeFrOF0Iam++VCxNbzDqTINA5ac6bqBPsZSyZs1Oojt9m4fNLh/5yP9QwpD+jQNYA2RKGubRoR5lsU=',
      'iVkbirieXW/94gmjVavVNU/eBSO47d8cSHsj6teqi5A=' ],
    'shared' => 'KXuEaXE9NWslH7/CMjpSm7MzSmWswtDP2+RTPUiZaJM=' ],
];

foreach ($vectors as $i => $vector) {
  $local = ece_p256_import(base64_decode($vector['local'][0]),
                           base64_decode($vector['local'][1]));

  $peer = ece_p256_import(base64_decode($vector['peer'][0]),
                          base64_decode($vector['peer'][1]));

  if (!$local || !$peer) {
    echo 'Unable to import the keys for vector #' . $i;
    break;
  }

  $shared1 = ece_p256_compute_key($local, $peer);
  $shared2 = ece_p256_compute_key($peer, $local);

  if ($shared1 != $shared2) {
    echo 'Shared secrets are not symmetrical for vector #' . $i;
    break;
  }

  if (base64_decode($vector['shared']) != $shared1) {
    echo 'Computed shared secret does not match reference for vector #' . $i;
    break;
  }
  
  echo 'Success for vector #' . $i . PHP_EOL;

  ece_p256_free($local);
  ece_p256_free($peer);
}

?>
--EXPECTF--
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
