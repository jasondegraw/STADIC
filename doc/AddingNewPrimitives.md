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

All material primitive objects should be initialized with acceptable default values. Geometric primitives may
be initialized to have no contents, but must cause an error if the user attempts to write out the primitive
without adding data to the primitive.

Add Basic Tests
---------------

A basic test of the functionality of the new object should be added to primitivetests.cpp. At a minimum, this
test should:

* Construct an object with valid arguments.
* Attempt to change the argument values to invalid values and verify that the changes are not accepted.
* Construct an object with invalid arguments and verify that default values have been used instead.

For objects in which there are fewer rules about what is acceptable or not, this test may be very short. The most
important things is to make sure that validation works.

Connect To I/O
--------------

Test I/O
--------
