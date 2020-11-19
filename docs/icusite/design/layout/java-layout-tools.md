# Java Layout Tools

• **NOTE**: at present, the following was found by experimentation, in the
absence of other docs.

• tools are in tools/misc under ICU4J

• running as (from ICU4J): "`ant all ; cd tools/misc ; java -classpath
../../icu4j.jar:out/bin com.ibm.icu.dev.tool.layout.OpenTypeTagBuilder`"

• likes to write into the source directory, such as
(tools/misc)/src/com/ibm/icu/dev/tool/layout

Usage:

*   com.ibm.icu.dev.tool.layout.**BuildMirroringTables**
    *   Writes MirroredCharData.cpp
*   com.ibm.icu.dev.tool.layout.**CanonGSUBBuilder** *filename*
    *   writes to *filename* ( in the java layout dir), contents are actually
        CanonData.cpp
*   com.ibm.icu.dev.tool.layout.**OpenTypeTagBuilder**
    *   writes contents of *two enums* in LETypes.h to stdout
    *   Differences from what's checked in: (Manual updates!)
        *   `/* 'calt' */` instead of `/**< 'calt' */`
        *   Tool is Missing these two:
            *   LE_CJCT_FEATURE_TAG = 0x636A6374UL, /\*\*< 'cjct' \*/
            *   LE_RKRF_FEATURE_TAG = 0x726B7266UL, /\*\*< 'rkrf' \*/
*   com.ibm.icu.dev.tool.layout.**ScriptNameBuilder**
    *   writes LEScripts.h,
        *   misses "qaaiScriptCode = zinhScriptCode /\* manually added alias,
            for API stability \*/"
    *   LELanguages.h,
    *   ScriptAndLanguageTags.h, ScriptAndLanguageTags.cpp,
        *   Misses the following:
            *   -const LETag bng2ScriptTag = 0x626E6732; /\* 'bng2' (BENGALI
                v.2) (manually added) \*/
            *   -const LETag dev2ScriptTag = 0x64657632; /\* 'dev2' (DEVANAGARI
                v.2) (manually added) \*/
            *   -const LETag gjr2ScriptTag = 0x676A7232; /\* 'gjr2' (GUJARATI
                v.2) (manually added) \*/
            *   -const LETag gur2ScriptTag = 0x67757232; /\* 'gur2' (GURMUKHI
                v.2) (manually added) \*/
            *   -const LETag knd2ScriptTag = 0x6B6E6432; /\* 'knd2' (KANNADA
                v.2) (manually added) \*/
            *   -const LETag mlm2ScriptTag = 0x6D6C6D32; /\* 'mlm2' (MALAYALAM
                v.2) (manually added) \*/
            *   -const LETag ory2ScriptTag = 0x6F727932; /\* 'ory2' (ORIYA v.2)
                (manually added) \*/
            *   -const LETag tml2ScriptTag = 0x746D6C32; /\* 'tml2' (TAMIL v.2)
                (manually added) \*/
            *   -const LETag tel2ScriptTag = 0x74656C32; /\* 'tel2' (TELUGU v.2)
                (manually added) \*/
    *   ScriptRunData.cpp
    *   updates file "ScriptAndLanguages"
*   com.ibm.icu.dev.tool.layout.**ShapingTypeBuilder** *filename*
    *   writes to *filename* which is really ShapingTypeData.cpp
*   com.ibm.icu.dev.tool.layout.**ThaiStateTableBuilder** *filename*
    *   writes to *filename* which is really ThaiStateTables.cpp
