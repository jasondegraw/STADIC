Adding New Primitives to STADIC
===============================

Adding new primitives to STADIC is a four step process:

1. Add new class - Write a new class that inherits from RadPrimitive
2. Add basic tests - Write tests of the primitive functionality, putting the tests in primitivetests.cpp
3. Connect to I/O - Connect the new class into the .rad file reading function fromRad in radprimitives.cpp
4. Test I/O - Add at least one instance of the primitive to the tests in radfiletests.cpp

Add New Class
-------------

The primary function of the RadPrimitive subclasses is to validate input and provide better access to the
data elments inside that are held in the RadPrimitive superclass. The data is stored as strings in the 
Radpimitive class - the subclass is responsible for making sure that the data makes sense for the particular
type. Note that primitives that do not have a subclass are still supported - the data will just not be
validated and the only access to the object data will be through RadPrimitives getArg/setArg functions.

Add Basic Tests
---------------

Connect To I/O
--------------

Test I/O
--------
