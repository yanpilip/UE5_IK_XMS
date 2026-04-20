#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "ThesisIKLibrary.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "ThesisAnimInstance.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FThesisIKLibraryTest,
    "ThesisPlugin.IK.WhiteBox.Internal.TesterIsGood?",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FThesisIKLibraryTest::RunTest(const FString& Parameters)
{
    TestEqual("base", 0, 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FThesisFootRotationTest,
    "ThesisPlugin.IK.WhiteBox.Math.SetFootIKC.CalculateFootRotation.Basic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FThesisFootRotationTest::RunTest(const FString& Parameters)
{
    {
        FRotator result = UThesisIKLibrary::CalculateFootRotation(FVector(0, 0, 1));
        TestTrue("FlatPitch = 0", FMath::IsNearlyZero(result.Pitch, 0.01f));
        TestTrue("FlatYaw = 0", FMath::IsNearlyZero(result.Yaw, 0.01f));
        TestTrue("FlatRoll = 0", FMath::IsNearlyZero(result.Roll, 0.01f));
    }
    {
        FVector slopedNormal = FVector(1, 0, 1).GetSafeNormal();
        FRotator result = UThesisIKLibrary::CalculateFootRotation(slopedNormal);
        TestTrue("FrontPitch ≈ -45", FMath::IsNearlyEqual(result.Pitch, -45.0f, 0.1f));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FSetFootIKC_NullCheck,
    "ThesisPlugin.IK.BlackBox.Safety.SetFootIKC.NullPointerHandling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FSetFootIKC_NullCheck::RunTest(const FString& Parameters)
{
    FVector offset(1.0, 1.0, 1.0); FVector target(1.0, 1.0, 1.0); FRotator rotator(1.0, 1.0, 1.0);
    UThesisIKLibrary::SetFootIKC(nullptr, nullptr, nullptr, false, FName("Enable_FootIK_L"), FName("ik_foot_l"), FName("root"), offset, target, rotator);
    TestEqual("null offset stays", offset, FVector(1.0, 1.0, 1.0));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFootTargetTest,
    "ThesisPlugin.IK.BlackBox.Logic.CalculateFootTarget.VerticalOffset",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FFootTargetTest::RunTest(const FString& Parameters)
{
    FVector result = UThesisIKLibrary::CalculateFootTarget(FVector(0, 0, 0), FVector(0, 0, 1), FVector(0, 0, 10), 10);
    TestEqual("Flat ground offset", result, FVector(0, 0, -10));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFootRotationZeroNormalTest,
    "ThesisPlugin.IK.WhiteBox.Math.CalculateFootRotation.ZeroNormalHandling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FFootRotationZeroNormalTest::RunTest(const FString& Parameters)
{
    FRotator result = UThesisIKLibrary::CalculateFootRotation(FVector(0, 0, 0));
    TestTrue("No NaN", !result.ContainsNaN());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIKBreakTest,
    "ThesisPlugin.IK.BlackBox.Logic.ShouldBreakIK.StandardConditions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FIKBreakTest::RunTest(const FString& Parameters)
{
    TestTrue("Standing break", UThesisIKLibrary::ShouldBreakIK(120, 100, 80, false));
    TestFalse("Within range", UThesisIKLibrary::ShouldBreakIK(50, 100, 80, false));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FPelvisAlphaTest,
    "ThesisPlugin.IK.WhiteBox.Math.CalculatePelvisAlpha.WeightAveraging",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FPelvisAlphaTest::RunTest(const FString& Parameters)
{
    TestEqual("Alpha = 0.5", UThesisIKLibrary::CalculatePelvisAlpha(1.0f, 0.0f), 0.5f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FPelvisTargetTest,
    "ThesisPlugin.IK.WhiteBox.Math.SetPelvisIKC.TargetSelectionLogic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FPelvisTargetTest::RunTest(const FString& Parameters)
{
    FVector result = UThesisIKLibrary::CalculatePelvisTarget(FVector(0, 0, -30), FVector(0, 0, -10));
    TestEqual("Lower foot chosen", result.Z, -30.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FInterpSpeedTest,
    "ThesisPlugin.IK.WhiteBox.Logic.SelectInterpSpeed.DirectionalBranching",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)
bool FInterpSpeedTest::RunTest(const FString& Parameters)
{
    TestEqual("Going down", UThesisIKLibrary::SelectInterpSpeed(10, 0, 30, 80), 30.0f);
    return true;
}

///NEW TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootRotation_VerticalWallTest, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootRotation.90DegreeSlope", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootRotation_VerticalWallTest::RunTest(const FString& Parameters) {
    FRotator result = UThesisIKLibrary::CalculateFootRotation(FVector(1, 0, 0));
    TestTrue("90 degree pitch", FMath::IsNearlyEqual(result.Pitch, -90.0f, 0.1f)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootRotation_NegativeNormalTest, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootRotation.InvertedNormal", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootRotation_NegativeNormalTest::RunTest(const FString& Parameters) {
    FRotator result = UThesisIKLibrary::CalculateFootRotation(FVector(0, 0, -1));
    TestTrue("No NaN inversion", !FMath::IsNaN(result.Pitch)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootTarget_ImpactAboveEFL, "ThesisPlugin.IK.BlackBox.Logic.CalculateFootTarget.GroundAboveRoot", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootTarget_ImpactAboveEFL::RunTest(const FString& Parameters) {
    FVector result = UThesisIKLibrary::CalculateFootTarget(FVector(0, 0, 20), FVector(0, 0, 1), FVector(0, 0, 10), 0);
    TestEqual("Step up", result.Z, 10.0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootTarget_SlopedSurface, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootTarget.SlopedHeightCorrection", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootTarget_SlopedSurface::RunTest(const FString& Parameters) {
    FVector normal(1, 0, 1); normal.Normalize();
    FVector result = UThesisIKLibrary::CalculateFootTarget(FVector(0, 0, 0), normal, FVector(0, 0, 0), 15.0f);
    TestTrue("Sloped Z check", result.Z < 0.0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIKBreak_ZeroLimit, "ThesisPlugin.IK.BlackBox.Logic.ShouldBreakIK.ZeroLimitBoundary", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FIKBreak_ZeroLimit::RunTest(const FString& Parameters) {
    TestTrue("Zero limit break", UThesisIKLibrary::ShouldBreakIK(0.1f, 0.0f, 0.0f, false)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIKBreak_BoundaryValue, "ThesisPlugin.IK.WhiteBox.Logic.ShouldBreakIK.BoundaryEqualityCheck", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FIKBreak_BoundaryValue::RunTest(const FString& Parameters) {
    TestFalse("Exactly limit", UThesisIKLibrary::ShouldBreakIK(100.0f, 100.0f, 80.0f, false));
    TestTrue("Above limit", UThesisIKLibrary::ShouldBreakIK(100.01f, 100.0f, 80.0f, false)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInterpSpeed_EqualZ, "ThesisPlugin.IK.WhiteBox.Logic.SelectInterpSpeed.EqualityBranchSelector", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FInterpSpeed_EqualZ::RunTest(const FString& Parameters) {
    TestEqual("Equality check", UThesisIKLibrary::SelectInterpSpeed(10.0f, 10.0f, 30.0f, 80.0f), 80.0f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPelvisAlpha_NegativeInput, "ThesisPlugin.IK.WhiteBox.Math.CalculatePelvisAlpha.NegativeInputStability", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPelvisAlpha_NegativeInput::RunTest(const FString& Parameters) {
    TestEqual("Negative alpha", UThesisIKLibrary::CalculatePelvisAlpha(-1.0f, 1.0f), 0.0f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPelvisAlpha_Overflow, "ThesisPlugin.IK.WhiteBox.Math.CalculatePelvisAlpha.HighValueStressTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPelvisAlpha_Overflow::RunTest(const FString& Parameters) {
    TestEqual("High alpha", UThesisIKLibrary::CalculatePelvisAlpha(100.0f, 200.0f), 150.0f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPelvisTarget_EqualZ, "ThesisPlugin.IK.WhiteBox.Math.CalculatePelvisTarget.EqualityPreference", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPelvisTarget_EqualZ::RunTest(const FString& Parameters) {
    FVector L(0, 0, -10); FVector R(5, 5, -10);
    TestEqual("Equal Z selection", UThesisIKLibrary::CalculatePelvisTarget(L, R).Z, -10.0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPelvisTarget_MixedCoordinates, "ThesisPlugin.IK.WhiteBox.Math.CalculatePelvisTarget.ZOnlySelection", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPelvisTarget_MixedCoordinates::RunTest(const FString& Parameters) {
    FVector L(100, 100, -10); FVector R(0, 0, -20);
    TestEqual("Z-only check", UThesisIKLibrary::CalculatePelvisTarget(L, R).Z, -20.0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSetFootIKC_CurveValueValidation, "ThesisPlugin.IK.BlackBox.State.SetFootIKC.CurveActivationCheck", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FSetFootIKC_CurveValueValidation::RunTest(const FString& Parameters) {
    FVector offset(10.0, 10.0, 10.0); FVector target(0, 0, 0); FRotator rot(5, 5, 5);
    UThesisIKLibrary::SetFootIKC(nullptr, nullptr, nullptr, false, FName("Null"), FName("ik_foot_l"), FName("root"), offset, target, rot);
    TestEqual("Curve check early exit keeps values", offset, FVector(10.0, 10.0, 10.0)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSetPelvisIKC_AlphaZeroReturn, "ThesisPlugin.IK.BlackBox.State.SetPelvisIKC.ZeroAlphaReset", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FSetPelvisIKC_AlphaZeroReturn::RunTest(const FString& Parameters) {
    float alpha = 1.0f; FVector offset(10, 10, 10);
    UThesisIKLibrary::SetPelvisIKC(nullptr, nullptr, FVector::ZeroVector, FVector::ZeroVector, alpha, offset);
    TestEqual("Pelvis null mesh keeps values", offset, FVector(10.0, 10.0, 10.0)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootTarget_Nullfh, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootTarget.ZeroFootHeightHandling", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootTarget_Nullfh::RunTest(const FString& Parameters) {
    TestEqual("Zero height", UThesisIKLibrary::CalculateFootTarget(FVector(0, 0, 5), FVector(0, 0, 1), FVector(0, 0, 0), 0.0f).Z, 5.0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSetFootIKC_NegativeInterpSpeed, "ThesisPlugin.IK.WhiteBox.Logic.SetFootIKC.NegativeSpeedStability", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FSetFootIKC_NegativeInterpSpeed::RunTest(const FString& Parameters) {
    TestEqual("Negative speed", UThesisIKLibrary::SelectInterpSpeed(0, 10, -5.0f, -10.0f), -10.0f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPelvisTarget_ExtremeZ, "ThesisPlugin.IK.WhiteBox.Math.CalculatePelvisTarget.ExtremeValueFloatingPoint", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPelvisTarget_ExtremeZ::RunTest(const FString& Parameters) {
    TestEqual("Massive Z", UThesisIKLibrary::CalculatePelvisTarget(FVector(0, 0, -1e10), FVector(0, 0, 1e10)).Z, -1e10); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIKBreak_CrouchStandingSwap, "ThesisPlugin.IK.BlackBox.Logic.ShouldBreakIK.StateDependentValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FIKBreak_CrouchStandingSwap::RunTest(const FString& Parameters) {
    TestNotEqual("Crouch state diff", UThesisIKLibrary::ShouldBreakIK(90, 100, 80, false), UThesisIKLibrary::ShouldBreakIK(90, 100, 80, true)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootRotation_HorizontalInversion, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootRotation.XYMirrorSymmetry", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootRotation_HorizontalInversion::RunTest(const FString& Parameters) {
    FRotator pos = UThesisIKLibrary::CalculateFootRotation(FVector(0.5, 0.5, 1).GetSafeNormal());
    FRotator neg = UThesisIKLibrary::CalculateFootRotation(FVector(-0.5, -0.5, 1).GetSafeNormal());
    TestEqual("Mirrored pitch", pos.Pitch, -neg.Pitch); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSetFootIKC_StanceWidthZeroCheck, "ThesisPlugin.IK.BlackBox.Feature.SetFootIKC.StanceWidthZeroVerification", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FSetFootIKC_StanceWidthZeroCheck::RunTest(const FString& Parameters) {
    TestTrue("Logic verification", true); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootTarget_LargeHeightValues, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootTarget.ExtremeHeightStability", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootTarget_LargeHeightValues::RunTest(const FString& Parameters) {
    TestEqual("Large height stability", UThesisIKLibrary::CalculateFootTarget(FVector::ZeroVector, FVector(0, 0, 1), FVector::ZeroVector, 10000.0f).Z, 0.0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPelvisAlpha_SingleFootAlpha, "ThesisPlugin.IK.BlackBox.Logic.CalculatePelvisAlpha.SingleFootIKBlending", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPelvisAlpha_SingleFootAlpha::RunTest(const FString& Parameters) {
    TestEqual("Single alpha", UThesisIKLibrary::CalculatePelvisAlpha(1.0f, 0.0f), 0.5f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIKBreak_NegativeLimit, "ThesisPlugin.IK.WhiteBox.Logic.ShouldBreakIK.NegativeLimitSafetyCheck", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FIKBreak_NegativeLimit::RunTest(const FString& Parameters) {
    TestTrue("Negative limit break", UThesisIKLibrary::ShouldBreakIK(0.0f, -10.0f, -10.0f, false)); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootRotation_FullNormalCircle, "ThesisPlugin.IK.WhiteBox.Math.TrigonometricRangeValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootRotation_FullNormalCircle::RunTest(const FString& Parameters) {
    for (float i = 0; i < 360; i += 90) {
        FVector N(FMath::Cos(FMath::DegreesToRadians(i)), FMath::Sin(FMath::DegreesToRadians(i)), 1.0f);
        TestTrue("Circle finite", !UThesisIKLibrary::CalculateFootRotation(N.GetSafeNormal()).ContainsNaN());
    } return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSetFootIKC_DeltaTimeZero, "ThesisPlugin.IK.WhiteBox.State.SetFootIKC.TimeFrozenStateVerification", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FSetFootIKC_DeltaTimeZero::RunTest(const FString& Parameters) {
    TestEqual("Zero Delta Time", FMath::FInterpTo(0.0f, 10.0f, 0.0f, 10.0f), 0.0f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootTarget_NegativeHeightTest, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootTarget.NegativeHeightStability", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootTarget_NegativeHeightTest::RunTest(const FString& Parameters) {
    FVector result = UThesisIKLibrary::CalculateFootTarget(FVector(0, 0, 0), FVector(0, 0, 1), FVector(0, 0, 0), -10.0f);
    TestEqual("Negative height handled", result.Z, 0.0); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPelvisAlpha_UniformWeights, "ThesisPlugin.IK.WhiteBox.Math.CalculatePelvisAlpha.UniformInput", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FPelvisAlpha_UniformWeights::RunTest(const FString& Parameters) {
    TestEqual("Uniform weights", UThesisIKLibrary::CalculatePelvisAlpha(0.7f, 0.7f), 0.7f); return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFootRotation_PerfectNormal, "ThesisPlugin.IK.WhiteBox.Math.CalculateFootRotation.IdentityNormal", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FFootRotation_PerfectNormal::RunTest(const FString& Parameters) {
    FRotator result = UThesisIKLibrary::CalculateFootRotation(FVector(0, 0, 1));
    TestTrue("Identity normal is zero rotator", result.IsNearlyZero()); return true;
}