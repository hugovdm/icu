# Clean-Sheet API

This is one idea for what an ICU resource bundle API could look like if we
started from scratch.

## Goals

*   Don't wrap simple types into a resource bundle class: To keep the API simple
    and to avoid building and using the wrapper.
*   Make it clear when a resource table (a key-value map) performs fallbacks and
    when not.
*   When we inherit the JDK ResourceBundle we should implement its semantics.

## Containers

We could define interfaces for the array and table container resource types.
They would give direct access to their contained objects, and only via their
natural access methods (index vs. key). Returned objects are immutable, except
for the mark/position/limit in Buffer values.

interface ResourceArray {

int getSize();

ResourceType getType(int i);

String getString(int i);

int getInt(int i);
int getUInt(int i);

ByteBuffer getBinary(int i); // read-only

ByteArrayInputStream getBinaryAsInputStream(int i);

IntBuffer getIntVector(int i); // read-only

ResourceTable getTable(int i);

ResourceArray getArray(int i);

List<String> getStringArray(int i); // unmodifiable

}

interface ResourceTable {

**boolean doesFallback();**

Set<String> keySet();

boolean containsKey(String key);

ResourceType getType(String key);

String getString(String key);

int getInt(String key);
int getUInt(String key);

ByteBuffer getBinary(String key); // read-only

ByteArrayInputStream getBinaryAsInputStream(String key);

IntBuffer getIntVector(String key); // read-only

ResourceTable getTable(String key);

ResourceArray getArray(String key);

List<String> getStringArray(String key); // unmodifiable

}
Yoshito: Distinction between int and unit is useless. If ResourceTable does not
fallback, we could provide index access to table items.

## Bundle

There seem to be two options for a whole bundle.

### Either: A bundle is a table

Simple, but it still somewhat blurs the distinction between a bundle and a table
item. Internally, it would probably actually *have* a table and delegate to it.

UResourceBundle extends JDK ResourceBundle implements ResourceTable {

... implement the ResourceTable methods and the JDK ResourceBundle handleXYZ()
methods ...

}

### Or: A bundle has a table

Clear distinction between bundle and item, but need to call a trivial getter for
access to all top-level items that are not of type String.

UResourceBundle extends JDK ResourceBundle {

... implement the JDK ResourceBundle handleXYZ() methods ...

ResourceTable getTable() {

return topLevelTable;

}

}
Yoshito: What I thought was -

*   Table is a simple container of named items.
*   Table can be accessed as a bundle.
*   ICU RB may provide two methods in parallel - ie. providing two different
    "views" to a single table.
    *   ResourceTable getTable(String key) - no inheritance
    *   ICURB getSubBundle(String key) - access the table as "bundle", which
        does inheritance
