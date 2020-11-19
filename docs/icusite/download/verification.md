# Verifying Downloads

Many ICU [downloads](index.md) may be verified as to their authenticity.

How to verify the downloads?

### [MD5](https://en.wikipedia.org/wiki/MD5)

*   Download the MD5 hash file by clicking the **\[MD5\]** link on the download
    page, as well as another ICU file.
*   Run a command line program such as `md5`, `md5sum`, `cfv`, or [fciv on
    Windows](http://support.microsoft.com/kb/841290) over your downloaded ICU
    file
*   Verify that the hashed result from the command line program matches the hash
    in the .md5 file.

### SHASUM512

*   SHA512 hashes may be created with `gpg --print-md SHA512 somefile`
*   These hashes may be verified with `shasum -c SHASUM512.txt`
*   (Note that GPG signed files may begin with "Hash: SHA256", but this is GPG's
    hash, not the SHASUM hashes)

### GPG / PGP

*   Download the file <http://ssl.icu-project.org/KEYS> and import it with:
    gpg --import KEYS`
    (This is safe to run multiple times, it will update any new keys)
*   Download the original ICU file, as well as the .asc file by clicking the
    **\[PGP\]** link in the left column of the download page.
*   To verify the file, run this command line against the .asc file
    ` gpg --verify `*`somefile`*`.tgz.asc`
*   If the signature is good, you should see "gpg Good signature from..."

For further reading, see the [Apache Project release signing
page](https://www.apache.org/dev/release-signing.html)
