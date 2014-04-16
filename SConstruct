# -*- mode: python; -*-

import buildscripts
import copy
import datetime
import imp
import os
import re
import shutil
import stat
import sys
import textwrap
import types
import urllib
import urllib2
from buildscripts import utils

import libdeps

EnsureSConsVersion( 1, 1, 0 )
if "uname" in dir(os):
    scons_data_dir = ".scons/%s/%s" % ( os.uname()[0] , os.getenv( "HOST" , "nohost" ) )
else:
    scons_data_dir = ".scons/%s/" % os.getenv( "HOST" , "nohost" )
SConsignFile( scons_data_dir + "/sconsign" )

DEFAULT_INSTALL_DIR = "/usr/local"

def versiontuple(v):
    return tuple(map(int, (v.split("."))))

# --- platform identification ---
#
# This needs to precede the options section so that we can only offer some options on certain
# platforms.

platform = os.sys.platform
nix = False
linux = False
darwin = False
windows = False
freebsd = False
openbsd = False
solaris = False

if "darwin" == platform:
    darwin = True
    platform = "osx" # prettier than darwin
elif platform.startswith("linux"):
    linux = True
    platform = "linux"
elif "sunos5" == platform:
    solaris = True
elif platform.startswith( "freebsd" ):
    freebsd = True
elif platform.startswith( "openbsd" ):
    openbsd = True
elif "win32" == platform:
    windows = True
else:
    print( "No special config for [" + platform + "] which probably means it won't work" )

nix = not windows

# --- options ----
use_clang = False

options = {}

def add_option( name, help, nargs, contributesToVariantDir,
                dest=None, default = None, type="string", choices=None, metavar=None ):

    if dest is None:
        dest = name

    if type == 'choice' and not metavar:
        metavar = '[' + '|'.join(choices) + ']'

    AddOption( "--" + name , 
               dest=dest,
               type=type,
               nargs=nargs,
               action="store",
               choices=choices,
               default=default,
               metavar=metavar,
               help=help )

    options[name] = { "help" : help ,
                      "nargs" : nargs ,
                      "contributesToVariantDir" : contributesToVariantDir ,
                      "dest" : dest,
                      "default": default }

def get_option( name ):
    return GetOption( name )

def has_option( name ):
    x = get_option( name )
    if x is None:
        return False

    if x == False:
        return False

    if x == "":
        return False

    return True

def get_variant_dir():
    if has_option('variant-dir'):
        return "#build/" + get_option('variant-dir') 

    substitute = lambda x: re.sub( "[:,\\\\/]" , "_" , x )

    a = []

    for name in options:
        o = options[name]
        if not has_option( o["dest"] ):
            continue
        if not o["contributesToVariantDir"]:
            continue
        if get_option(o["dest"]) == o["default"]:
            continue

        if o["nargs"] == 0:
            a.append( name )
        else:
            x = substitute( get_option( name ) )
            a.append( name + "_" + x )

    extras = []
    if has_option("extra-variant-dirs"):
        extras = [substitute(x) for x in get_option( 'extra-variant-dirs' ).split( ',' )]

    if has_option("add-branch-to-variant-dir"):
        extras += ["branch_" + substitute( utils.getGitBranch() )]

    if has_option('cache'):
        s = "#build/cached/"
        s += "/".join(extras) + "/"
    else:
        s = "#build/${PYSYSPLATFORM}/"
        a += extras

        if len(a) > 0:
            a.sort()
            s += "/".join( a ) + "/"
        else:
            s += "normal/"

    return s

# build output
add_option( "mute" , "do not display commandlines for compiling and linking, to reduce screen noise", 0, False )

# installation/packaging
add_option( "prefix" , "installation prefix" , 1 , False, default=DEFAULT_INSTALL_DIR )
add_option( "extra-variant-dirs", "extra variant dir components, separated by commas", 1, False)
add_option( "add-branch-to-variant-dir", "add current git branch to the variant dir", 0, False )
add_option( "variant-dir", "override variant subdirectory", 1, False )

add_option( "sharedclient", "build a libmongoclient.so/.dll" , 0 , False )

# linking options
add_option( "release" , "release build" , 0 , True )
add_option( "lto", "enable link time optimizations (experimental, except with MSVC)" , 0 , True )
add_option( "dynamic-windows", "dynamically link on Windows", 0, True)
add_option( "disable-declspec-thread", "don't use __declspec(thread) on Windows", 0, True)

# base compile flags
add_option( "64" , "whether to force 64 bit" , 0 , True , "force64" )
add_option( "32" , "whether to force 32 bit" , 0 , True , "force32" )

add_option( "cxx", "compiler to use" , 1 , True )
add_option( "cc", "compiler to use for c" , 1 , True )
add_option( "cc-use-shell-environment", "use $CC from shell for C compiler" , 0 , False )
add_option( "cxx-use-shell-environment", "use $CXX from shell for C++ compiler" , 0 , False )
add_option( "c++11", "enable c++11 support (experimental)", 0, True )

add_option( "cpppath", "Include path if you have headers in a nonstandard directory" , 1 , False )
add_option( "libpath", "Library path if you have libraries in a nonstandard directory" , 1 , False )

add_option( "extrapath", "comma separated list of add'l paths  (--extrapath /opt/foo/,/foo) static linking" , 1 , False )
add_option( "extralib", "comma separated list of libraries  (--extralib js_static,readline" , 1 , False )

add_option( "ssl" , "Enable SSL" , 0 , True )

# library choices
add_option( "libc++", "use libc++ (experimental, requires clang)", 0, True )

# new style debug and optimize flags
add_option( "dbg", "Enable runtime debugging checks", 1, True, "dbg",
            type="choice", choices=["on", "off"] )

add_option( "opt", "Enable compile-time optimization", 1, True, "opt",
            type="choice", choices=["on", "off"] )

sanitizer_choices = ["address", "memory", "thread", "undefined"]
add_option( "sanitize", "enable selected sanitizer", 1, True,
            type="choice", choices=sanitizer_choices, default=None )

add_option( "gcov" , "compile with flags for gcov" , 0 , True )

add_option("use-sasl-client", "Support SASL authentication in the client library", 0, False)

add_option('build-fast-and-loose', "NEVER for production builds", 0, False)

add_option('propagate-shell-environment',
           "Pass shell environment to sub-processes (NEVER for production builds)",
           0, False)

if darwin:
    osx_version_choices = ['10.6', '10.7', '10.8', '10.9']
    add_option("osx-version-min", "minimum OS X version to support", 1, True,
               type = 'choice', default = osx_version_choices[0], choices = osx_version_choices)

elif windows:
    win_version_min_choices = {
        'xpsp3'   : ('0501', '0300'),
        'ws03sp2' : ('0502', '0200'),
        'vista'   : ('0600', '0000'),
        'ws08r2'  : ('0601', '0000'),
        'win7'    : ('0601', '0000'),
        'win8'    : ('0602', '0000'),
    }

    add_option("win-version-min", "minimum Windows version to support", 1, True,
               type = 'choice', default = None,
               choices = win_version_min_choices.keys())

add_option('cache',
           "Use an object cache rather than a per-build variant directory (experimental)",
           0, False)

add_option('cache-dir',
           "Specify the directory to use for caching objects if --cache is in use",
           1, False, default="#build/cached/.cache")

# don't run configure if user calls --help
if GetOption('help'):
    Return()

# --- environment setup ---

variantDir = get_variant_dir()

def printLocalInfo():
    import sys, SCons
    print( "scons version: " + SCons.__version__ )
    print( "python version: " + " ".join( [ `i` for i in sys.version_info ] ) )

printLocalInfo()

boostLibs = [ "thread" , "system" ]

linux64  = False
force32 = has_option( "force32" ) 
force64 = has_option( "force64" )
if not force64 and not force32 and os.getcwd().endswith( "mongo-64" ):
    force64 = True
    print( "*** assuming you want a 64-bit build b/c of directory *** " )
msarch = None
if force32:
    msarch = "x86"
elif force64:
    msarch = "amd64"

releaseBuild = has_option("release")

# validate debug and optimization options
dbg_opt_mapping = {
    # --dbg, --opt   :   dbg    opt
    ( None,  None  ) : ( False, True ),
    ( None,  "on"  ) : ( False, True ),
    ( None,  "off" ) : ( False, False ),
    ( "on",  None  ) : ( True,  False ),  # special case interaction
    ( "on",  "on"  ) : ( True,  True ),
    ( "on",  "off" ) : ( True,  False ),
    ( "off", None  ) : ( False, True ),
    ( "off", "on"  ) : ( False, True ),
    ( "off", "off" ) : ( False, False ),
}
debugBuild, optBuild = dbg_opt_mapping[(get_option('dbg'), get_option('opt'))]

if releaseBuild and (debugBuild or not optBuild):
    print("Error: A --release build may not have debugging, and must have optimization")
    Exit(1)

mongoclientVersion = "0.8.0-pre"
# We don't keep the -pre in the user testable version identifiers, because
# nobody should be conditioning on the pre-release status.
mongoclientVersionComponents = mongoclientVersion.split('-')
if len(mongoclientVersionComponents) not in (1,2):
    print("Error: client version most be of the form w.x.y or w.x.y-string")
    Exit(1)
mongoclientVersionComponents = mongoclientVersionComponents[0].split('.')
if len(mongoclientVersionComponents) != 3:
    print("Error: client version most be of the form w.x.y or w.x.y-string")
    Exit(1)
print(mongoclientVersionComponents)

env = Environment( BUILD_DIR=variantDir,
                   EXTRAPATH=get_option("extrapath"),
                   MSVS_ARCH=msarch ,
                   PYTHON=utils.find_python(),
                   TARGET_ARCH=msarch ,
                   tools=["default", "unittest", "integration_test", "textfile"],
                   PYSYSPLATFORM=os.sys.platform,
                   CONFIGUREDIR = '#' + scons_data_dir + '/sconf_temp',
                   CONFIGURELOG = '#' + scons_data_dir + '/config.log',
                   MONGOCLIENT_VERSION=mongoclientVersion,
                   MONGOCLIENT_VERSION_MAJOR=mongoclientVersionComponents[0],
                   MONGOCLIENT_VERSION_MINOR=mongoclientVersionComponents[1],
                   MONGOCLIENT_VERSION_PATCH=mongoclientVersionComponents[2],
                   )

if has_option("cache"):
    EnsureSConsVersion( 2, 3, 0 )
    if has_option("release"):
        print("Using the experimental --cache option is not permitted for --release builds")
        Exit(1)
    if has_option("gcov"):
        print("Mixing --cache and --gcov doesn't work correctly yet. See SERVER-11084")
        Exit(1)
    env.CacheDir(str(env.Dir(get_option('cache-dir'))))

# This could be 'if solaris', but unfortuantely that variable hasn't been set yet.
if "sunos5" == os.sys.platform:
    # SERVER-9890: On Solaris, SCons preferentially loads the sun linker tool 'sunlink' when
    # using the 'default' tools as we do above. The sunlink tool sets -G as the flag for
    # creating a shared library. But we don't want that, since we always drive our link step
    # through CC or CXX. Instead, we want to let the compiler map GCC's '-shared' flag to the
    # appropriate linker specs that it has compiled in. We could (and should in the future)
    # select an empty set of tools above and then enable them as appropriate on a per platform
    # basis. Until then the simplest solution, as discussed on the scons-users mailing list,
    # appears to be to simply explicitly run the 'gnulink' tool to overwrite the Environment
    # changes made by 'sunlink'. See the following thread for more detail:
    #  http://four.pairlist.net/pipermail/scons-users/2013-June/001486.html
    env.Tool('gnulink')

if has_option("propagate-shell-environment"):
    env['ENV'] = dict(os.environ);

env['_LIBDEPS'] = '$_LIBDEPS_OBJS'

if has_option('build-fast-and-loose'):
    # See http://www.scons.org/wiki/GoFastButton for details
    env.Decider('MD5-timestamp')
    env.SetOption('max_drift', 1)
    env.SourceCode('.', None)

if has_option('mute'):
    env.Append( CCCOMSTR = "Compiling $TARGET" )
    env.Append( CXXCOMSTR = env["CCCOMSTR"] )
    env.Append( SHCCCOMSTR = "Compiling $TARGET" )
    env.Append( SHCXXCOMSTR = env["SHCCCOMSTR"] )
    env.Append( LINKCOMSTR = "Linking $TARGET" )
    env.Append( SHLINKCOMSTR = env["LINKCOMSTR"] )
    env.Append( ARCOMSTR = "Generating library $TARGET" )

libdeps.setup_environment( env )

if env['PYSYSPLATFORM'] == 'linux3':
    env['PYSYSPLATFORM'] = 'linux2'
if 'freebsd' in env['PYSYSPLATFORM']:
    env['PYSYSPLATFORM'] = 'freebsd'

if os.sys.platform == 'win32':
    env['OS_FAMILY'] = 'win'
else:
    env['OS_FAMILY'] = 'posix'

if has_option( "cc-use-shell-environment" ) and has_option( "cc" ):
    print("Cannot specify both --cc-use-shell-environment and --cc")
    Exit(1)
elif has_option( "cxx-use-shell-environment" ) and has_option( "cxx" ):
    print("Cannot specify both --cxx-use-shell-environment and --cxx")
    Exit(1)

if has_option( "cxx-use-shell-environment" ):
    env["CXX"] = os.getenv("CXX");
    env["CC"] = env["CXX"]
if has_option( "cc-use-shell-environment" ):
    env["CC"] = os.getenv("CC");

if has_option( "cxx" ):
    env["CC"] = get_option( "cxx" )
    env["CXX"] = get_option( "cxx" )
if has_option( "cc" ):
    env["CC"] = get_option( "cc" )

if env['PYSYSPLATFORM'] in ('linux2', 'freebsd'):
    env['LINK_LIBGROUP_START'] = '-Wl,--start-group'
    env['LINK_LIBGROUP_END'] = '-Wl,--end-group'
    env['RELOBJ_LIBDEPS_START'] = '--whole-archive'
    env['RELOBJ_LIBDEPS_END'] = '--no-whole-archive'
    env['RELOBJ_LIBDEPS_ITEM'] = ''
elif env['PYSYSPLATFORM'] == 'darwin':
    env['RELOBJFLAGS'] = [ '-arch', '$PROCESSOR_ARCHITECTURE' ]
    env['LINK_LIBGROUP_START'] = ''
    env['LINK_LIBGROUP_END'] = ''
    env['RELOBJ_LIBDEPS_START'] = '-all_load'
    env['RELOBJ_LIBDEPS_END'] = ''
    env['RELOBJ_LIBDEPS_ITEM'] = ''
elif env['PYSYSPLATFORM'].startswith('sunos'):
    if force64:
        env['RELOBJFLAGS'] = ['-64']
    env['LINK_LIBGROUP_START'] = '-z rescan'
    env['LINK_LIBGROUP_END'] = ''
    env['RELOBJ_LIBDEPS_START'] = '-z allextract'
    env['RELOBJ_LIBDEPS_END'] = '-z defaultextract'
    env['RELOBJ_LIBDEPS_ITEM'] = ''

env["LIBPATH"] = []

if has_option( "libpath" ):
    env["LIBPATH"] = [get_option( "libpath" )]

if has_option( "cpppath" ):
    env["CPPPATH"] = [get_option( "cpppath" )]

env.Prepend(
    CPPDEFINES=[
        "MONGO_EXPOSE_MACROS" ,
    ],
    CPPPATH=[
        '$BUILD_DIR',
        '$BUILD_DIR/mongo'
    ]
)

extraLibPlaces = []

env['EXTRACPPPATH'] = []
env['EXTRALIBPATH'] = []

def addExtraLibs( s ):
    for x in s.split(","):
        env.Append( EXTRACPPPATH=[ x + "/include" ] )
        env.Append( EXTRALIBPATH=[ x + "/lib" ] )
        extraLibPlaces.append( x + "/lib" )

if has_option( "extrapath" ):
    addExtraLibs( GetOption( "extrapath" ) )

if has_option( "extralib" ):
    for x in GetOption( "extralib" ).split( "," ):
        env.Append( LIBS=[ x ] )

# ---- other build setup -----

if "uname" in dir(os):
    processor = os.uname()[4]
else:
    processor = "i386"

if force32:
    processor = "i386"
if force64:
    processor = "x86_64"

env['PROCESSOR_ARCHITECTURE'] = processor

installDir = DEFAULT_INSTALL_DIR
nixLibPrefix = "lib"

if has_option( "prefix" ):
    installDir = GetOption( "prefix" )

def filterExists(paths):
    return filter(os.path.exists, paths)

if darwin:
    pass
elif linux:

    env.Append( LIBS=['m'] )

    if os.uname()[4] == "x86_64" and not force32:
        linux64 = True
        nixLibPrefix = "lib64"
        env.Append( EXTRALIBPATH=["/usr/lib64" , "/lib64" ] )
        env.Append( LIBS=["pthread"] )

        force64 = False

    if force32:
        env.Append( EXTRALIBPATH=["/usr/lib32"] )
        env.Append( CCFLAGS=["-mmmx"] )

elif solaris:
     env.Append( CPPDEFINES=[ "__sunos__" ] )
     env.Append( LIBS=["socket","resolv"] )

elif freebsd:
    env.Append( LIBS=[ "kvm" ] )
    env.Append( EXTRACPPPATH=[ "/usr/local/include" ] )
    env.Append( EXTRALIBPATH=[ "/usr/local/lib" ] )
    env.Append( CPPDEFINES=[ "__freebsd__" ] )
    env.Append( CCFLAGS=[ "-fno-omit-frame-pointer" ] )

elif openbsd:
    env.Append( EXTRACPPPATH=[ "/usr/local/include" ] )
    env.Append( EXTRALIBPATH=[ "/usr/local/lib" ] )
    env.Append( CPPDEFINES=[ "__openbsd__" ] )

elif windows:
    dynamicCRT = has_option("dynamic-windows")

    env['DIST_ARCHIVE_SUFFIX'] = '.zip'

    # If tools configuration fails to set up 'cl' in the path, fall back to importing the whole
    # shell environment and hope for the best. This will work, for instance, if you have loaded
    # an SDK shell.
    for pathdir in env['ENV']['PATH'].split(os.pathsep):
        if os.path.exists(os.path.join(pathdir, 'cl.exe')):
            break
    else:
        print("NOTE: Tool configuration did not find 'cl' compiler, falling back to os environment")
        env['ENV'] = dict(os.environ)

    env.Append( CPPDEFINES=[ "_UNICODE" ] )
    env.Append( CPPDEFINES=[ "UNICODE" ] )
    env.Append( CPPDEFINES=[ "NOMINMAX" ] )

    # /EHsc exception handling style for visual studio
    # /W3 warning level
    env.Append(CCFLAGS=["/EHsc","/W3"])

    # some warnings we don't like:
    # c4355
    # 'this' : used in base member initializer list
    #    The this pointer is valid only within nonstatic member functions. It cannot be used in the initializer list for a base class.
    # c4800
    # 'type' : forcing value to bool 'true' or 'false' (performance warning)
    #    This warning is generated when a value that is not bool is assigned or coerced into type bool. 
    # c4267
    # 'var' : conversion from 'size_t' to 'type', possible loss of data
    # When compiling with /Wp64, or when compiling on a 64-bit operating system, type is 32 bits but size_t is 64 bits when compiling for 64-bit targets. To fix this warning, use size_t instead of a type.
    # c4244
    # 'conversion' conversion from 'type1' to 'type2', possible loss of data
    #  An integer type is converted to a smaller integer type.
    # c4290
    #  C++ exception specification ignored except to indicate a function is not __declspec(nothrow
    #  A function is declared using exception specification, which Visual C++ accepts but does not
    #  implement
    env.Append( CCFLAGS=["/wd4355", "/wd4800", "/wd4267", "/wd4244", "/wd4290"] )

    # some warnings we should treat as errors:
    # c4099
    #  identifier' : type name first seen using 'objecttype1' now seen using 'objecttype2'
    #    This warning occurs when classes and structs are declared with a mix of struct and class
    #    which can cause linker failures
    env.Append( CCFLAGS=["/we4099"] )

    env.Append( CPPDEFINES=["_CONSOLE","_CRT_SECURE_NO_WARNINGS"] )

    # this would be for pre-compiled headers, could play with it later  
    #env.Append( CCFLAGS=['/Yu"pch.h"'] )

    # docs say don't use /FD from command line (minimal rebuild)
    # /Gy function level linking (implicit when using /Z7)
    # /Z7 debug info goes into each individual .obj file -- no .pdb created 
    env.Append( CCFLAGS= ["/Z7", "/errorReport:none"] )

    # /DEBUG will tell the linker to create a .pdb file
    # which WinDbg and Visual Studio will use to resolve
    # symbols if you want to debug a release-mode image.
    # Note that this means we can't do parallel links in the build.
    #
    # Please also note that this has nothing to do with _DEBUG or optimization.
    env.Append( LINKFLAGS=["/DEBUG"] )

    # /MD:  use the multithreaded, DLL version of the run-time library (MSVCRT.lib/MSVCR###.DLL)
    # /MT:  use the multithreaded, static version of the run-time library (LIBCMT.lib)
    # /MDd: Defines _DEBUG, _MT, _DLL, and uses MSVCRTD.lib/MSVCRD###.DLL
    # /MTd: Defines _DEBUG, _MT, and causes your application to use the
    #       debug multithread version of the run-time library (LIBCMTD.lib)

    winRuntimeLibMap = {
          #dyn   #dbg
        ( False, False ) : "/MT",
        ( False, True  ) : "/MTd",
        ( True,  False ) : "/MD",
        ( True,  True  ) : "/MDd",
    }

    env.Append(CCFLAGS=[winRuntimeLibMap[(dynamicCRT, debugBuild)]])

    if optBuild:
        # /O2:  optimize for speed (as opposed to size)
        # /Oy-: disable frame pointer optimization (overrides /O2, only affects 32-bit)
        # /INCREMENTAL: NO - disable incremental link - avoid the level of indirection for function
        # calls
        env.Append( CCFLAGS=["/O2", "/Oy-"] )
        env.Append( LINKFLAGS=["/INCREMENTAL:NO"])
    else:
        env.Append( CCFLAGS=["/Od"] )

    if debugBuild and not optBuild:
        # /RTC1: - Enable Stack Frame Run-Time Error Checking; Reports when a variable is used
        # without having been initialized (implies /Od: no optimizations)
        env.Append( CCFLAGS=["/RTC1"] )

    env.Append(LIBS=[ 'ws2_32.lib', 'DbgHelp.lib' ])

if nix:

    # -Winvalid-pch Warn if a precompiled header (see Precompiled Headers) is found in the search path but can't be used.
    env.Append( CCFLAGS=["-fPIC",
                         "-fno-strict-aliasing",
                         "-ggdb",
                         "-pthread",
                         "-Wall",
                         "-Wsign-compare",
                         "-Wno-unknown-pragmas",
                         "-Winvalid-pch"] )
    # env.Append( " -Wconversion" ) TODO: this doesn't really work yet
    if linux or darwin:
        env.Append( CCFLAGS=["-Werror", "-pipe"] )

    env.Append( CPPDEFINES=["_FILE_OFFSET_BITS=64"] )
    env.Append( CXXFLAGS=["-Wnon-virtual-dtor", "-Woverloaded-virtual"] )
    env.Append( LINKFLAGS=["-fPIC", "-pthread"] )

    # SERVER-9761: Ensure early detection of missing symbols in dependent libraries at program
    # startup.
    #
    # TODO: Is it necessary to add to both linkflags and shlinkflags, or are LINKFLAGS
    # propagated to SHLINKFLAGS?
    if darwin:
        env.Append( LINKFLAGS=["-Wl,-bind_at_load"] )
        env.Append( SHLINKFLAGS=["-Wl,-bind_at_load"] )
    else:
        env.Append( LINKFLAGS=["-Wl,-z,now"] )
        env.Append( SHLINKFLAGS=["-Wl,-z,now"] )

    if not darwin:
        env.Append( LINKFLAGS=["-rdynamic"] )

    env.Append( LIBS=[] )

    # Allow colorized output
    env['ENV']['TERM'] = os.environ.get('TERM', None)

    if linux and has_option( "gcov" ):
        env.Append( CXXFLAGS=" -fprofile-arcs -ftest-coverage " )
        env.Append( LINKFLAGS=" -fprofile-arcs -ftest-coverage " )

    if optBuild:
        env.Append( CCFLAGS=["-O3"] )
    else:
        env.Append( CCFLAGS=["-O0"] )

    if debugBuild:
        if not optBuild:
            env.Append( CCFLAGS=["-fstack-protector"] )
            env.Append( LINKFLAGS=["-fstack-protector"] )
            env.Append( SHLINKFLAGS=["-fstack-protector"] )
        env['ENV']['GLIBCXX_FORCE_NEW'] = 1; # play nice with valgrind
        env.Append( CPPDEFINES=["_DEBUG"] );

    if force64:
        env.Append( CCFLAGS="-m64" )
        env.Append( LINKFLAGS="-m64" )

    if force32:
        env.Append( CCFLAGS="-m32" )
        env.Append( LINKFLAGS="-m32" )

if "uname" in dir(os):
    hacks = buildscripts.findHacks( os.uname() )
    if hacks is not None:
        hacks.insert( env , { "linux64" : linux64 } )

if has_option( "ssl" ):
    env["MONGO_SSL"] = True
    if windows:
        env.Append( LIBS=["libeay32"] )
        env.Append( LIBS=["ssleay32"] )
    else:
        env.Append( LIBS=["ssl"] )
        env.Append( LIBS=["crypto"] )

try:
    umask = os.umask(022)
except OSError:
    pass

env.Prepend(CPPPATH=['$BUILD_DIR/third_party/gtest-1.7.0/include'])

env.Append( CPPPATH=['$EXTRACPPPATH'],
            LIBPATH=['$EXTRALIBPATH'] )

# --- check system ---

def doConfigure(myenv):

    # Check that the compilers work.
    #
    # TODO: Currently, we have some flags already injected. Eventually, this should test the
    # bare compilers, and we should re-check at the very end that TryCompile and TryLink still
    # work with the flags we have selected.
    conf = Configure(myenv, help=False)

    if 'CheckCXX' in dir( conf ):
        if not conf.CheckCXX():
            print("C++ compiler %s does not work" % (conf.env["CXX"]))
            Exit(1)

    # Only do C checks if CC != CXX
    check_c = (myenv["CC"] != myenv["CXX"])

    if check_c and 'CheckCC' in dir( conf ):
        if not conf.CheckCC():
            print("C compiler %s does not work" % (conf.env["CC"]))
            Exit(1)
    myenv = conf.Finish()

    # Identify the toolchain in use. We currently support the following:
    # TODO: Goes in the env?
    toolchain_gcc = "GCC"
    toolchain_clang = "clang"
    toolchain_msvc = "MSVC"

    def CheckForToolchain(context, toolchain, lang_name, compiler_var, source_suffix):
        test_bodies = {
            toolchain_gcc : (
                # Clang also defines __GNUC__
                """
                #if !defined(__GNUC__) || defined(__clang__)
                #error
                #endif
                """),
            toolchain_clang : (
                """
                #if !defined(__clang__)
                #error
                #endif
                """),
            toolchain_msvc : (
                """
                #if !defined(_MSC_VER)
                #error
                #endif
                """),
        }
        print_tuple = (lang_name, context.env[compiler_var], toolchain)
        context.Message('Checking if %s compiler "%s" is %s... ' % print_tuple)
        # Strip indentation from the test body to ensure that the newline at the end of the
        # endif is the last character in the file (rather than a line of spaces with no
        # newline), and that all of the preprocessor directives start at column zero. Both of
        # these issues can trip up older toolchains.
        test_body = textwrap.dedent(test_bodies[toolchain])
        result = context.TryCompile(test_body, source_suffix)
        context.Result(result)
        return result

    conf = Configure(myenv, help=False, custom_tests = {
        'CheckForToolchain' : CheckForToolchain,
    })

    toolchain = None
    have_toolchain = lambda: toolchain != None
    using_msvc = lambda: toolchain == toolchain_msvc
    using_gcc = lambda: toolchain == toolchain_gcc
    using_clang = lambda: toolchain == toolchain_clang

    if windows:
        toolchain_search_sequence = [toolchain_msvc]
    else:
        toolchain_search_sequence = [toolchain_gcc, toolchain_clang]

    for candidate_toolchain in toolchain_search_sequence:
        if conf.CheckForToolchain(candidate_toolchain, "C++", "CXX", ".cpp"):
            toolchain = candidate_toolchain
            break

    if not have_toolchain():
        print("Couldn't identify the toolchain")
        Exit(1)

    if check_c and not conf.CheckForToolchain(toolchain, "C", "CC", ".c"):
        print("C toolchain doesn't match identified C++ toolchain")
        Exit(1)

    myenv = conf.Finish()

    global use_clang
    use_clang = using_clang()

    # Figure out what our minimum windows version is. If the user has specified, then use
    # that. Otherwise, if they have explicitly selected between 32 bit or 64 bit, choose XP or
    # Vista respectively. Finally, if they haven't done either of these, try invoking the
    # compiler to figure out whether we are doing a 32 or 64 bit build and select as
    # appropriate.
    if windows:
        win_version_min = None
        default_32_bit_min = 'xpsp3'
        default_64_bit_min = 'ws03sp2'
        if has_option('win-version-min'):
            win_version_min = get_option('win-version-min')
        else:
            if force32:
                win_version_min = default_32_bit_min
            elif force64:
                win_version_min = default_64_bit_min
            else:
                def CheckFor64Bit(context):
                    win64_test_body = textwrap.dedent(
                        """
                        #if !defined(_WIN64)
                        #error
                        #endif
                        """
                    )
                    context.Message('Checking if toolchain is in 64-bit mode... ')
                    result = context.TryCompile(win64_test_body, ".c")
                    context.Result(result)
                    return result

                conf = Configure(myenv, help=False, custom_tests = {
                    'CheckFor64Bit' : CheckFor64Bit
                })
                if conf.CheckFor64Bit():
                    win_version_min = default_64_bit_min
                else:
                    win_version_min = default_32_bit_min
                conf.Finish();

        env['WIN_VERSION_MIN'] = win_version_min
        win_version_min = win_version_min_choices[win_version_min]
        env.Append( CPPDEFINES=[("_WIN32_WINNT", "0x" + win_version_min[0])] )
        env.Append( CPPDEFINES=[("NTDDI_VERSION", "0x" + win_version_min[0] + win_version_min[1])] )

    def AddFlagIfSupported(env, tool, extension, flag, **mutation):
        def CheckFlagTest(context, tool, extension, flag):
            test_body = ""
            context.Message('Checking if %s compiler supports %s... ' % (tool, flag))
            ret = context.TryCompile(test_body, extension)
            context.Result(ret)
            return ret

        if using_msvc():
            print("AddFlagIfSupported is not currently supported with MSVC")
            Exit(1)

        test_mutation = mutation
        if using_gcc():
            test_mutation = copy.deepcopy(mutation)
            # GCC helpfully doesn't issue a diagnostic on unknown flags of the form -Wno-xxx
            # unless other diagnostics are triggered. That makes it tough to check for support
            # for -Wno-xxx. To work around, if we see that we are testing for a flag of the
            # form -Wno-xxx (but not -Wno-error=xxx), we also add -Wxxx to the flags. GCC does
            # warn on unknown -Wxxx style flags, so this lets us probe for availablity of
            # -Wno-xxx.
            for kw in test_mutation.keys():
                test_flags = test_mutation[kw]
                for test_flag in test_flags:
                    if test_flag.startswith("-Wno-") and not test_flag.startswith("-Wno-error="):
                        test_flags.append(re.sub("^-Wno-", "-W", test_flag))

        cloned = env.Clone()
        cloned.Append(**test_mutation)

        # For GCC, we don't need anything since bad flags are already errors, but
        # adding -Werror won't hurt. For clang, bad flags are only warnings, so we need -Werror
        # to make them real errors.
        cloned.Append(CCFLAGS=['-Werror'])
        conf = Configure(cloned, help=False, custom_tests = {
                'CheckFlag' : lambda(ctx) : CheckFlagTest(ctx, tool, extension, flag)
        })
        available = conf.CheckFlag()
        conf.Finish()
        if available:
            env.Append(**mutation)
        return available

    def AddToCFLAGSIfSupported(env, flag):
        return AddFlagIfSupported(env, 'C', '.c', flag, CFLAGS=[flag])

    def AddToCCFLAGSIfSupported(env, flag):
        return AddFlagIfSupported(env, 'C', '.c', flag, CCFLAGS=[flag])

    def AddToCXXFLAGSIfSupported(env, flag):
        return AddFlagIfSupported(env, 'C++', '.cpp', flag, CXXFLAGS=[flag])

    if using_gcc() or using_clang():
        # This warning was added in g++-4.8.
        AddToCCFLAGSIfSupported(myenv, '-Wno-unused-local-typedefs')

        # Clang likes to warn about unused functions, which seems a tad aggressive and breaks
        # -Werror, which we want to be able to use.
        AddToCCFLAGSIfSupported(myenv, '-Wno-unused-function')

        # TODO: Note that the following two flags are added to CCFLAGS even though they are
        # really C++ specific. We need to do this because SCons passes CXXFLAGS *before*
        # CCFLAGS, but CCFLAGS contains -Wall, which re-enables the warnings we are trying to
        # suppress. In the future, we should move all warning flags to CCWARNFLAGS and
        # CXXWARNFLAGS and add these to CCOM and CXXCOM as appropriate.
        #
        # Clang likes to warn about unused private fields, but some of our third_party
        # libraries have such things.
        AddToCCFLAGSIfSupported(myenv, '-Wno-unused-private-field')

        # Prevents warning about using deprecated features (such as auto_ptr in c++11)
        # Using -Wno-error=deprecated-declarations does not seem to work on some compilers,
        # including at least g++-4.6.
        AddToCCFLAGSIfSupported(myenv, "-Wno-deprecated-declarations")

        # As of clang-3.4, this warning appears in v8, and gets escalated to an error.
        AddToCCFLAGSIfSupported(myenv, "-Wno-tautological-constant-out-of-range-compare")

        # New in clang-3.4, trips up things mostly in third_party, but in a few places in the
        # primary mongo sources as well.
        AddToCCFLAGSIfSupported(myenv, "-Wno-unused-const-variable")

    if has_option('c++11'):
        # The Microsoft compiler does not need a switch to enable C++11. Again we should be
        # checking for MSVC, not windows. In theory, we might be using clang or icc on windows.
        if not using_msvc():
            # For our other compilers (gcc and clang) we need to pass -std=c++0x or -std=c++11,
            # but we prefer the latter. Try that first, and fall back to c++0x if we don't
            # detect that --std=c++11 works.
            if not AddToCXXFLAGSIfSupported(myenv, '-std=c++11'):
                if not AddToCXXFLAGSIfSupported(myenv, '-std=c++0x'):
                    print( 'C++11 mode requested, but cannot find a flag to enable it' )
                    Exit(1)

            if not AddToCFLAGSIfSupported(myenv, '-std=c99'):
                print( 'C++11 mode selected for C++ files, but failed to enable C99 for C files' )

    # This needs to happen before we check for libc++, since it affects whether libc++ is available.
    if darwin and has_option('osx-version-min'):
        min_version = get_option('osx-version-min')
        min_version_flag = '-mmacosx-version-min=%s' % (min_version)
        if not AddToCCFLAGSIfSupported(myenv, min_version_flag):
            print( "Can't set minimum OS X version with this compiler" )
            Exit(1)
        myenv.AppendUnique(LINKFLAGS=[min_version_flag])

    if has_option('libc++'):
        if not using_clang():
            print( 'libc++ is currently only supported for clang')
            Exit(1)
        if darwin and has_option('osx-version-min') and versiontuple(min_version) < versiontuple('10.7'):
            print("Warning: You passed option 'libc++'. You probably want to also pass 'osx-version-min=10.7' or higher for libc++ support.")
        if AddToCXXFLAGSIfSupported(myenv, '-stdlib=libc++'):
            myenv.Append(LINKFLAGS=['-stdlib=libc++'])
        else:
            print( 'libc++ requested, but compiler does not support -stdlib=libc++' )
            Exit(1)

    # Check to see if we are trying to use an outdated libstdc++ in C++11 mode. This is
    # primarly to help people using clang in C++11 mode on OS X but forgetting to use
    # --libc++. We would, ideally, check the __GLIBCXX__ version, but for various reasons this
    # is not workable. Instead, we switch on the fact that std::is_nothrow_constructible wasn't
    # introduced until libstdc++ 4.6.0. Earlier versions of libstdc++ than 4.6 are unlikely to
    # work well anyway.
    if has_option('c++11') and not has_option('libc++'):

        def CheckModernLibStdCxx(context):

            test_body = """
            #include <vector>
            #include <cstdlib>
            #if defined(__GLIBCXX__)
            #include <type_traits>
            int main() {
                return std::is_nothrow_constructible<int>::value ? EXIT_SUCCESS : EXIT_FAILURE;
            }
            #endif
            """

            context.Message('Checking for libstdc++ 4.6.0 or better (for C++11 support)... ')
            ret = context.TryCompile(textwrap.dedent(test_body), ".cpp")
            context.Result(ret)
            return ret

        conf = Configure(myenv, help=False, custom_tests = {
            'CheckModernLibStdCxx' : CheckModernLibStdCxx,
        })
        haveGoodLibStdCxx = conf.CheckModernLibStdCxx()
        conf.Finish()

        if not haveGoodLibStdCxx:
            print( 'Detected libstdc++ is too old to support C++11 mode' )
            if darwin:
                print( 'Try building with --libc++ and --osx-version-min=10.7 or higher' )
            Exit(1)

    if has_option('sanitize'):
        if not (using_clang() or using_gcc()):
            print( 'sanitize is only supported with clang or gcc')
            Exit(1)
        sanitizer_option = '-fsanitize=' + GetOption('sanitize')
        if AddToCCFLAGSIfSupported(myenv, sanitizer_option):
            myenv.Append(LINKFLAGS=[sanitizer_option])
            myenv.Append(CCFLAGS=['-fno-omit-frame-pointer'])
        else:
            print( 'Failed to enable sanitizer with flag: ' + sanitizer_option )
            Exit(1)

    # Apply any link time optimization settings as selected by the 'lto' option.
    if has_option('lto'):
        if using_msvc():
            # Note that this is actually more aggressive than LTO, it is whole program
            # optimization due to /GL. However, this is historically what we have done for
            # windows, so we are keeping it.
            #
            # /GL implies /LTCG, so no need to say it in CCFLAGS, but we do need /LTCG on the
            # link flags.
            myenv.Append(CCFLAGS=['/GL'])
            myenv.Append(LINKFLAGS=['/LTCG'])
            myenv.Append(ARFLAGS=['/LTCG'])
        elif using_gcc() or using_clang():
            # For GCC and clang, the flag is -flto, and we need to pass it both on the compile
            # and link lines.
            if AddToCCFLAGSIfSupported(myenv, '-flto'):
                myenv.Append(LINKFLAGS=['-flto'])
            else:
                print( "Link time optimization requested, " +
                       "but selected compiler does not honor -flto" )
                Exit(1)
        else:
            printf("Don't know how to enable --lto on current toolchain")
            Exit(1)

    # When using msvc, check for support for __declspec(thread), unless we have been asked
    # explicitly not to use it. For other compilers, see if __thread works.
    if using_msvc():
        haveDeclSpecThread = False
        if not has_option("disable-declspec-thread"):
            def CheckDeclspecThread(context):
                test_body = """
                __declspec( thread ) int tsp_int;
                int main(int argc, char* argv[]) {
                    tsp_int = argc;
                    return 0;
                }
                """
                context.Message('Checking for __declspec(thread)... ')
                ret = context.TryLink(textwrap.dedent(test_body), ".cpp")
                context.Result(ret)
                return ret
            conf = Configure(myenv, help=False, custom_tests = {
                'CheckDeclspecThread' : CheckDeclspecThread,
            })
            haveDeclSpecThread = conf.CheckDeclspecThread()
            conf.Finish()
        if haveDeclSpecThread:
            myenv.Append(CPPDEFINES=['MONGO_HAVE___DECLSPEC_THREAD'])
    else:
        def CheckUUThread(context):
            test_body = """
            __thread int tsp_int;
            int main(int argc, char* argv[]) {
                tsp_int = argc;
                return 0;
            }
            """
            context.Message('Checking for __thread... ')
            ret = context.TryLink(textwrap.dedent(test_body), ".cpp")
            context.Result(ret)
            return ret
        conf = Configure(myenv, help=False, custom_tests = {
            'CheckUUThread' : CheckUUThread,
        })
        haveUUThread = conf.CheckUUThread()
        conf.Finish()
        if haveUUThread:
            myenv.Append(CPPDEFINES=['MONGO_HAVE___THREAD'])

    if using_msvc():
        # TODO: This is really only needed for MSVC 12, but we have no current way to know
        # which MSVC version we have. Presuming that this is harmless on other MSVC
        # implementations. Without this, gtest doesn't build. We need to apply universally, not
        # just while builing gtest.
        myenv.Append(CPPDEFINES=[('_VARIADIC_MAX', 10)])

    conf = Configure(myenv)
    libdeps.setup_conftests(conf)

    if not conf.CheckCXXHeader( "boost/version.hpp" ):
        print( "can't find boost headers" )
        Exit(1)

    conf.env.Append(CPPDEFINES=[("BOOST_THREAD_VERSION", "2")])

    # Note that on Windows with using-system-boost builds, the following 
    # FindSysLibDep calls do nothing useful (but nothing problematic either)
    for b in boostLibs:
        boostlib = "boost_" + b
        conf.FindSysLibDep( boostlib, [ boostlib + "-mt", boostlib ], language='C++' )

    if conf.CheckHeader('unistd.h'):
        conf.env['MONGO_HAVE_HEADER_UNISTD_H'] = True

    if solaris or conf.CheckDeclaration('clock_gettime', includes='#include <time.h>'):
        conf.CheckLib('rt')

    if solaris:
        conf.CheckLib( "nsl" )

    conf.env['MONGO_SASL'] = bool(has_option("use-sasl-client"))

    if conf.env['MONGO_SASL'] and not conf.CheckLibWithHeader(
            "sasl2", 
            ["stddef.h","sasl/sasl.h"], 
            "C", 
            "sasl_version_info(0, 0, 0, 0, 0, 0);", 
            autoadd=False ):
        Exit(1)

    # requires ports devel/libexecinfo to be installed
    if freebsd or openbsd:
        if not conf.CheckLib("execinfo"):
            Exit(1)

    return conf.Finish()

env = doConfigure( env )

env['PDB'] = '${TARGET.base}.pdb'

def checkErrorCodes():
    import buildscripts.errorcodes as x
    if x.checkErrorCodes() == False:
        print( "next id to use:" + str( x.getNextCode() ) )
        Exit(-1)

checkErrorCodes()

#  ---- Docs ----
def build_docs(env, target, source):
    from buildscripts import docs
    docs.main()

env.Alias("docs", [], [build_docs])
env.AlwaysBuild("docs")


# --- lint ----

def doLint( env , target , source ):
    import buildscripts.lint
    if not buildscripts.lint.run_lint( [ "src/mongo/" ] ):
        raise Exception( "lint errors" )

env.Alias( "lint" , [] , [ doLint ] )
env.AlwaysBuild( "lint" )

env['INSTALL_DIR'] = installDir

# --- an uninstall target ---
if len(COMMAND_LINE_TARGETS) > 0 and 'uninstall' in COMMAND_LINE_TARGETS:
    SetOption("clean", 1)
    # By inspection, changing COMMAND_LINE_TARGETS here doesn't do
    # what we want, but changing BUILD_TARGETS does.
    BUILD_TARGETS.remove("uninstall")
    BUILD_TARGETS.append("install")

# The following symbols are exported for use in subordinate SConscript files.
# Ideally, the SConscript files would be purely declarative.  They would only
# import build environment objects, and would contain few or no conditional
# statements or branches.
#
# Currently, however, the SConscript files do need some predicates for
# conditional decision making that hasn't been moved up to this SConstruct file,
# and they are exported here, as well.
Export("env")
Export("get_option")
Export("has_option")
Export("darwin windows solaris linux freebsd nix")
Export("debugBuild optBuild")
Export("use_clang")

env.SConscript('src/SConscript', variant_dir='$BUILD_DIR', duplicate=False)
env.SConscript('src/SConscript.client', variant_dir='$BUILD_DIR', duplicate=False)

# --- Coverage ---
if has_option("gcov"):
    env['GCOV_BASE_DIR'] = env.subst(Dir(".").abspath)
    env['GCOV_BUILD_DIR'] = env.subst(Dir("$BUILD_DIR").abspath).replace("#", "")

    # Zero out all the counters -- depends on tests being built
    env.Alias(
        'zero_counters',
        ['unittests','clientTests'],
        ['lcov -z -b $GCOV_BASE_DIR -d $GCOV_BUILD_DIR']
    )
    env.AlwaysBuild('zero_counters')

    # Generates test coverage information -- depends on tests being run
    env.Command(
        'coverage.info',
        ['zero_counters', 'test', 'smokeClient', 'integration'],
        'lcov --no-external -c -b $GCOV_BASE_DIR -d $GCOV_BUILD_DIR -o $TARGET'
    )
    env.AlwaysBuild('coverage.info')

    # Strip third_party and build related coverage info
    env.Alias(
        'strip_coverage',
        ['coverage.info'],
        [
            'lcov -r coverage.info src/third_party/\* -o coverage.info',
            'lcov -r coverage.info build/\* -o coverage.info',
            'lcov -r coverage.info \*_test.cpp -o coverage.info',
            'lcov -r coverage.info src/mongo/client/examples/\* -o coverage.info',
            'lcov -r coverage.info src/mongo/dbtests/\* -o coverage.info',
            'lcov -r coverage.info src/mongo/unittest/\* -o coverage.info',
            'lcov -r coverage.info src/mongo/bson/bsondemo/\* -o coverage.info',
        ]
    )
    env.AlwaysBuild('strip_coverage')

    # Generates the HTML output in "coverage" directory
    env.Command(
        Dir('coverage'),
        'strip_coverage',
        [
            'rm -rf coverage',
            'genhtml --legend -t "MongoDB C++ Driver Coverage" -o $TARGET coverage.info'
        ]
    )
    env.AlwaysBuild('coverage')

env.Alias('all', ['unittests', 'clientTests'])
