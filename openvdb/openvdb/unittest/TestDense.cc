// Copyright Contributors to the OpenVDB Project
// SPDX-License-Identifier: MPL-2.0

//#define BENCHMARK_TEST

#include "../openvdb.h"
#include "../tools/LevelSetSphere.h"
#include "../tools/Count.h"
#include "../tools/Dense.h"
#include "../Exceptions.h"
#ifdef BENCHMARK_TEST
#include "../util/CpuTimer.h"
#endif

#include <gtest/gtest.h>

#include <sstream>


class TestDense: public ::testing::Test
{
public:
    template <openvdb::tools::MemoryLayout Layout>
    void testCopy();
    template <openvdb::tools::MemoryLayout Layout>
    void testCopyBool();
    template <openvdb::tools::MemoryLayout Layout>
    void testCopyFromDenseWithOffset();
    template <openvdb::tools::MemoryLayout Layout>
    void testDense2Sparse();
    template <openvdb::tools::MemoryLayout Layout>
    void testDense2Sparse2();
    template <openvdb::tools::MemoryLayout Layout>
    void testInvalidBBox();
    template <openvdb::tools::MemoryLayout Layout>
    void testDense2Sparse2Dense();
};


TEST_F(TestDense, testDenseZYX)
{
    const openvdb::CoordBBox bbox(openvdb::Coord(-40,-5, 6),
                                  openvdb::Coord(-11, 7,22));
    openvdb::tools::Dense<float> dense(bbox);//LayoutZYX is the default

    // Check Desne::origin()
    EXPECT_TRUE(openvdb::Coord(-40,-5, 6) == dense.origin());

    // Check coordToOffset and offsetToCoord
    size_t offset = 0;
    for (openvdb::Coord P(bbox.min()); P[0] <= bbox.max()[0]; ++P[0]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[2] = bbox.min()[2]; P[2] <= bbox.max()[2]; ++P[2]) {
                //std::cerr << "offset = " << offset << " P = " << P << std::endl;
                EXPECT_EQ(offset, dense.coordToOffset(P));
                EXPECT_EQ(P - dense.origin(), dense.offsetToLocalCoord(offset));
                EXPECT_EQ(P, dense.offsetToCoord(offset));
                ++offset;
            }
        }
    }

    // Check Dense::valueCount
    const int size = static_cast<int>(dense.valueCount());
    EXPECT_EQ(30*13*17, size);

    // Check Dense::fill(float) and Dense::getValue(size_t)
    const float v = 0.234f;
    dense.fill(v);
    for (int i=0; i<size; ++i) {
        EXPECT_NEAR(v, dense.getValue(i),/*tolerance=*/0.0001);
    }

    // Check Dense::data() and Dense::getValue(Coord, float)
    float* a = dense.data();
    int s = size;
    while(s--) EXPECT_NEAR(v, *a++, /*tolerance=*/0.0001);

    for (openvdb::Coord P(bbox.min()); P[0] <= bbox.max()[0]; ++P[0]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[2] = bbox.min()[2]; P[2] <= bbox.max()[2]; ++P[2]) {
                EXPECT_NEAR(v, dense.getValue(P), /*tolerance=*/0.0001);
            }
        }
    }

    // Check Dense::setValue(Coord, float)
    const openvdb::Coord C(-30, 3,12);
    const float v1 = 3.45f;
    dense.setValue(C, v1);
    for (openvdb::Coord P(bbox.min()); P[0] <= bbox.max()[0]; ++P[0]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[2] = bbox.min()[2]; P[2] <= bbox.max()[2]; ++P[2]) {
                EXPECT_NEAR(P==C ? v1 : v, dense.getValue(P),
                    /*tolerance=*/0.0001);
            }
        }
    }

    // Check Dense::setValue(size_t, size_t, size_t, float)
    dense.setValue(C, v);
    const openvdb::Coord L(1,2,3), C1 = bbox.min() + L;
    dense.setValue(L[0], L[1], L[2], v1);
    for (openvdb::Coord P(bbox.min()); P[0] <= bbox.max()[0]; ++P[0]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[2] = bbox.min()[2]; P[2] <= bbox.max()[2]; ++P[2]) {
                EXPECT_NEAR(P==C1 ? v1 : v, dense.getValue(P),
                    /*tolerance=*/0.0001);
            }
        }
    }

}

TEST_F(TestDense, testDenseXYZ)
{
    const openvdb::CoordBBox bbox(openvdb::Coord(-40,-5, 6),
                                  openvdb::Coord(-11, 7,22));
    openvdb::tools::Dense<float, openvdb::tools::LayoutXYZ> dense(bbox);

    // Check Desne::origin()
    EXPECT_TRUE(openvdb::Coord(-40,-5, 6) == dense.origin());

    // Check coordToOffset and offsetToCoord
    size_t offset = 0;
    for (openvdb::Coord P(bbox.min()); P[2] <= bbox.max()[2]; ++P[2]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[0] = bbox.min()[0]; P[0] <= bbox.max()[0]; ++P[0]) {
                //std::cerr << "offset = " << offset << " P = " << P << std::endl;
                EXPECT_EQ(offset, dense.coordToOffset(P));
                EXPECT_EQ(P - dense.origin(), dense.offsetToLocalCoord(offset));
                EXPECT_EQ(P, dense.offsetToCoord(offset));
                ++offset;
            }
        }
    }

    // Check Dense::valueCount
    const int size = static_cast<int>(dense.valueCount());
    EXPECT_EQ(30*13*17, size);

    // Check Dense::fill(float) and Dense::getValue(size_t)
    const float v = 0.234f;
    dense.fill(v);
    for (int i=0; i<size; ++i) {
        EXPECT_NEAR(v, dense.getValue(i),/*tolerance=*/0.0001);
    }

    // Check Dense::data() and Dense::getValue(Coord, float)
    float* a = dense.data();
    int s = size;
    while(s--) EXPECT_NEAR(v, *a++, /*tolerance=*/0.0001);

    for (openvdb::Coord P(bbox.min()); P[2] <= bbox.max()[2]; ++P[2]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[0] = bbox.min()[0]; P[0] <= bbox.max()[0]; ++P[0]) {
                EXPECT_NEAR(v, dense.getValue(P), /*tolerance=*/0.0001);
            }
        }
    }

    // Check Dense::setValue(Coord, float)
    const openvdb::Coord C(-30, 3,12);
    const float v1 = 3.45f;
    dense.setValue(C, v1);
    for (openvdb::Coord P(bbox.min()); P[2] <= bbox.max()[2]; ++P[2]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[0] = bbox.min()[0]; P[0] <= bbox.max()[0]; ++P[0]) {
                EXPECT_NEAR(P==C ? v1 : v, dense.getValue(P),
                    /*tolerance=*/0.0001);
            }
        }
    }

    // Check Dense::setValue(size_t, size_t, size_t, float)
    dense.setValue(C, v);
    const openvdb::Coord L(1,2,3), C1 = bbox.min() + L;
    dense.setValue(L[0], L[1], L[2], v1);
    for (openvdb::Coord P(bbox.min()); P[2] <= bbox.max()[2]; ++P[2]) {
        for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
            for (P[0] = bbox.min()[0]; P[0] <= bbox.max()[0]; ++P[0]) {
                EXPECT_NEAR(P==C1 ? v1 : v, dense.getValue(P),
                    /*tolerance=*/0.0001);
            }
        }
    }

}


// The check is so slow that we're going to multi-thread it :)
template <typename TreeT,
          typename DenseT = openvdb::tools::Dense<typename TreeT::ValueType,
                                                  openvdb::tools::LayoutZYX> >
class CheckDense
{
public:
    typedef typename TreeT::ValueType ValueT;

    CheckDense() : mTree(NULL), mDense(NULL)
    {
        EXPECT_TRUE(DenseT::memoryLayout() == openvdb::tools::LayoutZYX ||
                       DenseT::memoryLayout() == openvdb::tools::LayoutXYZ );
    }

    void check(const TreeT& tree, const DenseT& dense)
    {
        mTree  = &tree;
        mDense = &dense;
        tbb::parallel_for(dense.bbox(), *this);
    }
    void operator()(const openvdb::CoordBBox& bbox) const
    {
        openvdb::tree::ValueAccessor<const TreeT> acc(*mTree);

        if (DenseT::memoryLayout() == openvdb::tools::LayoutZYX) {//resolved at compiletime
            for (openvdb::Coord P(bbox.min()); P[0] <= bbox.max()[0]; ++P[0]) {
                for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
                    for (P[2] = bbox.min()[2]; P[2] <= bbox.max()[2]; ++P[2]) {
                        EXPECT_NEAR(acc.getValue(P), mDense->getValue(P),
                                                     /*tolerance=*/0.0001);
                    }
                }
            }
        } else {
             for (openvdb::Coord P(bbox.min()); P[2] <= bbox.max()[2]; ++P[2]) {
                for (P[1] = bbox.min()[1]; P[1] <= bbox.max()[1]; ++P[1]) {
                    for (P[0] = bbox.min()[0]; P[0] <= bbox.max()[0]; ++P[0]) {
                        EXPECT_NEAR(acc.getValue(P), mDense->getValue(P),
                                                     /*tolerance=*/0.0001);
                    }
                }
            }
        }
    }
private:
    const TreeT*  mTree;
    const DenseT* mDense;
};// CheckDense

template <openvdb::tools::MemoryLayout Layout>
void
TestDense::testCopy()
{
    using namespace openvdb;

    //std::cerr << "\nTesting testCopy with "
    //          << (Layout == tools::LayoutXYZ ? "XYZ" : "ZYX") << " memory layout"
    //          << std::endl;

    typedef tools::Dense<float, Layout> DenseT;
    CheckDense<FloatTree, DenseT> checkDense;
    const float radius = 10.0f, tolerance = 0.00001f;
    const Vec3f center(0.0f);
    // decrease the voxelSize to test larger grids
#ifdef BENCHMARK_TEST
    const float voxelSize = 0.05f, width = 5.0f;
#else
    const float voxelSize = 0.5f, width = 5.0f;
#endif

    // Create a VDB containing a level set of a sphere
    FloatGrid::Ptr grid =
        tools::createLevelSetSphere<FloatGrid>(radius, center, voxelSize, width);
    FloatTree& tree0 = grid->tree();

    // Create an empty dense grid
    DenseT dense(grid->evalActiveVoxelBoundingBox());
#ifdef BENCHMARK_TEST
    std::cerr << "\nBBox = " << grid->evalActiveVoxelBoundingBox() << std::endl;
#endif

    {//check Dense::fill
        dense.fill(voxelSize);
#ifndef BENCHMARK_TEST
        checkDense.check(FloatTree(voxelSize), dense);
#endif
    }

    {// parallel convert to dense
#ifdef BENCHMARK_TEST
        util::CpuTimer ts;
        ts.start("CopyToDense");
#endif
        tools::copyToDense(*grid, dense);
#ifdef BENCHMARK_TEST
        ts.stop();
#else
        checkDense.check(tree0, dense);
#endif
    }

    {// Parallel create from dense
#ifdef BENCHMARK_TEST
        util::CpuTimer ts;
        ts.start("CopyFromDense");
#endif
        FloatTree tree1(tree0.background());
        tools::copyFromDense(dense, tree1, tolerance);
#ifdef BENCHMARK_TEST
        ts.stop();
#else
        checkDense.check(tree1, dense);
#endif
    }
}

template <openvdb::tools::MemoryLayout Layout>
void
TestDense::testCopyBool()
{
    using namespace openvdb;

    //std::cerr << "\nTesting testCopyBool with "
    //          << (Layout == tools::LayoutXYZ ? "XYZ" : "ZYX") << " memory layout"
    //          << std::endl;

    const Coord bmin(-1), bmax(8);
    const CoordBBox bbox(bmin, bmax);

    BoolGrid::Ptr grid = createGrid<BoolGrid>(false);
    BoolGrid::ConstAccessor acc = grid->getConstAccessor();

    typedef openvdb::tools::Dense<bool, Layout> DenseT;
    DenseT dense(bbox);
    dense.fill(false);

    // Start with sparse and dense grids both filled with false.
    Coord xyz;
    int &x = xyz[0], &y = xyz[1], &z = xyz[2];
    for (x = bmin.x(); x <= bmax.x(); ++x) {
        for (y = bmin.y(); y <= bmax.y(); ++y) {
            for (z = bmin.z(); z <= bmax.z(); ++z) {
                EXPECT_EQ(false, dense.getValue(xyz));
                EXPECT_EQ(false, acc.getValue(xyz));
            }
        }
    }

    // Fill the dense grid with true.
    dense.fill(true);
    // Copy the contents of the dense grid to the sparse grid.
    tools::copyFromDense(dense, *grid, /*tolerance=*/false);

    // Verify that both sparse and dense grids are now filled with true.
    for (x = bmin.x(); x <= bmax.x(); ++x) {
        for (y = bmin.y(); y <= bmax.y(); ++y) {
            for (z = bmin.z(); z <= bmax.z(); ++z) {
                EXPECT_EQ(true, dense.getValue(xyz));
                EXPECT_EQ(true, acc.getValue(xyz));
            }
        }
    }

    // Fill the dense grid with false.
    dense.fill(false);
    // Copy the contents (= true) of the sparse grid to the dense grid.
    tools::copyToDense(*grid, dense);

    // Verify that the dense grid is now filled with true.
    for (x = bmin.x(); x <= bmax.x(); ++x) {
        for (y = bmin.y(); y <= bmax.y(); ++y) {
            for (z = bmin.z(); z <= bmax.z(); ++z) {
                EXPECT_EQ(true, dense.getValue(xyz));
            }
        }
    }
}


// Test copying from a dense grid to a sparse grid with various bounding boxes.
template <openvdb::tools::MemoryLayout Layout>
void
TestDense::testCopyFromDenseWithOffset()
{
    using namespace openvdb;

    //std::cerr << "\nTesting testCopyFromDenseWithOffset with "
    //          << (Layout == tools::LayoutXYZ ? "XYZ" : "ZYX") << " memory layout"
    //          << std::endl;

    typedef openvdb::tools::Dense<float, Layout> DenseT;

    const int DIM = 20, COUNT = DIM * DIM * DIM;
    const float FOREGROUND = 99.0f, BACKGROUND = 5000.0f;

    const int OFFSET[] = { 1, -1, 1001, -1001 };
    for (int offsetIdx = 0; offsetIdx < 4; ++offsetIdx) {

        const int offset = OFFSET[offsetIdx];
        const CoordBBox bbox = CoordBBox::createCube(Coord(offset), DIM);

        DenseT dense(bbox, FOREGROUND);
        EXPECT_EQ(bbox, dense.bbox());

        FloatGrid grid(BACKGROUND);
        tools::copyFromDense(dense, grid, /*tolerance=*/0.0);

        const CoordBBox gridBBox = grid.evalActiveVoxelBoundingBox();
        EXPECT_EQ(bbox, gridBBox);
        EXPECT_EQ(COUNT, int(grid.activeVoxelCount()));

        FloatGrid::ConstAccessor acc = grid.getConstAccessor();
        for (int i = gridBBox.min()[0], ie = gridBBox.max()[0]; i < ie; ++i) {
            for (int j = gridBBox.min()[1], je = gridBBox.max()[1]; j < je; ++j) {
                for (int k = gridBBox.min()[2], ke = gridBBox.max()[2]; k < ke; ++k) {
                    const Coord ijk(i, j, k);
                    EXPECT_NEAR(
                        FOREGROUND, acc.getValue(ijk), /*tolerance=*/0.0);
                    EXPECT_TRUE(acc.isValueOn(ijk));
                }
            }
        }
    }
}

template <openvdb::tools::MemoryLayout Layout>
void
TestDense::testDense2Sparse()
{
    // The following test revealed a bug in v2.0.0b2
    using namespace openvdb;

    //std::cerr << "\nTesting testDense2Sparse with "
    //          << (Layout == tools::LayoutXYZ ? "XYZ" : "ZYX") << " memory layout"
    //          << std::endl;

    typedef tools::Dense<float, Layout> DenseT;

    // Test Domain Resolution
    Int32 sizeX = 8, sizeY = 8, sizeZ = 9;

    // Define a dense grid
    DenseT dense(Coord(sizeX, sizeY, sizeZ));
    const CoordBBox bboxD = dense.bbox();
    // std::cerr <<  "\nDense bbox" << bboxD << std::endl;

    // Verify that the CoordBBox is truely used as [inclusive, inclusive]
    EXPECT_TRUE(int(dense.valueCount()) == int(sizeX * sizeY * sizeZ));

    // Fill the dense grid with constant value 1.
    dense.fill(1.0f);

    // Create two empty float grids
    FloatGrid::Ptr gridS = FloatGrid::create(0.0f /*background*/);
    FloatGrid::Ptr gridP = FloatGrid::create(0.0f /*background*/);

    // Convert in serial and parallel modes
    tools::copyFromDense(dense, *gridS, /*tolerance*/0.0f, /*serial = */ true);
    tools::copyFromDense(dense, *gridP, /*tolerance*/0.0f, /*serial = */ false);

    float minS, maxS;
    float minP, maxP;

    math::MinMax<float> extrema = tools::minMax(gridS->tree());
    minS = extrema.min();
    maxS = extrema.max();

    extrema = tools::minMax(gridP->tree());
    minP = extrema.min();
    maxP = extrema.max();

    const float tolerance = 0.0001f;

    EXPECT_NEAR(minS, minP, tolerance);
    EXPECT_NEAR(maxS, maxP, tolerance);
    EXPECT_EQ(gridP->activeVoxelCount(), Index64(sizeX * sizeY * sizeZ));

    const FloatTree& treeS = gridS->tree();
    const FloatTree& treeP = gridP->tree();

    // Values in Test Domain are correct
    for (Coord ijk(bboxD.min()); ijk[0] <= bboxD.max()[0]; ++ijk[0]) {
        for (ijk[1] = bboxD.min()[1]; ijk[1] <= bboxD.max()[1]; ++ijk[1]) {
            for (ijk[2] = bboxD.min()[2]; ijk[2] <= bboxD.max()[2]; ++ijk[2]) {

                const float expected = bboxD.isInside(ijk) ? 1.f : 0.f;
                EXPECT_NEAR(expected, 1.f, tolerance);

                const float& vS = treeS.getValue(ijk);
                const float& vP = treeP.getValue(ijk);

                EXPECT_NEAR(expected, vS, tolerance);
                EXPECT_NEAR(expected, vP, tolerance);
            }
        }
    }

    CoordBBox bboxP = gridP->evalActiveVoxelBoundingBox();
    const Index64 voxelCountP = gridP->activeVoxelCount();
    //std::cerr <<  "\nParallel: bbox=" << bboxP << " voxels=" << voxelCountP << std::endl;
    EXPECT_TRUE( bboxP == bboxD );
    EXPECT_EQ( dense.valueCount(), voxelCountP);

    CoordBBox bboxS = gridS->evalActiveVoxelBoundingBox();
    const Index64 voxelCountS = gridS->activeVoxelCount();
    //std::cerr <<  "\nSerial: bbox=" << bboxS << " voxels=" << voxelCountS << std::endl;
    EXPECT_TRUE( bboxS == bboxD );
    EXPECT_EQ( dense.valueCount(), voxelCountS);

    // Topology
    EXPECT_TRUE( bboxS.isInside(bboxS) );
    EXPECT_TRUE( bboxP.isInside(bboxP) );
    EXPECT_TRUE( bboxS.isInside(bboxP) );
    EXPECT_TRUE( bboxP.isInside(bboxS) );

    /// Check that the two grids agree
    for (Coord ijk(bboxS.min()); ijk[0] <= bboxS.max()[0]; ++ijk[0]) {
        for (ijk[1] = bboxS.min()[1]; ijk[1] <= bboxS.max()[1]; ++ijk[1]) {
            for (ijk[2] = bboxS.min()[2]; ijk[2] <= bboxS.max()[2]; ++ijk[2]) {

                const float& vS = treeS.getValue(ijk);
                const float& vP = treeP.getValue(ijk);

                EXPECT_NEAR(vS, vP, tolerance);

                // the value we should get based on the original domain
                const float expected = bboxD.isInside(ijk) ? 1.f : 0.f;

                EXPECT_NEAR(expected, vP, tolerance);
                EXPECT_NEAR(expected, vS, tolerance);
            }
        }
    }


    // Verify the tree topology matches.

    EXPECT_EQ(gridP->activeVoxelCount(), gridS->activeVoxelCount());
    EXPECT_TRUE(gridP->evalActiveVoxelBoundingBox() == gridS->evalActiveVoxelBoundingBox());
    EXPECT_TRUE(treeP.hasSameTopology(treeS) );

}

template <openvdb::tools::MemoryLayout Layout>
void
TestDense::testDense2Sparse2()
{
    // The following tests copying a dense grid into a VDB tree with
    // existing values outside the bbox of the dense grid.

    using namespace openvdb;

    //std::cerr << "\nTesting testDense2Sparse2 with "
    //          << (Layout == tools::LayoutXYZ ? "XYZ" : "ZYX") << " memory layout"
    //          << std::endl;

    typedef tools::Dense<float, Layout> DenseT;

    // Test Domain Resolution
    const int sizeX = 8, sizeY = 8, sizeZ = 9;
    const Coord magicVoxel(sizeX, sizeY, sizeZ);

    // Define a dense grid
    DenseT dense(Coord(sizeX, sizeY, sizeZ));
    const CoordBBox bboxD = dense.bbox();
    //std::cerr <<  "\nDense bbox" << bboxD << std::endl;

    // Verify that the CoordBBox is truely used as [inclusive, inclusive]
    EXPECT_EQ(sizeX * sizeY * sizeZ, static_cast<int>(dense.valueCount()));

    // Fill the dense grid with constant value 1.
    dense.fill(1.0f);

    // Create two empty float grids
    FloatGrid::Ptr gridS = FloatGrid::create(0.0f /*background*/);
    FloatGrid::Ptr gridP = FloatGrid::create(0.0f /*background*/);
    gridS->tree().setValue(magicVoxel, 5.0f);
    gridP->tree().setValue(magicVoxel, 5.0f);

    // Convert in serial and parallel modes
    tools::copyFromDense(dense, *gridS, /*tolerance*/0.0f, /*serial = */ true);
    tools::copyFromDense(dense, *gridP, /*tolerance*/0.0f, /*serial = */ false);

    float minS, maxS;
    float minP, maxP;

    math::MinMax<float> extrema = tools::minMax(gridS->tree());
    minS = extrema.min();
    maxS = extrema.max();

    extrema = tools::minMax(gridP->tree());
    minP = extrema.min();
    maxP = extrema.max();

    const float tolerance = 0.0001f;

    EXPECT_NEAR(1.0f, minP, tolerance);
    EXPECT_NEAR(1.0f, minS, tolerance);
    EXPECT_NEAR(5.0f, maxP, tolerance);
    EXPECT_NEAR(5.0f, maxS, tolerance);
    EXPECT_EQ(gridP->activeVoxelCount(), Index64(1 + sizeX * sizeY * sizeZ));

    const FloatTree& treeS = gridS->tree();
    const FloatTree& treeP = gridP->tree();

    // Values in Test Domain are correct
    for (Coord ijk(bboxD.min()); ijk[0] <= bboxD.max()[0]; ++ijk[0]) {
        for (ijk[1] = bboxD.min()[1]; ijk[1] <= bboxD.max()[1]; ++ijk[1]) {
            for (ijk[2] = bboxD.min()[2]; ijk[2] <= bboxD.max()[2]; ++ijk[2]) {

                const float expected = bboxD.isInside(ijk) ? 1.0f : 0.0f;
                EXPECT_NEAR(expected, 1.0f, tolerance);

                const float& vS = treeS.getValue(ijk);
                const float& vP = treeP.getValue(ijk);

                EXPECT_NEAR(expected, vS, tolerance);
                EXPECT_NEAR(expected, vP, tolerance);
            }
        }
    }

    CoordBBox bboxP = gridP->evalActiveVoxelBoundingBox();
    const Index64 voxelCountP = gridP->activeVoxelCount();
    //std::cerr <<  "\nParallel: bbox=" << bboxP << " voxels=" << voxelCountP << std::endl;
    EXPECT_TRUE( bboxP != bboxD );
    EXPECT_TRUE( bboxP == CoordBBox(Coord(0,0,0), magicVoxel) );
    EXPECT_EQ( dense.valueCount()+1, voxelCountP);

    CoordBBox bboxS = gridS->evalActiveVoxelBoundingBox();
    const Index64 voxelCountS = gridS->activeVoxelCount();
    //std::cerr <<  "\nSerial: bbox=" << bboxS << " voxels=" << voxelCountS << std::endl;
    EXPECT_TRUE( bboxS != bboxD );
    EXPECT_TRUE( bboxS == CoordBBox(Coord(0,0,0), magicVoxel) );
    EXPECT_EQ( dense.valueCount()+1, voxelCountS);

    // Topology
    EXPECT_TRUE( bboxS.isInside(bboxS) );
    EXPECT_TRUE( bboxP.isInside(bboxP) );
    EXPECT_TRUE( bboxS.isInside(bboxP) );
    EXPECT_TRUE( bboxP.isInside(bboxS) );

    /// Check that the two grids agree
    for (Coord ijk(bboxS.min()); ijk[0] <= bboxS.max()[0]; ++ijk[0]) {
        for (ijk[1] = bboxS.min()[1]; ijk[1] <= bboxS.max()[1]; ++ijk[1]) {
            for (ijk[2] = bboxS.min()[2]; ijk[2] <= bboxS.max()[2]; ++ijk[2]) {

                const float& vS = treeS.getValue(ijk);
                const float& vP = treeP.getValue(ijk);

                EXPECT_NEAR(vS, vP, tolerance);

                // the value we should get based on the original domain
                const float expected = bboxD.isInside(ijk) ? 1.0f
                    : ijk == magicVoxel ? 5.0f : 0.0f;

                EXPECT_NEAR(expected, vP, tolerance);
                EXPECT_NEAR(expected, vS, tolerance);
            }
        }
    }

    // Verify the tree topology matches.

    EXPECT_EQ(gridP->activeVoxelCount(), gridS->activeVoxelCount());
    EXPECT_TRUE(gridP->evalActiveVoxelBoundingBox() == gridS->evalActiveVoxelBoundingBox());
    EXPECT_TRUE(treeP.hasSameTopology(treeS) );

}

template <openvdb::tools::MemoryLayout Layout>
void
TestDense::testInvalidBBox()
{
    using namespace openvdb;

    //std::cerr << "\nTesting testInvalidBBox with "
    //          << (Layout == tools::LayoutXYZ ? "XYZ" : "ZYX") << " memory layout"
    //          << std::endl;

    typedef tools::Dense<float, Layout> DenseT;
    const CoordBBox badBBox(Coord(1, 1, 1), Coord(-1, 2, 2));

    EXPECT_TRUE(badBBox.empty());
    EXPECT_THROW(DenseT dense(badBBox), ValueError);
}

template <openvdb::tools::MemoryLayout Layout>
void
TestDense::testDense2Sparse2Dense()
{
    using namespace openvdb;

    //std::cerr << "\nTesting testDense2Sparse2Dense with "
    //          << (Layout == tools::LayoutXYZ ? "XYZ" : "ZYX") << " memory layout"
    //          << std::endl;

    typedef tools::Dense<float, Layout> DenseT;

    const CoordBBox bboxBig(Coord(-12, 7, -32), Coord(12, 14, -15));
    const CoordBBox bboxSmall(Coord(-10, 8, -31), Coord(10, 12, -20));


    // A larger bbox
    CoordBBox bboxBigger = bboxBig;
    bboxBigger.expand(Coord(10));


    // Small is in big
    EXPECT_TRUE(bboxBig.isInside(bboxSmall));

    // Big is in Bigger
    EXPECT_TRUE(bboxBigger.isInside(bboxBig));

    // Construct a small dense grid
    DenseT denseSmall(bboxSmall, 0.f);
    {
        // insert non-const values
        const int n = static_cast<int>(denseSmall.valueCount());
        float* d = denseSmall.data();
        for (int i = 0; i < n; ++i) { d[i] = static_cast<float>(i); }
    }
    // Construct large dense grid
    DenseT denseBig(bboxBig, 0.f);
    {
        // insert non-const values
        const int n = static_cast<int>(denseBig.valueCount());
        float* d = denseBig.data();
        for (int i = 0; i < n; ++i) { d[i] = static_cast<float>(i); }
    }

    // Make a sparse grid to copy this data into
    FloatGrid::Ptr grid = FloatGrid::create(3.3f /*background*/);
    tools::copyFromDense(denseBig, *grid, /*tolerance*/0.0f, /*serial = */ true);
    tools::copyFromDense(denseSmall, *grid, /*tolerance*/0.0f, /*serial = */ false);

    const FloatTree& tree = grid->tree();
    //
    EXPECT_EQ(bboxBig.volume(), grid->activeVoxelCount());

    // iterate over the Bigger
    for (Coord ijk(bboxBigger.min()); ijk[0] <= bboxBigger.max()[0]; ++ijk[0]) {
        for (ijk[1] = bboxBigger.min()[1]; ijk[1] <= bboxBigger.max()[1]; ++ijk[1]) {
            for (ijk[2] = bboxBigger.min()[2]; ijk[2] <= bboxBigger.max()[2]; ++ijk[2]) {

                float expected = 3.3f;
                if (bboxSmall.isInside(ijk)) {
                    expected = denseSmall.getValue(ijk);
                } else if (bboxBig.isInside(ijk)) {
                    expected = denseBig.getValue(ijk);
                }

                const float& value = tree.getValue(ijk);

                EXPECT_NEAR(expected, value, 0.0001);

            }
        }
    }

    // Convert to Dense in small bbox
    {
        DenseT denseSmall2(bboxSmall);
        tools::copyToDense(*grid, denseSmall2, true /* serial */);

        // iterate over the Bigger
        for (Coord ijk(bboxSmall.min()); ijk[0] <= bboxSmall.max()[0]; ++ijk[0]) {
            for (ijk[1] = bboxSmall.min()[1]; ijk[1] <= bboxSmall.max()[1]; ++ijk[1]) {
                for (ijk[2] = bboxSmall.min()[2]; ijk[2] <= bboxSmall.max()[2]; ++ijk[2]) {

                    const float& expected = denseSmall.getValue(ijk);
                    const float& value = denseSmall2.getValue(ijk);
                    EXPECT_NEAR(expected, value, 0.0001);
                }
            }
        }
    }
    // Convert to Dense in large bbox
    {
        DenseT denseBig2(bboxBig);

        tools::copyToDense(*grid, denseBig2, false /* serial */);
         // iterate over the Bigger
        for (Coord ijk(bboxBig.min()); ijk[0] <= bboxBig.max()[0]; ++ijk[0]) {
            for (ijk[1] = bboxBig.min()[1]; ijk[1] <= bboxBig.max()[1]; ++ijk[1]) {
                for (ijk[2] = bboxBig.min()[2]; ijk[2] <= bboxBig.max()[2]; ++ijk[2]) {

                    float expected = -1.f; // should never be this
                    if (bboxSmall.isInside(ijk)) {
                        expected = denseSmall.getValue(ijk);
                    } else if (bboxBig.isInside(ijk)) {
                        expected = denseBig.getValue(ijk);
                    }
                    const float& value = denseBig2.getValue(ijk);
                    EXPECT_NEAR(expected, value, 0.0001);
                }
            }
        }
    }
}

TEST_F(TestDense, testCopyZYX) { this->testCopy<openvdb::tools::LayoutZYX>(); }
TEST_F(TestDense, testCopyXYZ) { this->testCopy<openvdb::tools::LayoutXYZ>(); }
TEST_F(TestDense, testCopyBoolZYX) { this->testCopyBool<openvdb::tools::LayoutZYX>(); }
TEST_F(TestDense, testCopyBoolXYZ) { this->testCopyBool<openvdb::tools::LayoutXYZ>(); }
TEST_F(TestDense, testCopyFromDenseWithOffsetZYX) { this->testCopyFromDenseWithOffset<openvdb::tools::LayoutZYX>(); }
TEST_F(TestDense, testCopyFromDenseWithOffsetXYZ) { this->testCopyFromDenseWithOffset<openvdb::tools::LayoutXYZ>(); }
TEST_F(TestDense, testDense2SparseZYX) { this->testDense2Sparse<openvdb::tools::LayoutZYX>(); }
TEST_F(TestDense, testDense2SparseXYZ) { this->testDense2Sparse<openvdb::tools::LayoutXYZ>(); }
TEST_F(TestDense, testDense2Sparse2ZYX) { this->testDense2Sparse2<openvdb::tools::LayoutZYX>(); }
TEST_F(TestDense, testDense2Sparse2XYZ) { this->testDense2Sparse2<openvdb::tools::LayoutXYZ>(); }
TEST_F(TestDense, testInvalidBBoxZYX) { this->testInvalidBBox<openvdb::tools::LayoutZYX>(); }
TEST_F(TestDense, testInvalidBBoxXYZ) { this->testInvalidBBox<openvdb::tools::LayoutXYZ>(); }
TEST_F(TestDense, testDense2Sparse2DenseZYX) { this->testDense2Sparse2Dense<openvdb::tools::LayoutZYX>(); }
TEST_F(TestDense, testDense2Sparse2DenseXYZ) { this->testDense2Sparse2Dense<openvdb::tools::LayoutXYZ>(); }

#undef BENCHMARK_TEST
