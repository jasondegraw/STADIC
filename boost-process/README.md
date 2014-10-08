This directory contains a modified version of the Boost.Process library. Since the library is not 
officially part of Boost, it needs to be kept separate (at least for now).

The modificatications are fairly simple:

* Update a few things for the newest version of Boost
* Change the treatment of the standard error in pipelines. Standard error can now be captured for
each process that is part of a pipeline
