This is the DEPU Application. It allows a patch to be loaded at start-up, and performs
the necessary operations to do CMPU and DMPU in one-go for AR0825 for customer evaluation
purpose only, with fixed keys.

To build the patch set:
make -C src\DepuApp BUILD_CONFIG=barebones_armcc clean all

To build the unit tests for MinGW
make -C src\DepuApp\test BUILD_CONFIG=cpputest_mingw32 clean all

To build a new set of evaluation keys:
make -C src\DepuApp BUILD_CONFIG=barebones_armcc generate_keys

To build the a set of Certificates:
make -C src\DepuApp BUILD_CONFIG=barebones_armcc generate_certificates
  Notes: The contents are generated using some random nonce so each execution generates a
  different file even if the keys haven't change.