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

We have been naming the primitives by appending the general primitive type to the Radiance name of the
primitive. In this scheme the "plastic" primitive is PlasticMaterial, the "polygon" primitive is
PolygonGeometry, and so on.

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

The individual primitives are read into memory in a static RadPrimitive method called fromRad (see
radprimitive.cpp). The important part that needs to be modified is a switch statement in the middle
of the function:

    switch(typeFromString(list[1])) {
    case Polygon:
        rad = new PolygonGeometry();
        break;
    case Plastic:
        rad = new PlasticMaterial();
        break;
    case BSDF:
        rad = new BSDFMaterial();
        break;
    /*  ... others ... */
    default:
        rad = new RadPrimitive();
        rad->setType(list[1]);
        break;
    }

The switch statement operates on the Type enumeration defined in the RadPrimitive class in radprimitives.h,
so the first step is to make sure that primitive is represented there and in the type strings array so that
the switch statement will work properly. Then add the new primitive in as a case statement.

Test I/O
--------

Add a test (or tests) in radfiletests.cpp that verifies that the objects are read correctly and are actually
the correct object. The test (or tests) should make sure that both correct and incorrect input are correctly
handled. To test correct input, add a new correct primitive to "supportedprimitives.rad" and add a separate
file for incorrect input (e.g. badglassmaterial.rad). In this context, "incorrect input" is input that can 
be read as a primitive but is not correct for the particular primitive. For example, a plastic primitive's 
third set of arguments is supposed to have five items in it (red, green, blue, specularity, roughness). This
could appear in the input file as:

    5 0.5 0.5 0.5 0.05 0.01

Examples of incorrect, but consistent, input for a plastic primitive would be

    5 1.5 0.5 0.5 0.05 0.01

(the red value is too large) and

    4 1.5 0.5 0.5 0.05
    
(there is no roughness). These are both legal inputs in the sense that the input is internally consistent
as a Radiance primitive, but the values are wrong for a plastic. However, this is not what we're looking for:

    5 0.5 0.5 0.5 0.05    # Don't do this!

This is not the kind of incorrect input that will test whether the code correctly recognizes a plastic type,
but is more likely testing how the code handles malformed primitive input. Don't test that here - it is tested
elsewhere.
