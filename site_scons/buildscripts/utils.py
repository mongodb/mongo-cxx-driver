import os
import re
import subprocess
import sys

def getAllSourceFiles( arr=None , prefix="." ):
    if arr is None:
        arr = []

    if not os.path.isdir( prefix ):
        # assume a file
        arr.append( prefix )
        return arr

    for x in os.listdir( prefix ):
        if x.startswith( "." ) or x.startswith( "pcre-" ) or x.startswith( "32bit" ) or x.startswith( "mongodb-" ) or x.startswith("debian") or x.startswith( "mongo-cxx-driver" ):
            continue
        full = prefix + "/" + x
        if os.path.isdir( full ) and not os.path.islink( full ):
            getAllSourceFiles( arr , full )
        else:
            if full.endswith( ".cpp" ) or full.endswith( ".h" ) or full.endswith( ".c" ):
                full = full.replace( "//" , "/" )
                arr.append( full )

    return arr

# various utilities that are handy
def find_python(min_version=(2, 5)):
    try:
        if sys.version_info >= min_version:
            return sys.executable
    except AttributeError:
        # In case the version of Python is somehow missing sys.version_info or sys.executable.
        pass

    version = re.compile(r'[Pp]ython ([\d\.]+)', re.MULTILINE)
    binaries = ('python27', 'python2.7', 'python26', 'python2.6', 'python25', 'python2.5', 'python')
    for binary in binaries:
        try:
            out, err = subprocess.Popen([binary, '-V'], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
            for stream in (out, err):
                match = version.search(stream)
                if match:
                    versiontuple = tuple(map(int, match.group(1).split('.')))
                    if versiontuple >= min_version:
                        return which(binary)
        except:
            pass

    raise Exception('could not find suitable Python (version >= %s)' % '.'.join(str(v) for v in min_version))
