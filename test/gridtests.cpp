#include "gridmaker.h"
#include "gtest/gtest.h"
#include <fstream>
#include <string>
#include "functions.h"
#include <vector>

/*
TEST(GridTests, SimpleInset){
    std::vector<std::string> files;
    files.clear();
    files.push_back("simple.rad");
    stadic::GridMaker grid(files);

    std::vector<std::string> layers;
    layers.clear();
    layers.push_back("l_floor");
    grid.setLayerNames(layers);
    grid.setOffset(12);
    grid.setSpaceX(24);
    grid.setSpaceY(24);
    grid.setOffsetZ(30);
    ASSERT_TRUE(grid.makeGrid());
}

TEST(GridTests, SimpleOffset){
    std::vector<std::string> files;
    files.clear();
    files.push_back("simple.rad");
    stadic::GridMaker grid(files);

    std::vector<std::string> layers;
    layers.clear();
    layers.push_back("l_floor");
    grid.setLayerNames(layers);
    grid.setOffsetX(24);
    grid.setOffsetY(24);
    grid.setSpaceX(24);
    grid.setSpaceY(24);
    grid.setOffsetZ(30);
    ASSERT_TRUE(grid.makeGrid());
}
*/

TEST(GridTests, Complicated)
{
    std::vector<std::string> files;
    files.clear();
    files.push_back("complicated.rad");
    stadic::GridMaker grid(files);

    std::vector<std::string> layers;
    layers.clear();
    layers.push_back("l_groundfloor");
    layers.push_back("l_firstfloor");
    layers.push_back("l_secondfloor");
    layers.push_back("l_stairs");
    grid.setLayerNames(layers);
    grid.setOffset(24);
    grid.setSpaceX(24);
    grid.setSpaceY(24);
    grid.setOffsetZ(30);
    ASSERT_TRUE(grid.makeGrid());
    ASSERT_TRUE(grid.writePTS("complicatedgrid.pts"));
    ASSERT_TRUE(grid.viewPTS("", "se"));
}
