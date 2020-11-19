# RB design discussion - 2009-08-21

## Issues

*   Runtime performance
    *   (ICU4J) Loading entire data into heap as raw bytes, creating a Java
        object when accessed. Without Java object caching, it creates multiple
        identical objects (like former implementation). Even with caching, raw
        byte data is not released.
    *   (ICU4J) Bundle size is too big. For example, time zone data
        (zoneinfo.res) is currently 155kb. ICU4J load entire res contents to
        heap. In many cases, client only need a single time zone, which require
        only several hundreds byte maximum.
    *   (ICU4J) Getting a resource item from our own proprietary format always
        require a new Java object instantiation and probably always slower than
        getting a Java object from .class file.
*   Usability / Design
    *   No public API for supporting inheritance (fallback to parents) for
        nested items.
    *   (ICU4J) UResourceBundle does not support JDK 6 ResourceBundle.Control
        features, which allow user to customize look up path, controlling cache,
        etc.
    *   (ICU4J) UResourceBundle extends JDK ResourceBundle, but its design is
        largely different from JDK.
*   [Yoshito's old note](issues.md)

## Design Options

*   Break ICU locale resources into smaller pieces - package items optimized for
    typical usage models.
    *   Doug to come up with resource type categorization proposal.
*   ICU4J to use .class file to store locale resources (such as
    ListResourceBundle)
    *   This could introduce ICU4C / J incompatibility
    *   We could add "locale data API" compatible between C and J.
    *   Duplicated (or substring) key optimization can be done by tooling.
    *   With smaller bundles, we should be able to avoid the Java class file
        size limitation.
    *   Large binary items might need to be externalized to avoid the Java class
        file limitation (e.g. collation rules)
    *   We need a way to verify the contents is equivalent to .res. We should
        probably start with .txt <-> .class. In longer term, we want to generate
        locale resources directly from LDML.
    *   We agreed that Yoshito to develop .class resource type based on the
        existing code for evaluation.
        [#7116](http://bugs.icu-project.org/trac/ticket/7116)
*   Deprecated ICU4J UResourceBundle and start new one from scratch. Leave
    UResourceBundle class as is.
    *   [Clean-Sheat API](clean.md)
    *   Low impact to ICU4J users. We've never heard of clients using ICU4J
        UResourceBundle API. For ICU4C, we know there are many clients using
        ures APIs, so we probably need incremental improvement.
    *   getInt / getUInt
        *   Yoshito/Doug think signed or unsigned should be clearly tagged at
            the resource level - which is not done in the current architecture.
        *   We still need both unless we change add resource types to identify
            signed or unsigned.
    *   We still need to support "table" without inheritance.
    *   Resource array won't support array item inheritance.
    *   Yoshito to start prototype based on the [Clean-Sheat API](clean.md) for
        ICU4J. [#7115](http://bugs.icu-project.org/trac/ticket/7115)
