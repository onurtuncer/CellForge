#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

#include <tesseract/common/eigen_types.h>
#include <tesseract/scene_graph/graph.h>
#include <tesseract/scene_graph/joint.h>
#include <tesseract/scene_graph/link.h>

using namespace tesseract::scene_graph;

// ─── tesseract_common ────────────────────────────────────────────────────────

TEST_CASE("TransformMap stores and retrieves an identity transform", "[tesseract][common]")
{
    tesseract::common::TransformMap transforms;
    transforms["world"] = Eigen::Isometry3d::Identity();

    REQUIRE(transforms.size() == 1);
    REQUIRE(transforms.at("world").isApprox(Eigen::Isometry3d::Identity()));
}

// ─── tesseract_scene_graph ────────────────────────────────────────────────────

TEST_CASE("SceneGraph constructs a two-link chain via addLink(link, joint)", "[tesseract][scene_graph]")
{
    SceneGraph g("smoke_robot");

    // First link added becomes the root automatically
    auto base = std::make_shared<Link>("base_link");
    REQUIRE(g.addLink(*base));

    // Second link added together with the connecting joint
    auto ee = std::make_shared<Link>("tool0");
    Joint j("base_to_tool0");
    j.parent_link_name = "base_link";
    j.child_link_name  = "tool0";
    j.type             = JointType::FIXED;
    REQUIRE(g.addLink(*ee, j));  // addLink clones the joint internally

    REQUIRE(g.getName() == "smoke_robot");
    REQUIRE(g.getRoot() == "base_link");
    REQUIRE(g.getLink("base_link") != nullptr);
    REQUIRE(g.getLink("tool0")     != nullptr);
    REQUIRE(g.getJoint("base_to_tool0") != nullptr);
    REQUIRE(g.getLinks().size()  == 2);
    REQUIRE(g.getJoints().size() == 1);
    REQUIRE(g.isTree());
}

TEST_CASE("SceneGraph rejects duplicate link names", "[tesseract][scene_graph]")
{
    SceneGraph g("dup_test");
    auto link = std::make_shared<Link>("root");
    REQUIRE(g.addLink(*link));
    REQUIRE_FALSE(g.addLink(*link));  // same name, replace_allowed defaults to false
}

TEST_CASE("SceneGraph clone is an independent copy", "[tesseract][scene_graph]")
{
    SceneGraph original("original");
    auto root = std::make_shared<Link>("root");
    original.addLink(*root);

    auto cloned = original.clone();

    REQUIRE(cloned != nullptr);
    REQUIRE(cloned->getName() == "original");
    REQUIRE(cloned->getRoot() == "root");

    // Mutating the clone must not affect the original
    cloned->setName("mutated");
    REQUIRE(original.getName() == "original");
}
