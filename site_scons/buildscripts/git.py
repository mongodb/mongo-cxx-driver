import os
import re

def getGitBranch():
    if not os.path.exists( ".git" ) or not os.path.isdir(".git"):
        return None

    version = open( ".git/HEAD" ,'r' ).read().strip()
    if not version.startswith( "ref: " ):
        return version
    version = version.split( "/" )
    version = version[len(version)-1]
    return version

def getGitBranchString( prefix="" , postfix="" ):
    t = re.compile( '[/\\\]' ).split( os.getcwd() )
    if len(t) > 2 and t[len(t)-1] == "mongo":
        par = t[len(t)-2]
        m = re.compile( ".*_([vV]\d+\.\d+)$" ).match( par )
        if m is not None:
            return prefix + m.group(1).lower() + postfix
        if par.find("Nightly") > 0:
            return ""


    b = getGitBranch()
    if b == None or b == "master":
        return ""
    return prefix + b + postfix

def getGitVersion():
    if not os.path.exists( ".git" ) or not os.path.isdir(".git"):
        return "nogitversion"

    version = open( ".git/HEAD" ,'r' ).read().strip()
    if not version.startswith( "ref: " ):
        return version
    version = version[5:]
    f = ".git/" + version
    if not os.path.exists( f ):
        return version
    return open( f , 'r' ).read().strip()
