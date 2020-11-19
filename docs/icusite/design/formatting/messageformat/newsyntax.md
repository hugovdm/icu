# New Syntax

## Proposal

Introduce new syntax, as follows. By default, auto-detect from the message
pattern string whether it uses the old syntax or the new one.

*   Use digraphs to enclose both arguments and sub-messages: {|...|} instead of
    {...}.
    *   ASCII digraphs for easy input, and easy string literals in non-Unicode
        source code.
    *   Unusual combinations of characters that are rare in normal text.
    *   Use the same digraphs for both arguments and sub-messages. Otherwise we
        believe that users will be confused. (And we would have to come up with
        a second set of low-collision digraphs.)
*   Make the plural/selectordinal remainder placeholder look like an argument:
    {|#|}
    *   Allow white space just like with an argument, for consistency: {| # |}
*   In a choice argument style, enclose sub-messages also with {|...|} rather
    than the less-than operator and the single |
    *   Allow white space before the {| and after the |} as in
        plural/select/selectordinal.
    *   We retain the # or ≤ in the choice format so that the syntax and the
        MessagePattern.Part stream remain mostly the same.
*   Add two more MessagePattern.ApostropheMode values: (The enum name does not
    really fit the choices well.)
    *   Existing DOUBLE_OPTIONAL (ICU 4.8 default behavior)
    *   Existing DOUBLE_REQUIRED (JDK compatibility mode)
    *   New SINGLE_AND_DIGRAPH_BRACES (new syntax)
    *   New HEURISTIC (auto-detect DOUBLE_OPTIONAL vs. SINGLE_APOS_AND_DIGRAPHS)
        *   Proposed to be the default behavior in ICU 50.
        *   Detects simply via pattern.contains("{|") or equivalent.
        *   Note: The new syntax looks to an old parser like a syntax error,
            unless it is immediately preceded by a quote-starting ASCII
            apostrophe that does not solely enclose the curly brace: "... '{|
            ...' ..." -- this seems extremely unusual.
*   No mechanism for quoting/escaping in the new syntax.
    *   Digraphs can be disrupted with space or U+200B ZWSP or similar.
*   New method ApostropheMode getResolvedApostropheMode()
    *   If getApostropheMode()==HEURISTIC, then get==Resolved==ApostropheMode()
        will return
        *   HEURISTIC if no pattern has been parsed yet.
        *   DOUBLE_OPTIONAL or SINGLE_AND_DIGRAPH_BRACES after a pattern has
            been parsed.
        *   When parsing another pattern, the heuristic is applied again. (Hence
            the new method.)
    *   Otherwise getResolvedApostropheMode() will return the same value as
        getApostropheMode()
*   C++ API signatures:
    *   New UMessagePatternApostropheMode constants
        *   UMSGPAT_APOS_SINGLE_AND_DIGRAPH_BRACES
        *   UMSGPAT_APOS_HEURISTIC
    *   New MessagePattern method
        *   UMessagePatternApostropheMode getResolvedApostropheMode () const

ICU ticket: <http://bugs.icu-project.org/trac/ticket/9333> "more robust
MessageFormat syntax"

### Example

With ApostropheMode.DOUBLE_OPTIONAL (ICU 4.8 default behavior):

```none
In folder {folder_name} on {when, date, yyyy-MM-dd} there are
{num1, choice, 1<very few|4#at least four} files including
{num2, plural, offset:1 =0 {no} =1 {one} =2 {two}
               other {one and # from group '#'3}} text files from
{gender, select, female {her} male{his}other{their}} collection.
```

With ApostropheMode.SINGLE_AND_DIGRAPH_BRACES (proposed ICU 50 behavior):

```none
In folder {|folder_name|} on {|when, date, yyyy-MM-dd|} there are
{|num1, choice, 1< {|very few|} 4# {|at least four|}|} files including
{|num2, plural, offset:1 =0 {|no|} =1 {|one|} =2 {|two|}
                other {|one and {|#|} from group #3|}|} text files from
{|gender, select, female {|her|} male{|his|}other{|their|}|} collection.
```

## Problems with old syntax

*   Uses single-ASCII-character syntax which is prone to collide with normal
    text and requires quoting/escaping.
*   Unfortunately, we made this worse in plural arguments by using a single #
    sign for the number.
*   Quoting is done with ASCII apostrophes which definitely collides with normal
    text, although this got improved significantly in ICU 4.8.

### Examples

*   ... {num, plural, ... other{# fans call them their #1 band}} ... (with num=5
    this becomes "5 fans call them their 51 band")
*   ... {gender, select, male{He said 'No way!'} ...} ... (fails with syntax
    error: ASCII apostrophe before variant-ending } starts quoted literal text)
*   ... {num, plural, =1{Seleccioneu un màxim d'# widget més} ...} ... (fails
    with syntax error: ASCII apostrophe before plural number replacement #
    starts quoted literal text)

### Minor Issue

In some translation tools, it has been awkward to have sub-messages enclosed in
{pairs of curly braces}. It would be slightly easier for those tools to instead
terminate sub-messages at the next syntax token. On the other hand, the {pairs}
are useful for avoiding white space characters when a message is broken into
lines without enclosing each line in "quotes" or similar. (White space (line
feeds, indentation) are not an issue in C++ or Java when messages are written in
"string literals", but can be awkward if messages are in XML contents or
similar.)

## Ideas

*   Use *digraphs* (*pairs* of characters) for syntax, to make syntax tokens
    very unlikely to collide with normal text.
    *   Each digraph should be a very unusual (in human-readable text)
        combination of unusual characters.
    *   For parsing, each of these characters should be
        [Pattern_Syntax](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%3APattern_Syntax%3A%5D&g=).
    *   For typing, each of these characters should be an [*ASCII*
        Pattern_Syntax](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%5B%3APattern_Syntax%3A%5D%26%5B%5Cu0000-%5Cu007f%5D%5D&g=)
        character \[\\- , ; \\: ! ? . ' " ( ) \\\[ \\\] \\{ \\} @ \* / \\\\ \\&
        # % \` \\^ + < = > | ~ \\$\], although this conflicts with the goal of
        choosing unusual characters.
        *   Maybe we could use [Latin-1 Pattern_Syntax
            characters](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%5B%3APattern_Syntax%3A%5D%26%5B%5Cu0080-%5Cu00ff%5D%5D&g=)
            \[¡ ¿ « » § ¶ © ® ° ± ÷ × ¬ ¦ ¤ ¢ £ ¥\] as a compromise between
            developer input and avoiding "normal text characters".
        *   See [German Wikipedia about keyboard input of quotation
            marks](http://de.wikipedia.org/wiki/Anf%C3%BChrungszeichen#Direkte_Eingabe_per_Tastatur).
*   Most syntax has begin-end tokens with further specs in between. At least in
    some of the simpler cases, we could make it so that only a completely
    syntactically correct begin-spec-end sequence is recognized as syntax, while
    any other sequence is just text (and does not cause a syntax error).
    *   This would strongly reduce possible collisions with normal text.
    *   However, it might complicate the parser, and it would turn actual syntax
        errors into visible text rather than reporting an error.
*   Arguments should be terminated with a digraph as well, so that the last
    sub-message in a complex argument is terminated with that rather than with a
    single character.
*   Maybe add an optional token like {.} for terminating a sub-message, or maybe
    {+} for generally ignoring following Pattern_White_Space? ({+} reminiscent
    of Java String concatenation.)
    *   These would be useful if we chose to bracket selectors rather than
        sub-messages.
    *   This seems to indicate that bracketing selectors is as problematic as
        bracketing sub-messages.
*   Quoting/escaping:
    *   Maybe no quoting/escaping at all.
    *   Or something sufficiently obscure like {'x'} where x is a single escaped
        Pattern_Syntax character. If someone really needs to write "$}" they
        could escape the $ and write "{'$'}}".
    *   Or something that harmlessly breaks the digraph rather than quoting a
        character. For example, reserve some character that results in no output
        at all, and insert it between the two characters that we want to prevent
        from forming a digraph. Or insert a special token that has no output,
        such as {°} which could also serve as a "this is new syntax" indicator.
*   ICU transition:
    *   We probably want an attribute on MessageFormat & MessagePattern.
    *   It would be good if we could auto-detect the new syntax, so that users
        need not switch APIs.
        *   For auto-detection, we might need a special token that just
            indicates the new syntax if we do not otherwise need
            arguments/placeholders, to suppress old-syntax quoting/escaping. Or
            else we ignore argument-less message patterns?

## Sample: All-ASCII syntax, bracketing selectors

When selectors are bracketed, then a sub-message is terminated either by another
selector or by the end-of-argument token.

Choice format selectors: {:1<:} for an exclusive limit (sub-message for values
greater than 1) vs. {:4:} for an inclusive limit (sub-message for values greater
than or equal to 4).

Plural format selectors: We could distinguish explicit values vs. keywords by
looking for a leading digit (although ICU 4.8 allows any non-syntax,
non-whitespace character in keywords), adding syntax inside the selector, or
using different selector begin-end tokens. This sample adds an equals sign
inside the explicit-value selector, similar to the ICU 4.8 syntax.

```none
In folder {@folder_name@} on {@when, date, yyyy-MM-dd@} there are
{@num1, choice, {:1<:}very few{:4:}at least four@} files including
{@num2, plural, offset:1 {:=0:}no{:=1:}one{:=2:}two{+}
                {:other:}one and {#} from group {{'#'}3}@} text files from
{@gender, select, {:female:}her{:male:}his{:other:}their {'@'}{'}'}@} collection.
```

## Sample: Syntax with Latin-1, bracketing sub-messages

Sub-messages are {§bracketed like this§}. No syntax for "end sub-message" or
"ignore following white space".

Choice format selectors: 1< for an exclusive limit (sub-message for values
greater than 1) vs. 4 for an inclusive limit (sub-message for values greater
than or equal to 4).

```none
In folder {°folder_name°} on {°when, date, yyyy-MM-dd°} there are
{°num1, choice, 1< {«very few»} 4 {«at least four»}°} files including
{°num2, plural, offset:1 =0 {«no»} =1 {«one»} =2 {«two»}
                other {«one and {¤} from group {{¦¤¦}3}»}°} text files from
{°gender, select, female {«her»} male{«his»}other{«their {¦§¦}{¦}¦}»}°} collection.
```

Same idea, different characters, no quoting/escaping syntax:

```none
In folder «%folder_name%» on «%when, date, yyyy-MM-dd%» there are
«%num1, choice, 1< «{very few}» 4 «{at least four}»%» files including
«%num2, plural, offset:1 =0 «{no}» =1 «{one}» =2 «{two}»
                other «{one and «#» from group #3%»%» text files from
«%gender, select, female «{her}» male«{his}»other«{their %#}»%» collection.
```

## Sample: Syntax with trigraphs, bracketing sub-messages

Sub-messages are ~+{bracketed like this}+~. No syntax for "ignore following
white space".

No syntax for quoting/escaping in this sample. Needed? Maybe ~%{}%~ as a
new-syntax marker and trigraph breaker with no output by itself?

Choice format selectors: 1< for an exclusive limit (sub-message for values
greater than 1) vs. 4 for an inclusive limit (sub-message for values greater
than or equal to 4).

```none
In folder ~%{folder_name}%~ on ~%{when, date, yyyy-MM-dd}%~ there are
~%{num1, choice, 1< ~+{very few}+~ 4 ~+{at least four}+~}%~ files including
~%{num2, plural, offset:1 =0 ~+{no}+~ =1 ~+{one}+~ =2 ~+{two}+~
                other ~+{one and ~%{#}%~ from group #3}+~}%~ text files from
~%{gender, select, female ~+{her}+~ male~+{his}+~other~+{their xyz}+~}%~ collection.
```

## Sample: Somewhat XML-ish

Argument is one of

*   `"{arg" PWS+ arg-name PWS+ "/}"`
*   `"{arg" PWS+ arg-name PWS+ ":" PWS+ arg-type PWS+ "/}"`
*   `"{arg" PWS+ arg-name PWS+ ":" PWS+ arg-type PWS+ "}" arg-style "{/arg}"`.

where PWS=Pattern_White_Space.

Sub-message begins with `"{msg" PWS+ selector "}"` and ends with `"{/msg}"`.

No syntax for quoting/escaping; needed?

Maybe optional `{msg}...{/msg}` around the whole message for syntax version
disambiguation? Or just scan for presence of "{/arg}" and similar? Or use square
brackets in new syntax? Or use "{$arg "..."{/arg$}"?

```none
In folder {arg folder_name/} on {arg when:date}yyyy-MM-dd{/arg} there are
{arg num1:choice}{msg 1<}very few{/msg}{msg 4<=}at least four{/msg}{/arg} files including
{arg num2:plural}{offset:1}{msg =0}no{/msg}{msg =1}one{/msg}{msg =2}two{/msg}
              {msg other}one and {plural_number/} from group #3{/msg}{/arg} text files from
{arg gender:select}{msg female}her{/msg}{msg male}his{/msg}{msg other}their xyz{/msg}{/arg} collection.
```

## Original Sample

```none
In folder {$folder_name$} on {$when, date, yyyy-MM-dd$} there are
{$num1, choice, {<1}very few{<=4}at least four$} files including
{$num2, plural, offset:1 {=0}no{=1}one{=2}two{+}
                {:other}one and {#} from group {{'#'}3}$} text files from
{$gender, select, {:female}her{:male}his{:other}their {'$'}{'$'}$} collection.
```

### Mark's comments on the original version & sample

Some thoughts on alternatives.

{%folder_name%} or {!folder_name!} or {$folder_name$}

{:0:}
{:other:}

{<1>}
{<=4>}

{#}

{'<any char>'}

Also, we could use {% or to trigger new vs old syntax, and then have the same
method....
