/*=========================================================================

   Library: iMSTK

   Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
   & Imaging in Medicine, Rensselaer Polytechnic Institute.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

=========================================================================*/

#include "gtest/gtest.h"

#include "imstkPbdPointTriangleConstraint.h"

using namespace imstk;

///
/// \brief TODO
///
class imstkPbdPointTriangleConstraintTest : public ::testing::Test
{
protected:
    PbdPointTriangleConstraint m_constraint;
};

///
/// \brief Test that a point below a triangle, and the triangle, meet on y axes
///
TEST_F(imstkPbdPointTriangleConstraintTest, TestConvergence1)
{
    Vec3d a = Vec3d(0.5, 0.0, -0.5);
    Vec3d b = Vec3d(-0.5, 0.0, -0.5);
    Vec3d c = Vec3d(0.0, 0.0, 0.5);

    Vec3d x = (a + b + c) / 3.0;
    x[1] -= 1.0;

     Vec3d zeroVelocity = Vec3d::Zero();
     m_constraint.initConstraint(
        { &x, 1.0, &zeroVelocity },
        { &a, 1.0, &zeroVelocity },
        { &b, 1.0, &zeroVelocity },
        { &c, 1.0, &zeroVelocity },
         1.0, 1.0);
     for (int i = 0; i < 3; i++)
    {
        m_constraint.solvePosition();
     }

     EXPECT_NEAR(x[1], a[1], 0.00000001);
     EXPECT_NEAR(x[1], b[1], 0.00000001);
     EXPECT_NEAR(x[1], c[1], 0.00000001);
}

///
/// \brief Test that a point above a triangle, and the triangle, meet on y axes
///
TEST_F(imstkPbdPointTriangleConstraintTest, TestConvergence2)
{
    Vec3d a = Vec3d(0.5, 0.0, -0.5);
    Vec3d b = Vec3d(-0.5, 0.0, -0.5);
    Vec3d c = Vec3d(0.0, 0.0, 0.5);

    Vec3d x = (a + b + c) / 3.0;
    x[1] += 1.0;

    Vec3d zeroVelocity = Vec3d::Zero();
    m_constraint.initConstraint(
        { &x, 1.0, &zeroVelocity },
        { &a, 1.0, &zeroVelocity },
        { &b, 1.0, &zeroVelocity },
        { &c, 1.0, &zeroVelocity },
        1.0, 1.0);
    for (int i = 0; i < 3; i++)
    {
        m_constraint.solvePosition();
    }

    EXPECT_NEAR(x[1], a[1], 0.00000001);
    EXPECT_NEAR(x[1], b[1], 0.00000001);
    EXPECT_NEAR(x[1], c[1], 0.00000001);
}

///
/// \brief Test that a point not within the triangle does not move at all
///
TEST_F(imstkPbdPointTriangleConstraintTest, TestNoConvergence1)
{
    Vec3d       a     = Vec3d(0.5, 0.0, -0.5);
    const Vec3d aInit = a;
    Vec3d       b     = Vec3d(-0.5, 0.0, -0.5);
    const Vec3d bInit = b;
    Vec3d       c     = Vec3d(0.0, 0.0, 0.5);
    const Vec3d cInit = c;

    // Test all 3 sides of the triangle (u,v,w)
    Vec3d testPts[3] =
    {
        Vec3d(3.0, -1.0, 0.0),
        Vec3d(-3.0, -1.0, 0.0),
        Vec3d(0.0, -1.0, -3.0)
    };
    const Vec3d initTestPts[3] = { testPts[0], testPts[1], testPts[2] };

    for (int i = 0; i < 3; i++)
    {
        a = aInit;
        b = bInit;
        c = cInit;

        Vec3d zeroVelocity = Vec3d::Zero();
        m_constraint.initConstraint(
            { &testPts[i], 1.0, &zeroVelocity },
            { &a, 1.0, &zeroVelocity },
            { &b, 1.0, &zeroVelocity },
            { &c, 1.0, &zeroVelocity },
            1.0, 1.0);
        for (int j = 0; j < 3; j++)
        {
            m_constraint.solvePosition();
        }

        // Test they haven't moved
        EXPECT_EQ(initTestPts[i][0], testPts[i][0]) << "i: " << i;
        EXPECT_EQ(initTestPts[i][1], testPts[i][1]) << "i: " << i;
        EXPECT_EQ(initTestPts[i][2], testPts[i][2]) << "i: " << i;

        EXPECT_EQ(aInit[0], a[0]);
        EXPECT_EQ(aInit[1], a[1]);
        EXPECT_EQ(aInit[2], a[2]);

        EXPECT_EQ(bInit[0], b[0]);
        EXPECT_EQ(bInit[1], b[1]);
        EXPECT_EQ(bInit[2], b[2]);

        EXPECT_EQ(cInit[0], c[0]);
        EXPECT_EQ(cInit[1], c[1]);
        EXPECT_EQ(cInit[2], c[2]);
    }
}

///
/// \brief TODO
///
int
imstkPbdPointTriangleConstraintTest(int argc, char* argv[])
{
    // Init Google Test & Mock
    ::testing::InitGoogleTest(&argc, argv);

    // Run tests with gtest
    return RUN_ALL_TESTS();
}