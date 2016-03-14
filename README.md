
# The code has moved and no longer maintained 
# Please wisit:
# https://github.com/NATTools
# for updated and maintaned code



NATtool is a package that contains several stand alone submodules.

-* Compiling *-
run ./bootstrap.sh to crate the ./configure script. This requires that autotools and autoconfigure is intalled

Then do a ./configure and make to compile

For OSX the following ./configure command can be used:
./configure --with-check --enable-gcov CFLAGS="-I/opt/local/include" LDFLAGS=-"L/opt/local/lib"


This will build the three libraries sockaddrutillib, stunlib and icelib. They are all standalone 
libs and can be build seperately if you want to. 

For osx it is recomended to build with a gcc version from port (MacPorts)
Before running the configure script make sure:
- Xcode commnd line tols are installed (xcode-select --install)
- gcc is installed. (port install gcc49)
- automake and libtool is installed
- check is installed (port install check)


** samplecode **
Module containing sample code of library usage. Note that this directory is not part of the distribution if you do a "make dist".
To build you ned to cd into the directory and do the ./configure && make dance.

** ICElib **
A implementation of RFC 5245 Interactive Connectivity Establishment (ICE):
A Protocol for Network Address Translator (NAT) Traversal for Offer/Answer 
Protocols

** STUNlib **
A implementation of RFC 5766 Traversal Using Relays around NAT (TURN):
Relay Extensions to Session Traversal Utilities for NAT (STUN).

It also has support for RFC 6156 Traversal Using Relays around NAT (TURN) 
Extension for IPv6.

Please note that this library currently is a bit client focused. But all message parsing is the same so it is still useful for a server implementation. The client side has some of the necessary state machines implemented to make usage of the library very simple for the client side. 

** Sockaddr Util **
Convenience library to make it easier to work with sockaddr structs.
