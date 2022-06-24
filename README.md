# Authors
- [Francisco Bernad](https://github.com/FrBernad)
- [Agustín Manfredi](https://github.com/imanfredi)
- [Joaquín Legammare](https://github.com/JoacoLega)

# STEGANOGRAPHY BPM
Embed and extract files inside .bmp using the following steganography strategies:
- LSB1
- LSB4
- LSBI

## Requirements
- [C Compiler](https://gcc.gnu.org/)
- [OpenSSL](https://www.openssl.org/)

## Setup

On the src project folder run the command `make`. This will compile and generate the executable stegobmp.

## Usage
Theres two ways of using the program:
* ```stegobmp -embed -in file -p in_file -out out_file -steg steg_algorithm [-a enc_algorithm] [-m mode] [-pass password]```
* ```stegobmp -extract -p in_file -out out_file -steg steg_algorithm [-a enc_algorithm] [-m mode] [-pass password]```

Where each agument specifies:
* -h:                      display the help menu and exit.
* -embed:                  indicates the embedding of information.
* -extract:                indicates the extraction of information.
* -in file:                file to hide.
* -p bitmap_file:          carrier file.
* -out bitmap_file:        BMP file with embeded information.
* -steg steg_algorithm:    steganography algorithm:
  * LSB1: LSB de 1bit
  * LSB4: LSB de 4 bits
  * LSBI: LSB Enhanced
* -a enc_algorithm:        encription algorithm:
  * aes128
  * aes192
  * aes256
  * des
* -m mode:                 encription algorithm chain mode:
  * ecb
  * cfb
  * ofb
  * cbc
* -pass password:          encription password


