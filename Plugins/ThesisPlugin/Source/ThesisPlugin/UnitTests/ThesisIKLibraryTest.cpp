#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "ThesisIKLibrary.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FThesisIKLibraryTest,
    "ThesisPlugin.IK.TesterIsGood?",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FThesisIKLibraryTest::RunTest(const FString& Parameters)
{
    TestEqual("base", 0, 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FThesisFootRotationTest,
    "ThesisPlugin.IK.SetFootIKC.FootRotationCalculation",
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
        TestTrue("FrontRoll = 0", FMath::IsNearlyZero(result.Roll, 0.01f));
    }

    {
        FVector slopedNormal = FVector(0, 1, 1).GetSafeNormal();
        FRotator result = UThesisIKLibrary::CalculateFootRotation(slopedNormal);
        TestTrue("RightRoll ≈ 45", FMath::IsNearlyEqual(result.Roll, 45.0f, 0.1f));
        TestTrue("RightPitch = 0", FMath::IsNearlyZero(result.Pitch, 0.01f));
    }

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FSetFootIKC_NullCheck,
    "ThesisPlugin.IK.SetFootIKC.NullCheck",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FSetFootIKC_NullCheck::RunTest(const FString& Parameters)
{
    FVector offset = FVector(1, 1, 1);
    FVector target = FVector(1, 1, 1);
    FRotator rotator = FRotator(1, 1, 1);

    UThesisIKLibrary::SetFootIKC(
        nullptr,
        nullptr,
        nullptr,
        false,
        FName("Enable_FootIK_L"),
        FName("ik_foot_l"),
        FName("root"),
        offset,
        target,
        rotator
    );

    TestEqual("null offset", offset, FVector(1, 1, 1));
    TestEqual("null target", target, FVector(1, 1, 1));
    TestEqual("null rotator", rotator, FRotator(1, 1, 1));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FSetFootIKC_CurveZero,
    "ThesisPlugin.IK.SetFootIKC.EarlyReturn_NoCharacter",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FSetFootIKC_CurveZero::RunTest(const FString& Parameters)
{
    USkeletalMeshComponent* mesh = NewObject<USkeletalMeshComponent>();
    mesh->SetAnimInstanceClass(UThesisAnimInstance::StaticClass());
    mesh->RegisterComponent();
    mesh->InitAnim(true);
    UThesisAnimInstance* animInst = Cast<UThesisAnimInstance>(mesh->GetAnimInstance());

    FVector offset = FVector(5, 5, 5);
    FVector target = FVector(5, 5, 5);
    FRotator rotator = FRotator(5, 5, 5);

    UThesisIKLibrary::SetFootIKC(
        mesh,
        nullptr,
        animInst,
        false,
        FName("Enable_FootIK_L"),
        FName("ik_foot_l"),
        FName("root"),
        offset,
        target,
        rotator
    );

    TestEqual("no character → no change", offset, FVector(5, 5, 5));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFootTargetTest,
    "ThesisPlugin.IK.SetFootIKC.CalculateFootTarget.Basic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FFootTargetTest::RunTest(const FString& Parameters)
{
    FVector impactPoint(0, 0, 0);
    FVector normal(0, 0, 1);
    FVector efl(0, 0, 10);
    float height = 10;

    FVector result = UThesisIKLibrary::CalculateFootTarget(
        impactPoint,
        normal,
        efl,
        height
    );

    TestEqual("Flat ground offset", result, FVector(0, 0, -10));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFootRotationZeroNormalTest,
    "ThesisPlugin.IK.SetFootIKC.CalculateFootRotation.ZeroNormal",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FFootRotationZeroNormalTest::RunTest(const FString& Parameters)
{
    FRotator result = UThesisIKLibrary::CalculateFootRotation(FVector(0, 0, 0));

    TestTrue("No NaN", !result.ContainsNaN());

    return true;
}