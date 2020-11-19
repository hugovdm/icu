# Java Logger

As of Java 1.4.\*, a resource cheap API for logging messages have been included
in the JDK. ICU4J can use this to log customized detailed messages in a
particular format to a desired destination by extending the Logger class. By
default, logging will be turned off and is only turned on when desired. Usually
logging is turned on for specific debugging purposes without the need to use
global flags and large amounts of System.out.println() statements.
**Design Description for ICU Logger:**
1) Add a ICULogger class in com.ibm.icu.impl which extends the JDK Logger class
2) Logging is off by default and is turned on globally by setting the system
property "debug.logging" to "on".
3) Add methods to turn on/off logging when the system property "debug.logging"
is set to "on".
**Implementation Description:**
1) Add a public static logger per class (if the system property "debug.logging"
is not set to "on", then the static logger is null)
2) At the desired location/condition in the code, use the static logger object
to call the logging method (e.g. logger.info("Message"))
Note: You should check that the logger object is not null
Note: You can call a more generic method log(Level, Message) but you will need
to import the Level class in order to specify the level
**Things to Consider:**
1) When it comes to the Java Logger, there are numerous levels: (SEVERE,
WARNING, INFO, CONFIG, FINE, FINER, FINEST, ALL, OFF)
Because of this, ICU needs to determine what condition warrants a particular
level. (e.g. If a condition should always log a mesage regardless of the level
set, then ALL should be used)
(Note: If the level is set to FINE, everything above it, (SEVERE, WARNING, INFO,
CONFIG), will also be logged but not FINER or FINEST)
**Sample Usage:**
\*Let's assume that TimeZone has a static logger object called TimeZoneLogger.
**public static ICULogger TimeZoneLogger =
ICULogger.getICULogger(TimeZone.class.getName());**
\*Also, we want to log a message with a level of INFO when we call getTimeZone()
and give a bogus ID. So the method may look like this:
**public static synchronized TimeZone getTimeZone(String ID, int type) {**
**...**
**if (result == null) {**
**if (TimeZoneLogger != null && TimeZoneLogger.isLoggingOn()) {**
**TimeZoneLogger.info("\\"" +ID + "\\" is a bogus id so timezone is falling back to GMT.");**
**}**
**result = ZoneMeta.getGMT();**
**}**
**...**
**}**
-Basically, this will log a level INFO message stating that a bogus ID was given
and ICU is using GMT.
\*In our test code we can do something like this:
**public static void main(String\[\] args) {**
*/\* Create a handler to determine where to send the logged message. In this case we are sending the message to the console. \*/*
**ConsoleHandler handler = new ConsoleHandler();**
*/\* Turn on logging. \*/*
**TimeZone.TimeZoneLogger.TurnOnLogging();**
*/\* Add the handler to the logger. Note: You can add multiple handlers \*/*
**TimeZone.TimeZoneLogger.addHandler(handler);**
*/\* Set the level of the logger \*/*
**TimeZone.TimeZoneLogger.setLevel(Level.INFO);**
*/\* Call the method with a bogus ID. The logged message should be displayed \*/*
**TimeZone tz1 = TimeZone.getTimeZone("foo");**
**}**
The user of ICU who wants to utilize logging needs to give their own handler.
The handler can be customize to log messages to HTML format or whatever the user
wants.
Note: If logging is not desired, there is little performance hit from calling
logging methods since the logger object is null.
