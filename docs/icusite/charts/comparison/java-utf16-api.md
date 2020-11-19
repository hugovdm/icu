# UTF16 API Comparison

TODO: flesh this out.
Java incorporated support for UTF16, roughly along the lines of what ICU had
done although with a number of omissions requiring work-arounds. The following
provides a mapping between the corresponding APIs.
**ICU** **JDK** UTF16.charAt(myString, offset)
myString.codePointAt(offset)
UTF16.valueOf(codePoint)
new StringBuilder().appendCodePoint(codePoint).toString()
UTF16.getCharCount(codePoint)
Character.charCount(codePoint)
