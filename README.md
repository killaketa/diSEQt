# diSEQt

Decode & encode BRSEQ (Binary Revolution Sequence) files.

Please view the <a href="https://github.com/killaketa/diSEQt/wiki">wiki</a> for more information.

*currently only supports Big Endian (0xFEFF identified) BRSEQ files. If a Little Endian (0xFFFE indentified) file is decoded every value will be byteswapped and parsed incorrectly.

### Usage:
```
diSEQt decode filename.brseq destfilename.xmlseq
diSEQt encode filename.xmlseq destfilename.brseq
diSEQt dc filename.brseq destfilename.xmlseq
diSEQt ec filename.xmlseq destfilename.brseq
```
