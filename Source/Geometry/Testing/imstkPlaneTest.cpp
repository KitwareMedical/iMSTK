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

#include "imstkPlane.h"

#include <gtest/gtest.h>

using namespace imstk;

TEST(imstkPlaneTest, SetGetWidth)
{
    Plane plane;
    plane.setWidth(2);
    EXPECT_DOUBLE_EQ(2, plane.getWidth());

    plane.setWidth(0.003);
    EXPECT_DOUBLE_EQ(0.003, plane.getWidth());

    plane.setWidth(400000000);
    EXPECT_DOUBLE_EQ(400000000.0, plane.getWidth());

    plane.setWidth(0.0);
    EXPECT_DOUBLE_EQ(0.0, plane.getWidth());

    plane.setWidth(-5.0);
    EXPECT_LT(-5.0, plane.getWidth());
}

TEST(imstkPlaneTest, SetGetNormal)
{
    Plane plane;
    Vec3d n1 = Vec3d(0.2, -0.3, 0.9);
    Vec3d n2 = Vec3d(0.003, -0.001, 0.002);
    Vec3d n3 = Vec3d(400000000, -500000000, 600000000);

    plane.setNormal(n1);
    EXPECT_TRUE(plane.getNormal().isApprox(n1.normalized()));

    plane.setNormal(n2);
    EXPECT_TRUE(plane.getNormal().isApprox(n2.normalized()));

    plane.setNormal(n3);
    EXPECT_TRUE(plane.getNormal().isApprox(n3.normalized()));

    plane.setNormal(Vec3d::Zero());
    EXPECT_FALSE(plane.getNormal().isApprox(Vec3d::Zero()));
}

TEST(imstkPlaneTest, GetVolume)
{
    Plane plane;
    EXPECT_DOUBLE_EQ(0, plane.getVolume());
}

TEST(imstkPlaneTest, GetFunctionValue)
{
    Plane plane;
    plane.setNormal(Vec3d(0., 1., 0.));
    plane.updatePostTransformData();

    EXPECT_DOUBLE_EQ(0., plane.getFunctionValue(Vec3d(0., 0., 0.)));
    EXPECT_DOUBLE_EQ(0., plane.getFunctionValue(Vec3d(0.5, 0., 0.)));
    EXPECT_DOUBLE_EQ(1., plane.getFunctionValue(Vec3d(1., 1., 1.)));
    EXPECT_DOUBLE_EQ(-10., plane.getFunctionValue(Vec3d(0., -10., 0.)));

    plane.setPosition(Vec3d(1., 1., 1.));
    plane.setNormal(Vec3d(1., 1., 1.));
    plane.updatePostTransformData();

    EXPECT_DOUBLE_EQ(-std::sqrt(3.0), plane.getFunctionValue(Vec3d(0., 0., 0.)));
    EXPECT_DOUBLE_EQ(0.0, plane.getFunctionValue(Vec3d(1., 1., 1.)));
    EXPECT_DOUBLE_EQ(-2.0 / std::sqrt(3.0), plane.getFunctionValue(Vec3d(1., 0., 0.)));
    EXPECT_DOUBLE_EQ(-13.0 / std::sqrt(3.0), plane.getFunctionValue(Vec3d(0., -10., 0.)));
}
