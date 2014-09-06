#include "objects.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>

#include <iostream>

TEST(ObjectTests, Process)
{
    stadic::Process proc;
    proc.setProgram("controltests");
    proc.start();
    ASSERT_TRUE(proc.waitForFinished(-1));
    // These are pretty lame test, but will do for now.
    EXPECT_FALSE(proc.error().empty());
    EXPECT_FALSE(proc.output().empty());
}
