"""Build the C++ client docs.
"""

from __future__ import with_statement

import os
import shutil
import subprocess

def clean_dir(dir):
    try:
        shutil.rmtree(dir)
    except:
        pass
    os.makedirs(dir)

def gen_cplusplus(dir):
    clean_dir(dir)
    clean_dir("docs/doxygen")

    # Too noisy...
    with open("/dev/null") as null:
        subprocess.call(["doxygen", "etc/doxygen/config"], stdout=null, stderr=null)

    os.rename("docs/doxygen/html", dir)

def version():
    """Get the server version from doxygenConfig.
    """
    with open("etc/doxygen/config") as f:
        for line in f.readlines():
            if line.startswith("PROJECT_NUMBER"):
                return line.split("=")[1].strip()

def link_current(version):
    """Create current link to the most recently generated documentation
    """
    print("Updating 'current' docs symlink")

    link_path = 'docs/html/cplusplus/current'

    try:
        os.unlink(link_path)
    except OSError:
        pass

    os.symlink(version, link_path)

def main():
    v = version()
    print("Generating C++ docs in docs/html/cplusplus/%s" % v)
    gen_cplusplus("docs/html/cplusplus/%s" % v)
    link_current(v)

if __name__ == "__main__":
    main()


