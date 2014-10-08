Adding New Primitives to STADIC
===============================

Adding new primitives to STADIC is a four step process:

1. New class - Write a new class that inherits from RadPrimitive
2. Basic tests - Write tests of the primitive functionality, putting the tests in primitivetests.cpp
3. Connect to I/O - Connect the new class into the .rad file reading function fromRad in radprimitives.cpp
4. Test I/O - Add at least one instance of the primitive to the tests in radfiletests.cpp
