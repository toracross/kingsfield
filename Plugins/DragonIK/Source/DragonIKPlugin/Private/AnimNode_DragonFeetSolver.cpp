/* Copyright (C) Gamasome Interactive LLP, Inc - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Mansoor Pathiyanthra <codehawk64@gmail.com , mansoor@gamasome.com>, July 2018
*/


#include "AnimNode_DragonFeetSolver.h"
#include "DragonIKPlugin.h"
#include "Animation/AnimInstanceProxy.h"
#include "DrawDebugHelpers.h"




#include "AnimationRuntime.h"
#include "AnimationCoreLibrary.h"
#include "Algo/Reverse.h"


DECLARE_CYCLE_STAT(TEXT("DragonFeetSolver Eval"), STAT_DragonFeetSolver_Eval, STATGROUP_Anim);



// Initialize the component pose as well as defining the owning skeleton
void FAnimNode_DragonFeetSolver::Initialize_AnyThread(const FAnimationInitializeContext & Context)
{

	FAnimNode_DragonControlBase::Initialize_AnyThread(Context);
	owning_skel = Context.AnimInstanceProxy->GetSkelMeshComponent();
	//	dragon_bone_data.Start_Spine = FBoneReference(dragon_input_data.Start_Spine);

	OptionalRefPose.Initialize(Context);
}


/*
// Cache the bones . Thats it !!
void FAnimNode_DragonFeetSolver::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	FAnimNode_Base::CacheBones_AnyThread(Context);
	ComponentPose.CacheBones(Context);
	InitializeBoneReferences(Context.AnimInstanceProxy->GetRequiredBones());
}
*/


/*
// Main update function . Do not perform any changed !!
void FAnimNode_DragonFeetSolver::Update_AnyThread(const FAnimationUpdateContext & Context)
{
	ComponentPose.Update(Context);
	ActualAlpha = 0.f;


	if (IsLODEnabled(Context.AnimInstanceProxy))
	{

		GetEvaluateGraphExposedInputs().Execute(Context);
		//EvaluateGraphExposedInputs.Execute(Context);
		// Apply the skeletal control if it's valid



		ActualAlpha = AlphaScaleBias.ApplyTo(Alpha);

		if (FAnimWeight::IsRelevant(ActualAlpha) && IsValidToEvaluate(Context.AnimInstanceProxy->GetSkeleton(), Context.AnimInstanceProxy->GetRequiredBones()))
		{
			UpdateInternal(Context);



		}
	}
}



void FAnimNode_DragonFeetSolver::UpdateInternal(const FAnimationUpdateContext & Context)
{
	FAnimNode_DragonControlBase::UpdateInternal(Context);
}
*/






void FAnimNode_DragonFeetSolver::Make_All_Bones(FCSPose<FCompactPose>& MeshBases)
{




}




// Store the zero pose transform data
void FAnimNode_DragonFeetSolver::GetResetedPoseInfo(FCSPose<FCompactPose>& MeshBases)
{

}


// Store the animated and calculated pose transform data
void FAnimNode_DragonFeetSolver::GetAnimatedPoseInfo(FCSPose<FCompactPose>& MeshBases)
{

	for (int spine_index = 0; spine_index < dragon_input_data.FeetBones.Num(); spine_index++)
	{
		FBoneReference knee_bone_ref = FBoneReference(dragon_input_data.FeetBones[spine_index].Knee_Bone_Name);

		if (automatic_leg_make == true)
		{
			knee_bone_ref = FBoneReference(dragon_input_data.FeetBones[spine_index].Feet_Bone_Name);
		}


		knee_bone_ref.Initialize(*SavedBoneContainer);

		if (knee_bone_ref.IsValidToEvaluate() && knee_Animated_transform_array.Num() > spine_index)
		{
			if (automatic_leg_make == false)
				knee_Animated_transform_array[spine_index] = MeshBases.GetComponentSpaceTransform(knee_bone_ref.CachedCompactPoseIndex);
			else
			{
				knee_Animated_transform_array[spine_index] = MeshBases.GetComponentSpaceTransform((*SavedBoneContainer).GetParentBoneIndex(knee_bone_ref.CachedCompactPoseIndex));

			}
		}

	}

	for (int spine_index = 0; spine_index < spine_hit_pairs.Num(); spine_index++)
	{
		if (spine_Feet_pair.Num() - 1 < spine_index) continue; // @tikwon

		if(feet_Animated_transform_array.Num() > spine_index && spine_Feet_pair.Num() > spine_index)
		for (int feet_index = 0; feet_index < spine_hit_pairs[spine_index].RV_Feet_Hits.Num(); feet_index++)
		{
			if (spine_Feet_pair[spine_index].Associated_Feet.Num() - 1 < feet_index) continue; // @tikwon

			if(feet_Animated_transform_array[spine_index].Num() > feet_index && spine_Feet_pair[spine_index].Associated_Feet.Num() > feet_index)
			feet_Animated_transform_array[spine_index][feet_index] = MeshBases.GetComponentSpaceTransform(spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex);



		}

	}


	/*
	int32 const NumTransforms = combined_indices.Num() - 1;
	OutBoneTransforms.AddUninitialized(NumTransforms);
	for (int i = 0; i < NumTransforms; i++)
	{

		FTransform updated_transform = MeshBases.GetComponentSpaceTransform(combined_indices[i]);
		FRotator delta_diff = (AnimatedBoneTransforms[i].Transform.Rotator() - RestBoneTransforms[i].Transform.Rotator()).GetNormalized();
		updated_transform.SetRotation((updated_transform.Rotator() + delta_diff).GetNormalized().Quaternion());

		FVector delta_pos_diff = AnimatedBoneTransforms[i].Transform.GetLocation() - RestBoneTransforms[i].Transform.GetLocation();
		updated_transform.SetLocation(updated_transform.GetLocation() + delta_pos_diff);
		OutBoneTransforms[i] = (FBoneTransform(combined_indices[i], updated_transform));
	}
	*/
}



void FAnimNode_DragonFeetSolver::Calculate_Feet_Rotation(FComponentSpacePoseContext & Output, TArray<TArray<FTransform>> feet_rotation_array)
{


	for (int spine_index = 0; spine_index < spine_hit_pairs.Num(); spine_index++)
	{
		if (spine_Feet_pair.Num() - 1 < spine_index) continue; // @tikwon
		for (int feet_index = 0; feet_index < spine_hit_pairs[spine_index].RV_Feet_Hits.Num(); feet_index++)
		{
			if (spine_Feet_pair[spine_index].Associated_Feet.Num() - 1 < feet_index || feet_rotation_array[spine_index].Num() - 1 < feet_index) continue; // @tikwon

			FTransform EndBoneCSTransform = Output.Pose.GetComponentSpaceTransform(spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex);


			if (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit)
			{


				if (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit&& atleast_one_hit)
				{






					FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentToWorld(), Output.Pose, EndBoneCSTransform, spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex, EBoneControlSpace::BCS_WorldSpace);


					FTransform const original_rot = feet_rotation_array[spine_index][feet_index];


					FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentToWorld(), Output.Pose, feet_rotation_array[spine_index][feet_index], spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex, EBoneControlSpace::BCS_WorldSpace);


					FVector normal_impact = spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].ImpactNormal;

					normal_impact = UKismetMathLibrary::InverseTransformDirection(owning_skel->GetComponentToWorld(), normal_impact);


					FRotator normal_rot = FRotator(UKismetMathLibrary::DegAtan2(normal_impact.X, normal_impact.Z)*-1, 0, UKismetMathLibrary::DegAtan2(normal_impact.Y, normal_impact.Z) * 1);

					FVector unit_normal_impact = character_direction_vector_CS;

					FRotator unit_normal_rot = FRotator(UKismetMathLibrary::DegAtan2(unit_normal_impact.X, unit_normal_impact.Z)*-1, 0, UKismetMathLibrary::DegAtan2(unit_normal_impact.Y, unit_normal_impact.Z) * 1);




					if (Enable_Pitch == false)
						normal_rot.Pitch = 0;

					if (Enable_Roll == false)
						normal_rot.Roll = 0;


					float feet_limit = FMath::Abs(dragon_input_data.FeetBones[spine_Feet_pair[spine_index].order_index[feet_index]].Feet_Rotation_Limit );

					normal_rot.Pitch = FMath::Clamp<float>(normal_rot.Pitch, -feet_limit, feet_limit);
					normal_rot.Roll = FMath::Clamp<float>(normal_rot.Roll, -feet_limit, feet_limit);
					normal_rot.Yaw = FMath::Clamp<float>(normal_rot.Yaw, -feet_limit, feet_limit);



					normal_rot = normal_rot - unit_normal_rot;


					//FRotator offset_local = spine_Feet_pair[spine_index].feet_rotation_offset[feet_index];

					//FTransform offset_transform = FTransform();
					//offset_transform.SetRotation(offset_local.Quaternion());

					//FAnimationRuntime::ConvertBoneSpaceTransformToCS(owning_skel->GetComponentToWorld(), Output.Pose, offset_transform, spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex, EBoneControlSpace::BCS_BoneSpace);


					//normal_rot = FRotator((offset_local.Quaternion())*normal_rot.Quaternion());





					FTransform TestTransform1 = Output.Pose.GetComponentSpaceTransform(spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex);

					//FQuat Ref_Anim_Diff = TestTransform1.GetRotation()*feet_Animated_transform_array[spine_index][feet_index].GetRotation().Inverse();

					FRotator Ref_Anim_Diff = FRotator(feet_Animated_transform_array[spine_index][feet_index].GetRotation()*TestTransform1.GetRotation().Inverse());
					Ref_Anim_Diff.Pitch = 0;
					Ref_Anim_Diff.Roll = 0;


					//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Ref_Anim_Diff : " + FRotator(Ref_Anim_Diff).ToString());



					FRotator offset_local = dragon_input_data.FeetBones[spine_Feet_pair[spine_index].order_index[feet_index]].Feet_Rotation_Offset;
					if (!Use_OptionalRef_Feet_As_Ref)
						offset_local = FRotator::ZeroRotator;

					const FQuat BoneInput(offset_local);
					TestTransform1.SetRotation(BoneInput*TestTransform1.GetRotation());

					//if (use_ref_rotation==false)
					TestTransform1.SetRotation(Ref_Anim_Diff.Quaternion()*TestTransform1.GetRotation());

					normal_rot = FRotator(normal_rot.Quaternion()*(TestTransform1.Rotator()).Quaternion());


					//normal_rot = UKismetMathLibrary::TransformRotation(owning_skel->GetComponentToWorld(),normal_rot);

					//const FQuat BoneQuat(normal_rot.Quaternion());
					//EndBoneCSTransform.SetRotation((BoneQuat).GetNormalized());

					// Convert back to Component Space.
					FAnimationRuntime::ConvertBoneSpaceTransformToCS(owning_skel->GetComponentToWorld(), Output.Pose, EndBoneCSTransform, spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex, EBoneControlSpace::BCS_WorldSpace);

					FRotator direction_rot = normal_rot;





					//	direction_rot = FRotator((TestTransform1.Rotator()).Quaternion().Inverse() * direction_rot.Quaternion());


					//	direction_rot = FRotator((TestTransform1.Rotator()).Quaternion()*direction_rot.Quaternion());


					if (Ignore_Lerping == false)
						feet_mod_transform_array[spine_index][feet_index].SetRotation(AnimRotSLerp(feet_mod_transform_array[spine_index][feet_index].GetRotation(), direction_rot.Quaternion(), (1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds()))*feet_rotation_speed));
					else
						feet_mod_transform_array[spine_index][feet_index].SetRotation(direction_rot.Quaternion());




					//feet_mod_transform_array[spine_index][feet_index].SetRotation(direction_rot.Quaternion());

					//						EndBoneCSTransform.SetRotation(feet_mod_transform_array[spine_index][feet_index].GetRotation());





				}
				else
				{
					//						feet_mod_transform_array[spine_index][feet_index].SetRotation(UKismetMathLibrary::RInterpTo(feet_mod_transform_array[spine_index][feet_index].Rotator(), EndBoneCSTransform.Rotator(), 1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds()), shift_speed).Quaternion());
				}

			}
		}

	}

}



void FAnimNode_DragonFeetSolver::GetFeetHeights(FComponentSpacePoseContext & Output)
{
	if (owning_skel)
	{


		FTransform RootTraceTransform = Output.Pose.GetComponentSpaceTransform(FCompactPoseBoneIndex(0));

		FeetRootHeights.Empty();

		scale_mode = 1;

		if (owning_skel->GetOwner())
		{
			scale_mode = owning_skel->GetOwner()->GetActorScale().Z;
		}
		//	FeetRootHeights.AddUninitialized(spine_Feet_pair.Num());
		FeetRootHeights.AddDefaulted(spine_Feet_pair.Num());

		for (int i = 0; i < spine_Feet_pair.Num(); i++)
		{
			for (int j = 0; j < spine_Feet_pair[i].Associated_Feet.Num(); j++)
			{
				FTransform bonetraceTransform = Output.Pose.GetComponentSpaceTransform(spine_Feet_pair[i].Associated_Feet[j].GetCompactPoseIndex(*SavedBoneContainer));
				//			float feet_root_height = (bonetraceTransform.GetLocation(), RootTraceTransform.GetLocation()).Size();

				FVector bone_location_ws = owning_skel->GetComponentTransform().TransformPosition(bonetraceTransform.GetLocation());

				FVector zero_ws = owning_skel->GetComponentTransform().TransformPosition(FVector(0, 0, 0));

				float Height_Extra = 0;


				if (i == 0)
				{
					if (j == 0)
					{
						Height_Extra = Foot_01_Height_Offset;
					}
					if (j == 1)
					{
						Height_Extra = Foot_02_Height_Offset;
					}

				}



				if (i == spine_Feet_pair.Num() - 1)
				{
					if (j == 0)
					{
						Height_Extra = Foot_03_Height_Offset;
					}
					if (j == 1)
					{
						Height_Extra = Foot_04_Height_Offset;
					}

				}




				//GEngine->AddOnScreenDebugMessage(-1, 0.02f, FColor::Red, " feet : " + FString::SanitizeFloat(Height_Extra));



				if (Automatic_Foot_Height_Detection)
				{
					if(dragon_input_data.FeetBones.Num() > spine_Feet_pair[i].order_index[j])
					FeetRootHeights[i].Add(FMath::Abs(bonetraceTransform.GetLocation().Z - FVector(0, 0, 0).Z) * owning_skel->GetComponentScale().Z + ((dragon_input_data.FeetBones[spine_Feet_pair[i].order_index[j]].Feet_Heights + Height_Extra) * owning_skel->GetComponentScale().Z));
				}
				else
					FeetRootHeights[i].Add((spine_Feet_pair[i].Feet_Heights[j] + Height_Extra) * owning_skel->GetComponentScale().Z);


				//FeetRootHeights[i].Add(FMath::Abs(10*owning_skel->GetComponentScale().Z));



			}

		}

	}

}

void FAnimNode_DragonFeetSolver::EvaluateComponentSpaceInternal(FComponentSpacePoseContext & Output)
{
	Super::EvaluateComponentSpaceInternal(Output);

}






void FAnimNode_DragonFeetSolver::ConditionalDebugDraw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* PreviewSkelMeshComp) const
{

#if WITH_EDITORONLY_DATA
	if (show_trace_in_game&&PreviewSkelMeshComp && PreviewSkelMeshComp->GetWorld())
	{

		float owner_scale = 1;

		if (PreviewSkelMeshComp->GetOwner())
		{
			owner_scale = PreviewSkelMeshComp->GetComponentToWorld().GetScale3D().Z;
		}



		/*
		for (int k = 0; k < spine_Feet_pair.Num(); k++)
		{

			
			
			for (int i = 0; i < spine_Feet_pair[k].knee_direction_offset.Num(); i++)
			{
				DrawDebugBox(PreviewSkelMeshComp->GetWorld(), feet_knee_offset_array[k][i], FVector(Trace_Radius*owner_scale), FColor::Red, false, 0.1f);

			}
			
		}
		*/


		for (int i = 0; i < TraceStartList.Num(); i++)
		{

			if (trace_type == EIKTrace_Type_Plugin::ENUM_LineTrace_Type || Trace_Radius < 0.2f)
				DrawDebugLine(PreviewSkelMeshComp->GetWorld(), TraceStartList[i], TraceEndList[i], FColor::Red, false, 0.1f);
			else
				if (trace_type == EIKTrace_Type_Plugin::ENUM_SphereTrace_Type)
				{
					FVector Vector_Difference = (TraceStartList[i] - TraceEndList[i]);
					Vector_Difference.X = 0;
					Vector_Difference.Y = 0;



					FVector character_direction_vector = character_direction_vector_CS;

					if (owning_skel->GetOwner())
					{
						const FVector char_up = character_direction_vector_CS;
						//	FVector character_direction_vector = UKismetMathLibrary::TransformDirection(owning_skel->GetComponentToWorld(), char_up);
						character_direction_vector = UKismetMathLibrary::TransformDirection(owning_skel->GetComponentToWorld(), char_up);

					}

					float Scaled_Trace_Radius = Trace_Radius * owner_scale;




					DrawDebugCapsule(PreviewSkelMeshComp->GetWorld(), (TraceStartList[i] + character_direction_vector * Scaled_Trace_Radius) - FVector(0, 0, Vector_Difference.Z * 0.5f), Vector_Difference.Size()*0.5f + (Scaled_Trace_Radius), Scaled_Trace_Radius, FRotator(0, 0, 0).Quaternion(), FColor::Red, false, 0.1f);


					//DrawDebugCapsuleTraceSingle(PreviewSkelMeshComp->GetWorld(), TraceStartList[i], TraceEndList[i], HalfSize, Orientation, DrawDebugType, bHit, OutHit, TraceColor, TraceHitColor, DrawTime);
					//FHitResult out_result;
					//DrawDebugSphereTraceSingle(PreviewSkelMeshComp->GetWorld(), TraceStartList[i], TraceEndList[i], Trace_Radius*owner_scale,EDrawDebugTrace::ForOneFrame, true, out_result,FLinearColor::Red, FLinearColor::Red,0.1f);


					//DrawDebugSphereTraceSingle(World, Start, End, Radius, DrawDebugType, bHit, OutHit, TraceColor, TraceHitColor, DrawTime);
				}
				else
					if (trace_type == EIKTrace_Type_Plugin::ENUM_BoxTrace_Type)
					{
						FVector Vector_Difference = (TraceStartList[i] - TraceEndList[i]);
						Vector_Difference.X = 0;
						Vector_Difference.Y = 0;
						// - FVector(0,0, Vector_Difference.Z*2)
						DrawDebugBox(PreviewSkelMeshComp->GetWorld(), TraceStartList[i] - FVector(0, 0, Vector_Difference.Z * 0.5f), FVector(Trace_Radius*owner_scale, Trace_Radius*owner_scale, Vector_Difference.Size()*0.5f), FColor::Red, false, 0.1f);
					}


		}


	}
#endif

}

//Perform update operations inside this
void FAnimNode_DragonFeetSolver::UpdateInternal(const FAnimationUpdateContext & Context)
{

	FAnimNode_DragonControlBase::UpdateInternal(Context);

	if (enable_solver)
	{


		TraceStartList.Empty();
		TraceEndList.Empty();



		if (test_counter < 500)
			test_counter++;


		trace_draw_counter++;

		if (trace_draw_counter > 5)
			trace_draw_counter = 0;


		scale_mode = 1;

		if (owning_skel->GetOwner())
		{
			scale_mode = owning_skel->GetComponentScale().Z;
		}



		const FVector char_up = character_direction_vector_CS;

		FVector character_direction_vector = UKismetMathLibrary::TransformDirection(owning_skel->GetComponentToWorld(), char_up);


		TArray<FVector> feet_mid_points;
		feet_mid_points.AddUninitialized(spine_hit_pairs.Num());

		for (int32 i = 0; i < spine_hit_pairs.Num(); i++)
		{

			float chest_distance = FMath::Abs(spine_Transform_pairs[i].Spine_Involved.GetLocation().Z - owning_skel->GetComponentToWorld().GetLocation().Z);

			//if (spine_Transform_pairs[i].Associated_Feet.Num() > 0 && spine_hit_pairs[i].RV_Feet_Hits.Num() > 0)
			{
				if (spine_Feet_pair.Num() - 1 < i) continue; // @tikwon

				if (i < spine_Feet_pair.Num() && i < spine_Transform_pairs.Num())
					for (int32 j = 0; j < spine_Feet_pair[i].Associated_Feet.Num(); j++)
					{

						if (j < spine_Transform_pairs[i].Associated_Feet.Num() && dragon_input_data.FeetBones.Num() > spine_Feet_pair[i].order_index[j])
						{

							FVector offseted_linetrace_location = spine_Transform_pairs[i].Associated_Feet[j].GetLocation();

							offseted_linetrace_location += (UKismetMathLibrary::TransformDirection(owning_skel->GetComponentToWorld(), dragon_input_data.FeetBones[spine_Feet_pair[i].order_index[j]].Feet_Trace_Offset ));


							//				line_trace_func(owning_skel, offseted_linetrace_location + FVector(0, 0, Total_spine_heights[i] * 1), offseted_linetrace_location - FVector(0, 0, FeetRootHeights[i][j] * 1), spine_hit_pairs[i].RV_Feet_Hits[j], spine_Feet_pair[i].Associated_Feet[j].BoneName, spine_Feet_pair[i].Associated_Feet[j].BoneName, spine_hit_pairs[i].RV_Feet_Hits[j]);
							FVector a = character_direction_vector * FeetRootHeights[i][j];

							FHitResult d = spine_hit_pairs[i].RV_Feet_Hits[j];

							FName c = spine_Feet_pair[i].Associated_Feet[j].BoneName;




							//FVector feet_direction = (spine_AnimatedTransform_pairs[i].Associated_Knee[j].GetLocation() - spine_AnimatedTransform_pairs[i].Associated_Feet[j].GetLocation()).GetSafeNormal();

							/*
							line_trace_func(owning_skel
								, offseted_linetrace_location + (feet_direction*line_trace_upper_height * scale_mode),
								offseted_linetrace_location - (feet_direction* (FeetRootHeights[i][j] - line_trace_down_height)  ),
								spine_hit_pairs[i].RV_Feet_Hits[j],
								spine_Feet_pair[i].Associated_Feet[j].BoneName,
								spine_Feet_pair[i].Associated_Feet[j].BoneName
								, spine_hit_pairs[i].RV_Feet_Hits[j]);
							*/


							float start_const = line_trace_upper_height * scale_mode;

							float end_const = FeetRootHeights[i][j] + line_trace_down_height * scale_mode;


							line_trace_func(owning_skel, offseted_linetrace_location + character_direction_vector * start_const,
								offseted_linetrace_location - character_direction_vector * end_const,
								//					offseted_linetrace_location,
								spine_hit_pairs[i].RV_Feet_Hits[j], spine_Feet_pair[i].Associated_Feet[j].BoneName
								, spine_Feet_pair[i].Associated_Feet[j].BoneName,
								spine_hit_pairs[i].RV_Feet_Hits[j], FLinearColor::Blue);

						//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " FeetRootHeights : " + FString::SanitizeFloat(FeetRootHeights[i][j]) );


							/*

							FVector forward_dir = owning_skel->GetComponentToWorld().TransformVector(FVector(0,1,0)).GetSafeNormal();
							FVector right_dir = UKismetMathLibrary::GetRightVector(forward_dir.Rotation());




							line_trace_func(owning_skel, offseted_linetrace_location + forward_dir*10 + FVector(0, 0, line_trace_upper_height * scale_mode),
								offseted_linetrace_location + forward_dir * 10 - FVector(0, 0, FeetRootHeights[i][j] - line_trace_down_height),
								//					offseted_linetrace_location,
								spine_hit_pairs[i].RV_FeetFront_Hits[j], spine_Feet_pair[i].Associated_Feet[j].BoneName
								, spine_Feet_pair[i].Associated_Feet[j].BoneName,
								spine_hit_pairs[i].RV_FeetFront_Hits[j], FLinearColor::Red);



							line_trace_func(owning_skel, offseted_linetrace_location + forward_dir * -10 + FVector(0, 0, line_trace_upper_height * scale_mode),
								offseted_linetrace_location + forward_dir * -10 - FVector(0, 0, FeetRootHeights[i][j] - line_trace_down_height),
								//					offseted_linetrace_location,
								spine_hit_pairs[i].RV_FeetBack_Hits[j], spine_Feet_pair[i].Associated_Feet[j].BoneName
								, spine_Feet_pair[i].Associated_Feet[j].BoneName,
								spine_hit_pairs[i].RV_FeetBack_Hits[j], FLinearColor::Red);




							line_trace_func(owning_skel, offseted_linetrace_location+ right_dir*-10 + FVector(0, 0, line_trace_upper_height * scale_mode),
								offseted_linetrace_location + right_dir * -10 - FVector(0, 0, FeetRootHeights[i][j] - line_trace_down_height),
								//					offseted_linetrace_location,
								spine_hit_pairs[i].RV_FeetLeft_Hits[j], spine_Feet_pair[i].Associated_Feet[j].BoneName
								, spine_Feet_pair[i].Associated_Feet[j].BoneName,
								spine_hit_pairs[i].RV_FeetLeft_Hits[j], FLinearColor::Yellow);



							line_trace_func(owning_skel, offseted_linetrace_location + right_dir * 10 + FVector(0, 0, line_trace_upper_height * scale_mode),
								offseted_linetrace_location + right_dir * 10 - FVector(0, 0, FeetRootHeights[i][j] - line_trace_down_height),
								//					offseted_linetrace_location,
								spine_hit_pairs[i].RV_FeetRight_Hits[j], spine_Feet_pair[i].Associated_Feet[j].BoneName
								, spine_Feet_pair[i].Associated_Feet[j].BoneName,
								spine_hit_pairs[i].RV_FeetRight_Hits[j], FLinearColor::Yellow);



								*/



								/*
								if (every_foot_dont_have_child == false)
								{


									float start_const_F = line_trace_upper_height * scale_mode;

									float end_const_F = FeetRootHeights[i][j] + line_trace_down_height;

									line_trace_func(owning_skel
										, spine_AnimatedTransform_pairs[i].Associated_FeetBalls[j].GetLocation() + character_direction_vector* start_const_F,
										spine_AnimatedTransform_pairs[i].Associated_FeetBalls[j].GetLocation(),
										spine_hit_pairs[i].RV_FeetBalls_Hits[j],
										spine_Feet_pair[i].Associated_Feet_Tips[j].BoneName,
										spine_Feet_pair[i].Associated_Feet_Tips[j].BoneName
										, spine_hit_pairs[i].RV_FeetBalls_Hits[j]);
								}
									*/
							if (spine_hit_pairs[i].RV_Feet_Hits[j].bBlockingHit)
							{
								feet_mid_points[i] = spine_hit_pairs[i].RV_Feet_Hits[j].ImpactPoint;

							}

						}
					}
			}


			/*
			if (i == 0)
			{
				FVector a = spine_AnimatedTransform_pairs[i].Spine_Involved.GetLocation();
				float b = Total_spine_heights[i];
				FVector c = spine_AnimatedTransform_pairs[i].Spine_Involved.GetLocation();


				line_trace_func(owning_skel, spine_AnimatedTransform_pairs[i].Spine_Involved.GetLocation() + FVector(0, 0, Total_spine_heights[i] * line_trace_upper_height * scale_mode), spine_AnimatedTransform_pairs[i].Spine_Involved.GetLocation() - FVector(0, 0, line_trace_downward_height * scale_mode), spine_hit_pairs[i].Parent_Spine_Hit, spine_Feet_pair[i].Spine_Involved.BoneName, spine_Feet_pair[i].Spine_Involved.BoneName, spine_hit_pairs[i].Parent_Spine_Hit);
			}
			else
			{
				FVector offseted_vector = spine_Transform_pairs[i].Spine_Involved.GetLocation() - spine_Transform_pairs[0].Spine_Involved.GetLocation();

				line_trace_func(owning_skel, spine_AnimatedTransform_pairs[0].Spine_Involved.GetLocation() + offseted_vector + FVector(0, 0, line_trace_upper_height * scale_mode), spine_AnimatedTransform_pairs[0].Spine_Involved.GetLocation() + offseted_vector - FVector(0, 0, line_trace_downward_height * scale_mode), spine_hit_pairs[i].Parent_Spine_Hit, spine_Feet_pair[i].Spine_Involved.BoneName, spine_Feet_pair[i].Spine_Involved.BoneName, spine_hit_pairs[i].Parent_Spine_Hit);
			}
			*/

		}

	}

}



/*
void FAnimNode_DragonFeetSolver::EvaluateBoneTransforms(USkeletalMeshComponent * SkelComp, FCSPose<FCompactPose>& MeshBases, TArray<FBoneTransform>& OutBoneTransforms)
{
}
*/

void FAnimNode_DragonFeetSolver::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{

	//ComponentPose.EvaluateComponentSpace(Output);

	if (enable_solver && (FVector(0, 0, 0) - Output.AnimInstanceProxy->GetActorTransform().GetScale3D()).Size() > 0 && FAnimWeight::IsRelevant(ActualAlpha) && IsValidToEvaluate(Output.AnimInstanceProxy->GetSkeleton(), Output.AnimInstanceProxy->GetRequiredBones()) && spine_Feet_pair.Num() > 0 && Output.ContainsNaN() == false)
	{


		LineTraceControl_AnyThread(Output, BoneTransforms);


		GetAnimatedPoseInfo(Output.Pose);



		for (int i = 0; i < spine_Feet_pair.Num(); i++)
		{

			if (i < spine_Feet_pair.Num() && i < spine_AnimatedTransform_pairs.Num())
			{

				const FCompactPoseBoneIndex ModifyBoneIndex_Local_i = spine_Feet_pair[i].Spine_Involved.GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer());
				FTransform ComponentBoneTransform_Local_i = Output.Pose.GetComponentSpaceTransform(ModifyBoneIndex_Local_i);
				FVector lerp_data_local_i = owning_skel->GetComponentTransform().TransformPosition(ComponentBoneTransform_Local_i.GetLocation());

				spine_AnimatedTransform_pairs[i].Spine_Involved = (ComponentBoneTransform_Local_i)*owning_skel->GetComponentTransform();

				spine_AnimatedTransform_pairs[i].Spine_Involved.SetRotation(owning_skel->GetComponentToWorld().GetRotation()*ComponentBoneTransform_Local_i.GetRotation());
				FVector back_to_front_dir = ((Output.Pose.GetComponentSpaceTransform(spine_Feet_pair[0].Spine_Involved.GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer()))).GetLocation() - (Output.Pose.GetComponentSpaceTransform(spine_Feet_pair[spine_Feet_pair.Num() - 1].Spine_Involved.GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer()))).GetLocation()).GetSafeNormal();

				FTransform ComponentBoneTransform_Temp = owning_skel->GetComponentToWorld()*ComponentBoneTransform_Local_i;
				//			FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentTransform(), Output.Pose, ComponentBoneTransform_Temp, FCompactPoseBoneIndex(spine_Feet_pair[i].Spine_Involved.BoneIndex), EBoneControlSpace::BCS_WorldSpace);

				FVector dir_world = owning_skel->GetComponentToWorld().TransformVector(back_to_front_dir);



				//			GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " spine rot 2 : " + FRotator(ComponentBoneTransform.GetRotation()).ToString());



				for (int j = 0; j < spine_Feet_pair[i].Associated_Feet.Num(); j++)
				{

					if (j < spine_Feet_pair[i].Associated_Feet.Num())
					{

						const FCompactPoseBoneIndex ModifyBoneIndex_Local_j = spine_Feet_pair[i].Associated_Feet[j].GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer());
						FTransform ComponentBoneTransform_Local_j = Output.Pose.GetComponentSpaceTransform(ModifyBoneIndex_Local_j);
						//				FVector lerp_data_local_j = owning_skel->GetComponentTransform().TransformPosition(ComponentBoneTransform_Local_j.GetLocation());

						if (i < spine_AnimatedTransform_pairs.Num())
						{
							if (j < spine_AnimatedTransform_pairs[i].Associated_Feet.Num())
							{
								spine_AnimatedTransform_pairs[i].Associated_Feet[j] = (ComponentBoneTransform_Local_j)*owning_skel->GetComponentTransform();
							}
						}
						/*
						const FCompactPoseBoneIndex ModifyBoneIndex_Knee_j = spine_Feet_pair[i].Associated_Knees[j].GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer());
						FTransform ComponentBoneTransform_Knee_j = Output.Pose.GetComponentSpaceTransform(ModifyBoneIndex_Knee_j);
						//				FVector lerp_data_local_j = owning_skel->GetComponentTransform().TransformPosition(ComponentBoneTransform_Knee_j.GetLocation());

						spine_AnimatedTransform_pairs[i].Associated_Knee[j] = (ComponentBoneTransform_Knee_j)*owning_skel->GetComponentTransform();
						*/

					}





				}


			}
		}




	



		TArray<TArray<FTransform>> feet_rotation_array = TArray<TArray<FTransform>>();


		for (int spine_index = 0; spine_index < spine_hit_pairs.Num(); spine_index++)
		{
			feet_rotation_array.Add(TArray<FTransform>());

			if (spine_Feet_pair.Num() - 1 < spine_index) continue; // @tikwon

			for (int feet_index = 0; feet_index < spine_hit_pairs[spine_index].RV_Feet_Hits.Num(); feet_index++)
			{

				if (feet_index < spine_hit_pairs[spine_index].RV_Feet_Hits.Num())
				{

					if (spine_Feet_pair[spine_index].Associated_Feet.Num() - 1 < feet_index) continue;

					FTransform EndBoneCSTransform = Output.Pose.GetComponentSpaceTransform(spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex);

					//FTransform EndBoneCSTransform = FTransform::Identity;


					feet_rotation_array[spine_index].Add(EndBoneCSTransform);
				}

			}
		}



	//	Output.ResetToRefPose();

		OptionalRefPose.EvaluateComponentSpace(Output);


		GetFeetHeights(Output);

		if(Use_OptionalRef_Feet_As_Ref)
		Calculate_Feet_Rotation(Output, feet_rotation_array);




		EvaluateComponentSpaceInternal(Output);
		USkeletalMeshComponent* Component = Output.AnimInstanceProxy->GetSkelMeshComponent();
		AnimatedBoneTransforms.Reset(AnimatedBoneTransforms.Num());
		FinalBoneTransforms.Reset(FinalBoneTransforms.Num());
		//Get Initial Pose data
		GetResetedPoseInfo(Output.Pose);
		//Reset Bone Transform array
		BoneTransforms.Reset(BoneTransforms.Num());
		saved_pose = &Output;


		ComponentPose.EvaluateComponentSpace(Output);

		if(!Use_OptionalRef_Feet_As_Ref)
		Calculate_Feet_Rotation(Output, feet_rotation_array);


		//	if (!use_ref_rotation)
		//		Calculate_Feet_Rotation(Output, feet_rotation_array);




		SCOPE_CYCLE_COUNTER(STAT_DragonFeetSolver_Eval);
		check(OutBoneTransforms.Num() == 0);



		if (spine_hit_pairs.Num() > 0)
		{

			

			atleast_one_hit = true;


			//	for (int i = 0; i<feet_bone_array.Num(); i++)
			for (int i = 0; i < spine_hit_pairs.Num(); i++)
			{
				if (spine_Feet_pair.Num() - 1 < i) continue; // @tikwon
				for (int j = 0; j < spine_hit_pairs[i].RV_Feet_Hits.Num(); j++)
				{
					if (spine_Feet_pair[i].Associated_Feet.Num() - 1 < j) continue; // @tikwon
					Leg_Full_Function(spine_Feet_pair[i].Associated_Feet[j].BoneName, i, j, Output, OutBoneTransforms);
				}

			}


		}


	}


}



void FAnimNode_DragonFeetSolver::Leg_Full_Function(FName foot_name, int spine_index, int feet_index, FComponentSpacePoseContext& MeshBasesSaved, TArray<FBoneTransform>& OutBoneTransforms)
{


	FBoneReference ik_feet_bone = FBoneReference(foot_name);
	ik_feet_bone.Initialize(*SavedBoneContainer);

	FTransform bonetraceTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(ik_feet_bone.GetCompactPoseIndex(*SavedBoneContainer));



	FVector lerp_data = owning_skel->GetComponentTransform().TransformPosition(bonetraceTransform.GetLocation());


	if (ik_type == EIK_Type_Plugin::ENUM_Two_Bone_Ik)
		Leg_ik_Function(ik_feet_bone, spine_index, feet_index, EBoneControlSpace::BCS_WorldSpace, EBoneControlSpace::BCS_ComponentSpace, MeshBasesSaved, OutBoneTransforms);
	else
		Leg_Singleik_Function(ik_feet_bone, spine_index, feet_index, EBoneControlSpace::BCS_WorldSpace, EBoneControlSpace::BCS_ComponentSpace, MeshBasesSaved, OutBoneTransforms);

}






void FAnimNode_DragonFeetSolver::Leg_ik_Function(FBoneReference ik_footbone, int spine_index, int feet_index, TEnumAsByte<enum EBoneControlSpace> EffectorLocationSpace, TEnumAsByte<enum EBoneControlSpace> JointTargetLocationSpace, FComponentSpacePoseContext& MeshBasesSaved, TArray<FBoneTransform>& OutBoneTransforms)
{

	// Get indices of the lower and upper limb bones and check validity.
	bool bInvalidLimb = false;

	FCompactPoseBoneIndex IKBoneCompactPoseIndex = ik_footbone.GetCompactPoseIndex(*SavedBoneContainer);


	if (automatic_leg_make == false)
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Feet : " + owning_skel->GetBoneName(spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex.GetInt()).ToString() + " = "+ owning_skel->GetBoneName(IKBoneCompactPoseIndex.GetInt()).ToString());

		IKBoneCompactPoseIndex = spine_Feet_pair[spine_index].Associated_Feet[feet_index].CachedCompactPoseIndex;
	}


	const FVector char_up = character_direction_vector_CS;

	FVector character_direction_vector = UKismetMathLibrary::TransformDirection(owning_skel->GetComponentToWorld(), char_up);



	FCompactPoseBoneIndex LowerLimbIndex = (*SavedBoneContainer).GetParentBoneIndex(IKBoneCompactPoseIndex);

	if (automatic_leg_make == false)
	{
		LowerLimbIndex = spine_Feet_pair[spine_index].Associated_Knees[feet_index].CachedCompactPoseIndex;

		//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Knee : " + owning_skel->GetBoneName(spine_Feet_pair[spine_index].Associated_Knees[feet_index].CachedCompactPoseIndex.GetInt()).ToString() + " = " + owning_skel->GetBoneName(LowerLimbIndex.GetInt()).ToString());

	}



	if (LowerLimbIndex == INDEX_NONE)
	{
		bInvalidLimb = true;
	}


	FCompactPoseBoneIndex UpperLimbIndex = (*SavedBoneContainer).GetParentBoneIndex(LowerLimbIndex);

	if (automatic_leg_make == false)
	{
		UpperLimbIndex = spine_Feet_pair[spine_index].Associated_Thighs[feet_index].CachedCompactPoseIndex;

		//	 GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Thigh : " + owning_skel->GetBoneName(spine_Feet_pair[spine_index].Associated_Thighs[feet_index].CachedCompactPoseIndex.GetInt()).ToString() + " = " + owning_skel->GetBoneName(UpperLimbIndex.GetInt()).ToString());

	}


	if (UpperLimbIndex == INDEX_NONE)
	{
		bInvalidLimb = true;
	}




	const bool bInBoneSpace = (EffectorLocationSpace == BCS_ParentBoneSpace) || (EffectorLocationSpace == BCS_BoneSpace);
	const int32 EffectorBoneIndex = bInBoneSpace ? (*SavedBoneContainer).GetPoseBoneIndexForBoneName("") : INDEX_NONE;
	const FCompactPoseBoneIndex EffectorSpaceBoneIndex = (*SavedBoneContainer).MakeCompactPoseIndex(FMeshPoseBoneIndex(EffectorBoneIndex));


	// If we walked past the root, this controlled is invalid, so return no affected bones.
	if (bInvalidLimb)
	{
		return;
	}



	const FTransform EndBoneLocalTransform = MeshBasesSaved.Pose.GetLocalSpaceTransform(IKBoneCompactPoseIndex);

	// Now get those in component space...
	FTransform LowerLimbCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(LowerLimbIndex);
	FTransform UpperLimbCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(UpperLimbIndex);
	FTransform EndBoneCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex);


	//	FTransform RootBoneCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(FCompactPoseBoneIndex(0));

	FTransform RootBoneCSTransform = FTransform::Identity;


	float feet_root_height = 0;

	// Get current position of root of limb.
	// All position are in Component space.
	const FVector RootPos = UpperLimbCSTransform.GetTranslation();
	const FVector InitialJointPos = LowerLimbCSTransform.GetTranslation();
	const FVector InitialEndPos = EndBoneCSTransform.GetTranslation();



	FVector EffectorLocation_Point = owning_skel->GetComponentToWorld().InverseTransformPosition(spine_AnimatedTransform_pairs[spine_index].Associated_Feet[feet_index].GetLocation());


	if (feet_mod_transform_array[spine_index][feet_index].GetLocation().Equals(FVector::ZeroVector))
	{
		feet_mod_transform_array[spine_index][feet_index].SetLocation(EffectorLocation_Point);

		feet_mod_transform_array[spine_index][feet_index].SetRotation(EndBoneCSTransform.GetRotation());
	}

	//if 


	//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Feet Z : " + FString::SanitizeFloat(FMath::Abs(spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].ImpactPoint.Z - (spine_AnimatedTransform_pairs[spine_index].Associated_Feet[feet_index].GetLocation().Z)))+" FeetBall Z : " + FString::SanitizeFloat(FMath::Abs(spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].ImpactPoint.Z - (spine_AnimatedTransform_pairs[spine_index].Associated_FeetBalls[feet_index].GetLocation().Z))));



	if ((spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit || spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].bBlockingHit) && atleast_one_hit && enable_solver == true)
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, "Feet is hit");



		FTransform eb_wtransform = EndBoneCSTransform;

		FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, eb_wtransform, EffectorSpaceBoneIndex, EffectorLocationSpace);

		if (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit)
		{

			//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, "Feet is not hit");

			EffectorLocation_Point = (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].ImpactPoint + character_direction_vector * FeetRootHeights[spine_index][feet_index]);


			if (every_foot_dont_have_child == false && Use_Feet_Tips)
			{
				if (spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].bBlockingHit && (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].ImpactPoint.Z < spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].ImpactPoint.Z))
				{
					EffectorLocation_Point = (spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].ImpactPoint + character_direction_vector * FeetRootHeights[spine_index][feet_index]);
				}
			}


			EffectorLocation_Point = owning_skel->GetComponentToWorld().InverseTransformPosition(EffectorLocation_Point);

			if (interpolate_only_z)
			{
				FVector X_Y_Loc = EndBoneCSTransform.GetLocation();

				EffectorLocation_Point.X = X_Y_Loc.X;
				EffectorLocation_Point.Y = X_Y_Loc.Y;
			}

			//			feet_mod_transform_array[spine_index][feet_index].SetLocation(EffectorLocation_Point);
		}

		//   feet_mod_transform_array[spine_index][feet_index].SetLocation(EffectorLocation_Point);



		if (Ignore_Location_Lerping == false && (feet_mod_transform_array[spine_index][feet_index].GetLocation() - EffectorLocation_Point).Size() < 500)
		{                            //UKismetMathLibrary::VLerp
			feet_mod_transform_array[spine_index][feet_index].SetLocation(AnimLocLerp(feet_mod_transform_array[spine_index][feet_index].GetLocation(), EffectorLocation_Point, owning_skel->GetWorld()->DeltaTimeSeconds * 15 * Location_Lerp_Speed));
		}
		else
			feet_mod_transform_array[spine_index][feet_index].SetLocation(EffectorLocation_Point);



		feet_Alpha_array[spine_index][feet_index] = UKismetMathLibrary::FInterpTo(feet_Alpha_array[spine_index][feet_index], 1, 1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds()), shift_speed*0.5f);

	}
	else
	{

		FTransform eb_wtransform = EndBoneCSTransform;

		if (Ignore_Location_Lerping == false && (feet_mod_transform_array[spine_index][feet_index].GetLocation() - eb_wtransform.GetLocation()).Size() < 500)
			feet_mod_transform_array[spine_index][feet_index].SetLocation(AnimLocLerp(feet_mod_transform_array[spine_index][feet_index].GetLocation(), eb_wtransform.GetLocation(), owning_skel->GetWorld()->DeltaTimeSeconds * 15 * Location_Lerp_Speed));
		else
			feet_mod_transform_array[spine_index][feet_index].SetLocation(eb_wtransform.GetLocation());

		FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, eb_wtransform, EffectorSpaceBoneIndex, EffectorLocationSpace);
		EffectorLocation_Point = eb_wtransform.GetLocation();


		//	feet_Alpha_array[spine_index][feet_index] = 0;

		feet_Alpha_array[spine_index][feet_index] = UKismetMathLibrary::FInterpTo(feet_Alpha_array[spine_index][feet_index], 0, 1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds()), shift_speed*0.5f);

	}



	//	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, " Feet Root Alpha : " + FString::SanitizeFloat(feet_Alpha_array[spine_index][feet_index]));


		// Transform EffectorLocation from EffectorLocationSpace to ComponentSpace.
	FTransform EffectorTransform(owning_skel->GetComponentToWorld().TransformPosition(feet_mod_transform_array[spine_index][feet_index].GetLocation()));

	//	FTransform EffectorTransform(DebugEffectorTransform);

	FAnimationRuntime::ConvertBoneSpaceTransformToCS(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, EffectorTransform, EffectorSpaceBoneIndex, EffectorLocationSpace);

	// This is our reach goal.
	FVector DesiredPos = EffectorTransform.GetTranslation();
	FVector DesiredDelta = DesiredPos - RootPos;
	float DesiredLength = DesiredDelta.Size();

	// Check to handle case where DesiredPos is the same as RootPos.
	FVector	DesiredDir;
	if (DesiredLength < (float)KINDA_SMALL_NUMBER)
	{
		DesiredLength = (float)KINDA_SMALL_NUMBER;
		DesiredDir = FVector(1, 0, 0);
	}
	else
	{
		DesiredDir = DesiredDelta / DesiredLength;
	}



	// Get joint target (used for defining plane that joint should be in).
	//	FTransform JointTargetTransform(ik_footbone.joint_point);

	//The transform for the bending direction joint point
	FTransform BendingDirectionTransform = LowerLimbCSTransform;


	FVector cs_forward = owning_skel->GetComponentToWorld().InverseTransformVector(owning_skel->GetRightVector());


	FVector UpperLimb_WS = owning_skel->GetComponentToWorld().TransformPosition(UpperLimbCSTransform.GetLocation());
	FVector EndLimb_WS = owning_skel->GetComponentToWorld().TransformPosition(EndBoneCSTransform.GetLocation());
	FVector LowerLimb_WS = owning_skel->GetComponentToWorld().TransformPosition(LowerLimbCSTransform.GetLocation());


//	cs_forward = owning_skel->GetComponentToWorld().InverseTransformVector( (((UpperLimb_WS + EndLimb_WS + LowerLimb_WS) / 3) - LowerLimb_WS).GetSafeNormal() );



	if( dragon_input_data.FeetBones.Num() > spine_Feet_pair[spine_index].order_index[feet_index])
	cs_forward = (((UpperLimbCSTransform.GetLocation() + EndBoneCSTransform.GetLocation() + LowerLimbCSTransform.GetLocation() ) / 3) - (LowerLimbCSTransform.GetLocation() + dragon_input_data.FeetBones[spine_Feet_pair[spine_index].order_index[feet_index]].Knee_Direction_Offset) ).GetSafeNormal();


	BendingDirectionTransform.SetLocation(BendingDirectionTransform.GetLocation() + cs_forward *-1000);

	//BendingDirectionTransform.SetLocation(BendingDirectionTransform.GetLocation() + cs_forward * -1000);

	//BendingDirectionTransform.AddToTranslation(spine_Feet_pair[spine_index].knee_direction_offset[feet_index]);


	//feet_knee_offset_array[spine_index][feet_index] = owning_skel->GetComponentToWorld().TransformPosition(LowerLimbCSTransform.GetLocation() );

	if (dragon_input_data.FeetBones.Num() > spine_Feet_pair[spine_index].order_index[feet_index])
	feet_knee_offset_array[spine_index][feet_index] = owning_skel->GetComponentToWorld().TransformPosition(LowerLimbCSTransform.GetLocation() + dragon_input_data.FeetBones[spine_Feet_pair[spine_index].order_index[feet_index]].Knee_Direction_Offset);

	//feet_knee_offset_array[spine_index][feet_index] = owning_skel->GetComponentToWorld().TransformPosition(LowerLimbCSTransform.GetLocation() + Foot_Knee_Array[spine_index]);



	FTransform JointTargetTransform(BendingDirectionTransform);


	FCompactPoseBoneIndex JointTargetSpaceBoneIndex(INDEX_NONE);


	FVector	JointTargetPos = JointTargetTransform.GetTranslation();
	FVector JointTargetDelta = JointTargetPos - RootPos;
	const float JointTargetLengthSqr = JointTargetDelta.SizeSquared();

	// Same check as above, to cover case when JointTarget position is the same as RootPos.
	FVector JointPlaneNormal, JointBendDir;
	if (JointTargetLengthSqr < FMath::Square((float)KINDA_SMALL_NUMBER))
	{
		JointBendDir = FVector(0, 1, 0);
		//JointPlaneNormal = FVector(0, 0, 1);
		JointPlaneNormal = character_direction_vector;

	}
	else
	{
		JointPlaneNormal = DesiredDir ^ JointTargetDelta;

		// If we are trying to point the limb in the same direction that we are supposed to displace the joint in, 
		// we have to just pick 2 random vector perp to DesiredDir and each other.
		if (JointPlaneNormal.SizeSquared() < FMath::Square((float)KINDA_SMALL_NUMBER))
		{
			DesiredDir.FindBestAxisVectors(JointPlaneNormal, JointBendDir);
		}
		else
		{
			JointPlaneNormal.Normalize();

			// Find the final member of the reference frame by removing any component of JointTargetDelta along DesiredDir.
			// This should never leave a zero vector, because we've checked DesiredDir and JointTargetDelta are not parallel.
			JointBendDir = JointTargetDelta - ((JointTargetDelta | DesiredDir) * DesiredDir);
			JointBendDir.Normalize();
		}
	}


	float LowerLimbLength = (InitialEndPos - InitialJointPos).Size();
	float UpperLimbLength = (InitialJointPos - RootPos).Size();
	float MaxLimbLength = LowerLimbLength + UpperLimbLength;


	FVector OutEndPos = DesiredPos;
	FVector OutJointPos = InitialJointPos;

	// If we are trying to reach a goal beyond the length of the limb, clamp it to something solvable and extend limb fully.
	if (DesiredLength > MaxLimbLength)
	{
		OutEndPos = RootPos + (MaxLimbLength * DesiredDir);
		OutJointPos = RootPos + (UpperLimbLength * DesiredDir);
	}
	else
	{
		// So we have a triangle we know the side lengths of. We can work out the angle between DesiredDir and the direction of the upper limb
		// using the sin rule:
		const float TwoAB = 2.f * UpperLimbLength * DesiredLength;

		const float CosAngle = (TwoAB != 0.f) ? ((UpperLimbLength*UpperLimbLength) + (DesiredLength*DesiredLength) - (LowerLimbLength*LowerLimbLength)) / TwoAB : 0.f;

		// If CosAngle is less than 0, the upper arm actually points the opposite way to DesiredDir, so we handle that.
		const bool bReverseUpperBone = (CosAngle < 0.f);

		// If CosAngle is greater than 1.f, the triangle could not be made - we cannot reach the target.
		// We just have the two limbs double back on themselves, and EndPos will not equal the desired EffectorLocation.
		if ((CosAngle > 1.f) || (CosAngle < -1.f))
		{
			// Because we want the effector to be a positive distance down DesiredDir, we go back by the smaller section.
			if (UpperLimbLength > LowerLimbLength)
			{
				OutJointPos = RootPos + (UpperLimbLength * DesiredDir);
				OutEndPos = OutJointPos - (LowerLimbLength * DesiredDir);
			}
			else
			{
				OutJointPos = RootPos - (UpperLimbLength * DesiredDir);
				OutEndPos = OutJointPos + (LowerLimbLength * DesiredDir);
			}
		}
		else
		{
			// Angle between upper limb and DesiredDir
			const float Angle = FMath::Acos(CosAngle);

			// Now we calculate the distance of the joint from the root -> effector line.
			// This forms a right-angle triangle, with the upper limb as the hypotenuse.
			const float JointLineDist = UpperLimbLength * FMath::Sin(Angle);

			// And the final side of that triangle - distance along DesiredDir of perpendicular.
			// ProjJointDistSqr can't be neg, because JointLineDist must be <= UpperLimbLength because appSin(Angle) is <= 1.
			const float ProjJointDistSqr = (UpperLimbLength*UpperLimbLength) - (JointLineDist*JointLineDist);
			// although this shouldn't be ever negative, sometimes Xbox release produces -0.f, causing ProjJointDist to be NaN
			// so now I branch it. 						
			float ProjJointDist = (ProjJointDistSqr > 0.f) ? FMath::Sqrt(ProjJointDistSqr) : 0.f;
			if (bReverseUpperBone)
			{
				ProjJointDist *= -1.f;
			}

			// So now we can work out where to put the joint!
			OutJointPos = RootPos + (ProjJointDist * DesiredDir) + (JointLineDist * JointBendDir);
		}
	}


	// Update transform for upper bone.
	{
		// Get difference in direction for old and new joint orientations
		FVector const OldDir = (InitialJointPos - RootPos).GetSafeNormal();
		FVector const NewDir = (OutJointPos - RootPos).GetSafeNormal();
		// Find Delta Rotation take takes us from Old to New dir
		FQuat const DeltaRotation = FQuat::FindBetweenNormals(OldDir, NewDir);
		// Rotate our Joint quaternion by this delta rotation




		UpperLimbCSTransform.SetRotation(DeltaRotation * UpperLimbCSTransform.GetRotation());
		// And put joint where it should be.
		UpperLimbCSTransform.SetTranslation(RootPos);

		// Order important. First bone is upper limb.
	}

	// Update transform for lower bone.
	{
		// Get difference in direction for old and new joint orientations
		FVector const OldDir = (InitialEndPos - InitialJointPos).GetSafeNormal();
		FVector const NewDir = (OutEndPos - OutJointPos).GetSafeNormal();

		// Find Delta Rotation take takes us from Old to New dir
		FQuat const DeltaRotation = FQuat::FindBetweenNormals(OldDir, NewDir);





		LowerLimbCSTransform.SetRotation(DeltaRotation * LowerLimbCSTransform.GetRotation());
		// And put joint where it should be.
		LowerLimbCSTransform.SetTranslation(OutJointPos);



		// Order important. Second bone is lower limb.
	}

	// Update transform for end bone.




	{

		// Set correct location for end bone.
		EndBoneCSTransform.SetTranslation(OutEndPos);
		//+ impact_result.ImpactNormal.GetSafeNormal() 


		//		MeshBasesSaved.ResetToRefPose();

		if (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit&& atleast_one_hit)
		{
			if (owning_skel->GetOwner() != nullptr)
			{
				//				FRotator input_rotation_axis;



							//	input_rotation_axis = FRotator(YZ_rot.Pitch, bone_rotation.Yaw, XZ_rot.Roll);

							//	FRotator direction_rot = BoneRelativeConversion(spine_Feet_pair[spine_index].feet_rotation_offset[feet_index],spine_Feet_pair[spine_index].Associated_Feet[feet_index].GetCompactPoseIndex(*SavedBoneContainer), input_rotation_axis, *SavedBoneContainer, MeshBasesSaved.Pose);


				//				feet_mod_transform_array[spine_index][feet_index].SetRotation(direction_rot.Quaternion());


				FRotator rot_result = feet_mod_transform_array[spine_index][feet_index].Rotator();



				FTransform FeetCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex);

				FTransform Feet_Saved_Transform = FTransform(feet_mod_transform_array[spine_index][feet_index].Rotator());



				if (owning_skel->GetWorld()->IsGameWorld() == false)
				{
					feet_mod_transform_array[spine_index][feet_index].SetRotation(FeetCSTransform.GetRotation());
				}
				else
				{

					if (Should_Rotate_Feet)
						EndBoneCSTransform.SetRotation(feet_mod_transform_array[spine_index][feet_index].GetRotation());

				}



				//						EndBoneCSTransform.SetRotation(feet_mod_transform_array[spine_index][feet_index].GetRotation());





			}


			//					EndBoneCSTransform.SetRotation(UKismetMathLibrary::RInterpTo_Constant(EndBoneCSTransform.Rotator(), feet_mod_transform_array[spine_index][feet_index].Rotator(), feet_Alpha_array[spine_index][feet_index]));





		}
		else
		{


			if(feet_mod_transform_array[spine_index][feet_index].Rotator().Equals(FRotator::ZeroRotator))
			feet_mod_transform_array[spine_index][feet_index].SetRotation(EndBoneCSTransform.GetRotation());


			if (Ignore_Lerping == false)
				feet_mod_transform_array[spine_index][feet_index].SetRotation(UKismetMathLibrary::RLerp(feet_mod_transform_array[spine_index][feet_index].Rotator(), EndBoneCSTransform.Rotator(), owning_skel->GetWorld()->DeltaTimeSeconds*feet_rotation_speed * 5, true).Quaternion());
			else
				feet_mod_transform_array[spine_index][feet_index].SetRotation(EndBoneCSTransform.GetRotation());



			if (Should_Rotate_Feet)
			{
				EndBoneCSTransform.SetRotation(feet_mod_transform_array[spine_index][feet_index].GetRotation());
			}

			/*

				FTransform LowerLimbCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(LowerLimbIndex);
				FTransform UpperLimbCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(UpperLimbIndex);
				FTransform EndBoneCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex);


				//feet_mod_transform_array[spine_index][feet_index].SetRotation(UKismetMathLibrary::RInterpTo(feet_mod_transform_array[spine_index][feet_index].Rotator(), EndBoneCSTransformX.Rotator(),(1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds())),shift_speed).Quaternion());


	//			feet_mod_transform_array[spine_index][feet_index].SetRotation(UKismetMathLibrary::RInterpTo(feet_mod_transform_array[spine_index][feet_index].Rotator(), EndBoneCSTransformX.Rotator(), (1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds())), shift_speed).Quaternion());




				if (owning_skel->GetWorld()->IsGameWorld() == false)
				{
					feet_mod_transform_array[spine_index][feet_index].SetRotation(EndBoneCSTransformX.GetRotation());
				}
				else
				{


					if(Ignore_Lerping==false)
					feet_mod_transform_array[spine_index][feet_index].SetRotation(FQuat::Slerp(feet_mod_transform_array[spine_index][feet_index].GetRotation(), EndBoneCSTransformX.GetRotation(), (1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds()))*feet_rotation_speed));
					else
						feet_mod_transform_array[spine_index][feet_index].SetRotation(EndBoneCSTransformX.GetRotation());



				}






	//			feet_mod_transform_array[spine_index][feet_index].SetRotation(EndBoneCSTransformX.GetRotation());

				EndBoneCSTransform = EndBoneCSTransformX;
				LowerLimbCSTransform = LowerLimbCSTransformX;

				UpperLimbCSTransform = UpperLimbCSTransformX;

				if(Should_Rotate_Feet)
				EndBoneCSTransform.SetRotation(feet_mod_transform_array[spine_index][feet_index].GetRotation());



				*/




		}




	}


	//feet_Alpha_array[spine_index][feet_index] = 1;


	FTransform LowerLimbCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(LowerLimbIndex);
	FTransform UpperLimbCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(UpperLimbIndex);
	FTransform EndBoneCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex);

	//EndBoneCSTransformX.SetRotation(EndBoneCSTransform.GetRotation());

	////


	//FTransform NewBoneTM = MeshBasesSaved.GetComponentSpaceTransform(IKBoneCompactPoseIndex);

	//FAnimationRuntime::Convert


//	FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, eb_wtransform, EffectorSpaceBoneIndex, EffectorLocationSpace);

	FTransform FeetTransform = EndBoneCSTransformX;
	FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, FeetTransform, IKBoneCompactPoseIndex, EBoneControlSpace::BCS_ParentBoneSpace);

	FRotator offset_local = spine_Feet_pair[spine_index].feet_rotation_offset[feet_index];
	const FQuat BoneInput(offset_local);
	FRotator original_relation_rot = FeetTransform.Rotator();
	original_relation_rot.Yaw = 0;
	original_relation_rot.Pitch = 0;

	//	EndBoneCSTransform.SetRotation(EndBoneCSTransform.GetRotation()*original_relation_rot.Quaternion());

	//	EndBoneCSTransform.SetRotation(EndBoneCSTransform.GetRotation()*BoneInput);




		// Convert back to Component Space.
	//	FAnimationRuntime::ConvertBoneSpaceTransformToCS(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, FeetTransform, IKBoneCompactPoseIndex, EBoneControlSpace::BCS_BoneSpace);


		////

	//	FTransform Lerped_EndBoneCSTransform = UKismetMathLibrary::TLerp(EndBoneCSTransformX, EndBoneCSTransform, feet_Alpha_array[spine_index][feet_index]);
	//	FTransform Lerped_LowerLimbCSTransform = UKismetMathLibrary::TLerp(LowerLimbCSTransformX, LowerLimbCSTransform, feet_Alpha_array[spine_index][feet_index]);
	//	FTransform Lerped_UpperLimbCSTransform = UKismetMathLibrary::TLerp(UpperLimbCSTransformX, UpperLimbCSTransform, feet_Alpha_array[spine_index][feet_index]);




	FTransform Lerped_EndBoneCSTransform = UKismetMathLibrary::TLerp(EndBoneCSTransformX, EndBoneCSTransform, feet_Alpha_array[spine_index][feet_index]);

	//	if( FMath::Abs<float>( (EndBoneCSTransformX.GetLocation() - EndBoneCSTransform.GetLocation()).Size())>1000  )
	{
		//		Lerped_EndBoneCSTransform = EndBoneCSTransform;
	}


	FTransform Lerped_LowerLimbCSTransform = UKismetMathLibrary::TLerp(LowerLimbCSTransformX, LowerLimbCSTransform, feet_Alpha_array[spine_index][feet_index]);

	//	if (FMath::Abs<float>((LowerLimbCSTransformX.GetLocation() - LowerLimbCSTransform.GetLocation()).Size())>1000)
	{
		//		Lerped_LowerLimbCSTransform = LowerLimbCSTransform;
	}


	FTransform Lerped_UpperLimbCSTransform = UKismetMathLibrary::TLerp(UpperLimbCSTransformX, UpperLimbCSTransform, feet_Alpha_array[spine_index][feet_index]);

	//	if (FMath::Abs<float>((UpperLimbCSTransformX.GetLocation() - UpperLimbCSTransform.GetLocation()).Size())>1000)
	{
		//		Lerped_UpperLimbCSTransform = UpperLimbCSTransform;
	}

	//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Feet 1 : " + owning_skel->GetBoneName(UpperLimbIndex.GetInt() ).ToString());
	//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Feet 2 : " + owning_skel->GetBoneName(LowerLimbIndex.GetInt()).ToString());
	//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, " Feet 3 : " + owning_skel->GetBoneName(IKBoneCompactPoseIndex.GetInt()).ToString());



	OutBoneTransforms.Add(FBoneTransform(UpperLimbIndex, Lerped_UpperLimbCSTransform));

	OutBoneTransforms.Add(FBoneTransform(LowerLimbIndex, Lerped_LowerLimbCSTransform));

	OutBoneTransforms.Add(FBoneTransform(IKBoneCompactPoseIndex, Lerped_EndBoneCSTransform));

	/*
		OutBoneTransforms.Add(FBoneTransform(UpperLimbIndex, Lerped_UpperLimbCSTransform));

		OutBoneTransforms.Add(FBoneTransform(LowerLimbIndex, Lerped_LowerLimbCSTransform));

		OutBoneTransforms.Add(FBoneTransform(IKBoneCompactPoseIndex, Lerped_EndBoneCSTransform));
		*/

}










void FAnimNode_DragonFeetSolver::Leg_Singleik_Function(FBoneReference ik_footbone, int spine_index, int feet_index, TEnumAsByte<enum EBoneControlSpace> EffectorLocationSpace, TEnumAsByte<enum EBoneControlSpace> JointTargetLocationSpace, FComponentSpacePoseContext & MeshBasesSaved, TArray<FBoneTransform>& OutBoneTransforms)
{





	// Get indices of the lower and upper limb bones and check validity.
	bool bInvalidLimb = false;

	FCompactPoseBoneIndex IKBoneCompactPoseIndex = ik_footbone.GetCompactPoseIndex(*SavedBoneContainer);


	const FCompactPoseBoneIndex UpperLimbIndex = (*SavedBoneContainer).GetParentBoneIndex(IKBoneCompactPoseIndex);
	if (UpperLimbIndex == INDEX_NONE)
	{
		bInvalidLimb = true;
	}


	const FVector char_up = character_direction_vector_CS;

	FVector character_direction_vector = UKismetMathLibrary::TransformDirection(owning_skel->GetComponentToWorld(), char_up);



	const bool bInBoneSpace = (EffectorLocationSpace == BCS_ParentBoneSpace) || (EffectorLocationSpace == BCS_BoneSpace);
	const int32 EffectorBoneIndex = bInBoneSpace ? (*SavedBoneContainer).GetPoseBoneIndexForBoneName("") : INDEX_NONE;
	const FCompactPoseBoneIndex EffectorSpaceBoneIndex = (*SavedBoneContainer).MakeCompactPoseIndex(FMeshPoseBoneIndex(EffectorBoneIndex));


	// If we walked past the root, this controlled is invalid, so return no affected bones.
	if (bInvalidLimb)
	{
		return;
	}



	const FTransform EndBoneLocalTransform = MeshBasesSaved.Pose.GetLocalSpaceTransform(IKBoneCompactPoseIndex);

	// Now get those in component space...
	FTransform UpperLimbCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(UpperLimbIndex);
	FTransform EndBoneCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex);


	FTransform RootBoneCSTransform = MeshBasesSaved.Pose.GetComponentSpaceTransform(FCompactPoseBoneIndex(0));



	float feet_root_height = 0;

	// Get current position of root of limb.
	// All position are in Component space.
	const FVector RootPos = UpperLimbCSTransform.GetTranslation();
	const FVector InitialEndPos = EndBoneCSTransform.GetTranslation();



	FVector EffectorLocation_Point;



	if ((spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit || spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].bBlockingHit) && atleast_one_hit && enable_solver == true)
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, "Feet is hit");



		FTransform eb_wtransform = EndBoneCSTransform;

		FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, eb_wtransform, EffectorSpaceBoneIndex, EffectorLocationSpace);

		if (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit)
		{

			//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, "Feet is not hit");

			EffectorLocation_Point = (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].ImpactPoint + character_direction_vector * FeetRootHeights[spine_index][feet_index]);


			if (every_foot_dont_have_child == false && Use_Feet_Tips)
			{
				if (spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].bBlockingHit && (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].ImpactPoint.Z < spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].ImpactPoint.Z))
				{
					EffectorLocation_Point = (spine_hit_pairs[spine_index].RV_FeetBalls_Hits[feet_index].ImpactPoint + character_direction_vector * FeetRootHeights[spine_index][feet_index]);
				}
			}


			EffectorLocation_Point = owning_skel->GetComponentToWorld().InverseTransformPosition(EffectorLocation_Point);

			if (interpolate_only_z)
			{
				FVector X_Y_Loc = EndBoneCSTransform.GetLocation();

				EffectorLocation_Point.X = X_Y_Loc.X;
				EffectorLocation_Point.Y = X_Y_Loc.Y;
			}


			//			feet_mod_transform_array[spine_index][feet_index].SetLocation(EffectorLocation_Point);
		}

		//   feet_mod_transform_array[spine_index][feet_index].SetLocation(EffectorLocation_Point);




		//if (Ignore_Location_Lerping == false)
		if (Ignore_Location_Lerping == false && (feet_mod_transform_array[spine_index][feet_index].GetLocation() - EffectorLocation_Point).Size() < 500)
		{
			feet_mod_transform_array[spine_index][feet_index].SetLocation(AnimLocLerp(feet_mod_transform_array[spine_index][feet_index].GetLocation(), EffectorLocation_Point, owning_skel->GetWorld()->DeltaTimeSeconds * 15 * Location_Lerp_Speed));
		}
		else
		{
			feet_mod_transform_array[spine_index][feet_index].SetLocation(EffectorLocation_Point);
		}


		feet_Alpha_array[spine_index][feet_index] = UKismetMathLibrary::FInterpTo(feet_Alpha_array[spine_index][feet_index], 1, 1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds()), shift_speed*0.5f);

	}
	else
	{

		FTransform eb_wtransform = EndBoneCSTransform;



		//if (Ignore_Location_Lerping == false)
		if (Ignore_Location_Lerping == false && (feet_mod_transform_array[spine_index][feet_index].GetLocation() - eb_wtransform.GetLocation()).Size() < 500)
			feet_mod_transform_array[spine_index][feet_index].SetLocation(AnimLocLerp(feet_mod_transform_array[spine_index][feet_index].GetLocation(), eb_wtransform.GetLocation(), owning_skel->GetWorld()->DeltaTimeSeconds * 15 * Location_Lerp_Speed));
		else
			feet_mod_transform_array[spine_index][feet_index].SetLocation(eb_wtransform.GetLocation());

		FAnimationRuntime::ConvertCSTransformToBoneSpace(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, eb_wtransform, EffectorSpaceBoneIndex, EffectorLocationSpace);
		EffectorLocation_Point = eb_wtransform.GetLocation();


		//	feet_Alpha_array[spine_index][feet_index] = 0;

		feet_Alpha_array[spine_index][feet_index] = UKismetMathLibrary::FInterpTo(feet_Alpha_array[spine_index][feet_index], 0, 1 - FMath::Exp(-10 * owning_skel->GetWorld()->GetDeltaSeconds()), shift_speed*0.5f);

	}



	//	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, " Feet Root Alpha : " + FString::SanitizeFloat(feet_Alpha_array[spine_index][feet_index]));


		// Transform EffectorLocation from EffectorLocationSpace to ComponentSpace.
	FTransform EffectorTransform(owning_skel->GetComponentToWorld().TransformPosition(feet_mod_transform_array[spine_index][feet_index].GetLocation()));

	// Transform EffectorLocation from EffectorLocationSpace to ComponentSpace.
	//FTransform EffectorTransform(feet_mod_transform_array[spine_index][feet_index]);

	//	FTransform EffectorTransform(DebugEffectorTransform);

	FAnimationRuntime::ConvertBoneSpaceTransformToCS(owning_skel->GetComponentTransform(), MeshBasesSaved.Pose, EffectorTransform, EffectorSpaceBoneIndex, EffectorLocationSpace);

	// This is our reach goal.
	FVector DesiredPos = EffectorTransform.GetTranslation();
	FVector DesiredDelta = DesiredPos - RootPos;
	float DesiredLength = DesiredDelta.Size();

	// Check to handle case where DesiredPos is the same as RootPos.
	FVector	DesiredDir;
	if (DesiredLength < (float)KINDA_SMALL_NUMBER)
	{
		DesiredLength = (float)KINDA_SMALL_NUMBER;
		DesiredDir = FVector(1, 0, 0);
	}
	else
	{
		DesiredDir = DesiredDelta / DesiredLength;
	}













	float UpperLimbLength = (InitialEndPos - RootPos).Size();
	float MaxLimbLength = UpperLimbLength;


	FVector OutEndPos = DesiredPos;

	// If we are trying to reach a goal beyond the length of the limb, clamp it to something solvable and extend limb fully.
	if (DesiredLength > MaxLimbLength)
	{
		OutEndPos = RootPos + (MaxLimbLength * DesiredDir);
	}
	else
	{

		OutEndPos = EffectorTransform.GetLocation();

	}


	// Update transform for upper bone.
	{
		// Get difference in direction for old and new joint orientations
		FVector const OldDir = (InitialEndPos - RootPos).GetSafeNormal();
		FVector const NewDir = (OutEndPos - RootPos).GetSafeNormal();
		// Find Delta Rotation take takes us from Old to New dir
		FQuat const DeltaRotation = FQuat::FindBetweenNormals(OldDir, NewDir);
		// Rotate our Joint quaternion by this delta rotation




		UpperLimbCSTransform.SetRotation(DeltaRotation * UpperLimbCSTransform.GetRotation());
		// And put joint where it should be.
		UpperLimbCSTransform.SetTranslation(RootPos);

		// Order important. First bone is upper limb.
	}




	{

		EndBoneCSTransform.SetTranslation(OutEndPos);

		if (spine_hit_pairs[spine_index].RV_Feet_Hits[feet_index].bBlockingHit&& atleast_one_hit)
		{

			EndBoneCSTransform.SetRotation(feet_mod_transform_array[spine_index][feet_index].GetRotation());

		}
		else
		{


			FTransform UpperLimbCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(UpperLimbIndex);
			FTransform EndBoneCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex);


			//			feet_mod_transform_array[spine_index][feet_index].SetRotation(UKismetMathLibrary::RLerp(feet_mod_transform_array[spine_index][feet_index].Rotator(), EndBoneCSTransform.Rotator(), owning_skel->GetWorld()->GetDeltaSeconds() * 15, true).Quaternion());


			feet_mod_transform_array[spine_index][feet_index].SetRotation(EndBoneCSTransform.Rotator().Quaternion());


			EndBoneCSTransform = EndBoneCSTransformX;

			UpperLimbCSTransform = UpperLimbCSTransformX;

			EndBoneCSTransform.SetRotation(feet_mod_transform_array[spine_index][feet_index].GetRotation());








		}




	}



	FTransform UpperLimbCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(UpperLimbIndex);
	FTransform EndBoneCSTransformX = MeshBasesSaved.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex);

	//feet_Alpha_array[spine_index][feet_index] = 0;


	FTransform Lerped_EndBoneCSTransform = UKismetMathLibrary::TLerp(EndBoneCSTransformX, EndBoneCSTransform, feet_Alpha_array[spine_index][feet_index]);
	FTransform Lerped_UpperLimbCSTransform = UKismetMathLibrary::TLerp(UpperLimbCSTransformX, UpperLimbCSTransform, feet_Alpha_array[spine_index][feet_index]);


	OutBoneTransforms.Add(FBoneTransform(UpperLimbIndex, Lerped_UpperLimbCSTransform));

	OutBoneTransforms.Add(FBoneTransform(IKBoneCompactPoseIndex, Lerped_EndBoneCSTransform));








}






void FAnimNode_DragonFeetSolver::LineTraceControl_AnyThread(FComponentSpacePoseContext & Output, TArray<FBoneTransform>& OutBoneTransforms)
{

	if (spine_hit_pairs.Num() > 0 && spine_Feet_pair.Num() > 0 && spine_Transform_pairs.Num() > 0 && !Output.Pose.GetPose().ContainsNaN())
	{

		for (int32 i = 0; i < spine_Feet_pair.Num(); i++)
		{
			if (i < spine_Transform_pairs.Num())
			{

				const FCompactPoseBoneIndex ModifyBoneIndex_Local_i = spine_Feet_pair[i].Spine_Involved.GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer());
				FTransform ComponentBoneTransform_Local_i;
				ComponentBoneTransform_Local_i = Output.Pose.GetComponentSpaceTransform(ModifyBoneIndex_Local_i);
				FVector lerp_data_Local_i = owning_skel->GetComponentTransform().TransformPosition(ComponentBoneTransform_Local_i.GetLocation());

				spine_Transform_pairs[i].Spine_Involved = ComponentBoneTransform_Local_i * owning_skel->GetComponentTransform();

				//spine_Transform_pairs[i].Spine_Involved = FTransform(lerp_data_Local_i);



				for (int32 j = 0; j < spine_Feet_pair[i].Associated_Feet.Num(); j++)
				{

					if (j < spine_Transform_pairs[i].Associated_Feet.Num())
					{
						const FCompactPoseBoneIndex ModifyBoneIndex_Local_j = spine_Feet_pair[i].Associated_Feet[j].GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer());
						FTransform ComponentBoneTransform_Local_j;
						ComponentBoneTransform_Local_j = Output.Pose.GetComponentSpaceTransform(ModifyBoneIndex_Local_j);
						FVector lerp_data_Local_j = owning_skel->GetComponentTransform().TransformPosition(ComponentBoneTransform_Local_j.GetLocation());
						//				spine_Transform_pairs[i].Associated_Feet[j] = FTransform(lerp_data_Local_j);

						spine_Transform_pairs[i].Associated_Feet[j] = ComponentBoneTransform_Local_j * owning_skel->GetComponentTransform();
					}

				}
			}
		}
	}



	if (feet_transform_array.Num() > 0)
	{

		for (int32 i = 0; i < feet_bone_array.Num(); i++)
		{
			const FCompactPoseBoneIndex ModifyBoneIndex = feet_bone_array[i].GetCompactPoseIndex(Output.Pose.GetPose().GetBoneContainer());
			FTransform ComponentBoneTransform;
			ComponentBoneTransform = Output.Pose.GetComponentSpaceTransform(ModifyBoneIndex);
			FVector lerp_data = owning_skel->GetComponentTransform().TransformPosition(ComponentBoneTransform.GetLocation());
			feet_transform_array[i] = FTransform(lerp_data);

		}


	}
}



bool FAnimNode_DragonFeetSolver::IsValidToEvaluate(const USkeleton * Skeleton, const FBoneContainer & RequiredBones)
{


	bool feet_is_true = true;

	for (int i = 0; i < dragon_bone_data.FeetBones.Num(); i++)
	{
		//		if (dragon_bone_data.FeetBones.Num() == dragon_input_data.FeetBones.Num())
		if (FBoneReference(dragon_bone_data.FeetBones[i]).IsValidToEvaluate(RequiredBones) == false)
		{
			feet_is_true = false;
		}


		if (automatic_leg_make == false)
		{


			if (FBoneReference(dragon_bone_data.KneeBones[i]).IsValidToEvaluate(RequiredBones) == false)
			{
				feet_is_true = false;
			}

			if (FBoneReference(dragon_bone_data.ThighBones[i]).IsValidToEvaluate(RequiredBones) == false)
			{
				feet_is_true = false;
			}


		}

		//			if (dragon_bone_data.FeetBones.Num() == dragon_input_data.FeetBones.Num())

	}

	//	return false;

		//return (feet_is_true &&dragon_bone_data.Start_Spine.IsValidToEvaluate(RequiredBones) &&dragon_bone_data.Pelvis.IsValidToEvaluate(RequiredBones));

		//return false;

	return (solve_should_fail == false && feet_is_true &&
		dragon_bone_data.Start_Spine.IsValidToEvaluate(RequiredBones) &&
		dragon_bone_data.Pelvis.IsValidToEvaluate(RequiredBones) &&
		RequiredBones.BoneIsChildOf(FBoneReference(dragon_bone_data.Start_Spine).BoneIndex, FBoneReference(dragon_bone_data.Pelvis).BoneIndex));



}


void FAnimNode_DragonFeetSolver::Evaluate_AnyThread(FPoseContext & Output)
{
}


FAnimNode_DragonFeetSolver::FAnimNode_DragonFeetSolver()
{
	is_initialized = false;
}

void FAnimNode_DragonFeetSolver::InitializeBoneReferences(FBoneContainer & RequiredBones)
{

	solve_should_fail = false;


	dragon_bone_data.Start_Spine = FBoneReference(dragon_input_data.Start_Spine);
	dragon_bone_data.Start_Spine.Initialize(RequiredBones);

	dragon_bone_data.Pelvis = FBoneReference(dragon_input_data.Pelvis);
	dragon_bone_data.Pelvis.Initialize(RequiredBones);

	if (!RequiredBones.BoneIsChildOf(FBoneReference(dragon_bone_data.Start_Spine).BoneIndex, FBoneReference(dragon_bone_data.Pelvis).BoneIndex))
	{
		solve_should_fail = true;
	}


	//	if (test_counter < 10)

	if(solve_should_fail == false)
	{

		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, " Test Counter : " + FString::SanitizeFloat(test_counter));


		SavedBoneContainer = &RequiredBones;

		feet_bone_array.Empty();

		feet_transform_array.Empty();

		feet_hit_array.Empty();

		EffectorLocationList.Empty();

		//		feet_mod_transform_array.Empty();

		//		feet_Animated_transform_array.Empty();


		

		FeetRootHeights.Empty();



		/*SPINE CODE START*/

		Total_spine_bones.Empty();

		spine_Feet_pair.Empty();



		if (test_counter < 10)
		{
			spine_Transform_pairs.Empty();
			spine_AnimatedTransform_pairs.Empty();
			spine_hit_pairs.Empty();

			feet_mod_transform_array.Empty();
			feet_Animated_transform_array.Empty();

			knee_Animated_transform_array.Empty();

		}



		solve_should_fail = false;



		Total_spine_bones = BoneArrayMachine(0, dragon_input_data.Start_Spine, dragon_input_data.Pelvis, false);

		Algo::Reverse(Total_spine_bones);


		for (int32 i = 0; i < dragon_input_data.FeetBones.Num(); i++)
		{


			for (int32 j = 0; j < dragon_input_data.FeetBones.Num(); j++)
			{
				if (i != j)
				{
					if (dragon_input_data.FeetBones[i].Feet_Bone_Name == dragon_input_data.FeetBones[j].Feet_Bone_Name)
					{
						solve_should_fail = true;
					}
				}
			}


			BoneArrayMachine_Feet(i, dragon_input_data.FeetBones[i].Feet_Bone_Name, dragon_input_data.FeetBones[i].Knee_Bone_Name, dragon_input_data.FeetBones[i].Thigh_Bone_Name, dragon_input_data.Pelvis, true);
		}


		if (dragon_input_data.Pelvis == dragon_input_data.Start_Spine)
		{
			solve_should_fail = true;
		}

		Spine_Indices.Empty();


		for (int32 i = 0; i < Total_spine_bones.Num(); i++)
		{
			FBoneReference bone_ref = FBoneReference(Total_spine_bones[i]);
			bone_ref.Initialize(RequiredBones);
			Spine_Indices.Add(bone_ref.GetCompactPoseIndex(RequiredBones));

		}



		for (int32 i = 0; i < spine_Feet_pair.Num(); i++)
		{
			if (spine_Feet_pair[i].Associated_Feet.Num() == 0)
				spine_Feet_pair.RemoveAt(i, 1, true);

		}
		spine_Feet_pair = Swap_Spine_Pairs(spine_Feet_pair);


		spine_Feet_pair = Swap_Spine_Pairs(spine_Feet_pair);












		if (test_counter < 10)
		{
			spine_Transform_pairs.AddDefaulted(spine_Feet_pair.Num());
			spine_AnimatedTransform_pairs.AddDefaulted(spine_Feet_pair.Num());
			feet_Animated_transform_array.AddDefaulted(spine_Feet_pair.Num());

			knee_Animated_transform_array.AddDefaulted(dragon_input_data.FeetBones.Num());


			feet_mod_transform_array.AddDefaulted(spine_Feet_pair.Num());

			feet_knee_offset_array.AddDefaulted(spine_Feet_pair.Num());


			feet_Alpha_array.AddDefaulted(spine_Feet_pair.Num());
			spine_hit_pairs.AddDefaulted(spine_Feet_pair.Num());

		}


		FeetRootHeights.AddDefaulted(spine_Feet_pair.Num());

	//	Foot_Knee_Array.SetNum(dragon_input_data.FeetBones.Num());

		for (int32 i = 0; i < spine_Feet_pair.Num(); i++)
		{

			if (test_counter < 10)
			{
				


				spine_hit_pairs[i].RV_Feet_Hits.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());
				spine_hit_pairs[i].RV_FeetFront_Hits.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());
				spine_hit_pairs[i].RV_FeetBack_Hits.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());
				spine_hit_pairs[i].RV_FeetLeft_Hits.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());
				spine_hit_pairs[i].RV_FeetRight_Hits.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());




				spine_hit_pairs[i].RV_Knee_Hits.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());

				spine_hit_pairs[i].RV_FeetBalls_Hits.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());


				spine_Transform_pairs[i].Associated_Feet.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());
				spine_AnimatedTransform_pairs[i].Associated_Feet.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());

				spine_AnimatedTransform_pairs[i].Associated_Knee.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());

				spine_AnimatedTransform_pairs[i].Associated_FeetBalls.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());

				spine_AnimatedTransform_pairs[i].Associated_FeetBalls.AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());


				//////

				feet_mod_transform_array[i].AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());

				feet_knee_offset_array[i].AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());




				feet_Animated_transform_array[i].AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());

				feet_Alpha_array[i].AddDefaulted(spine_Feet_pair[i].Associated_Feet.Num());




			}






			//		spine_Feet_pair[i].Associated_Knees.AddUninitialized(spine_Feet_pair[i].Associated_Feet.Num());

			//		spine_Feet_pair[i].Associated_Thighs.AddUninitialized(spine_Feet_pair[i].Associated_Feet.Num());



			every_foot_dont_have_child = false;


			for (int32 j = 0; j < spine_Feet_pair[i].Associated_Feet.Num(); j++)
			{
				FeetRootHeights[i].Add(0);
				//		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, " Feet Root Heights : " + FString::SanitizeFloat(FeetRootHeights[i][j]));


			


				/*
				if (automatic_leg_make == true)
				{
					FBoneReference Knee_Involved = FBoneReference(owning_skel->GetParentBone(spine_Feet_pair[i].Associated_Feet[j].BoneName));
					Knee_Involved.Initialize(*SavedBoneContainer);

					spine_Feet_pair[i].Associated_Knees[j] = Knee_Involved;
				}
				else
				{

					FBoneReference Knee_Involved = FBoneReference(owning_skel->GetParentBone(spine_Feet_pair[i].Associated_Feet[j].BoneName));
					Knee_Involved.Initialize(*SavedBoneContainer);
					spine_Feet_pair[i].Associated_Knees[j] = Knee_Involved;

				}
				*/

			}

		}


		/*SPINE CODE END*/



		for (int i = 0; i < dragon_input_data.FeetBones.Num(); i++)
		{
			feet_bone_array.Add(dragon_input_data.FeetBones[i].Feet_Bone_Name);
			feet_bone_array[i].Initialize(RequiredBones);

		}





		bool is_swapped = false;

		do
		{
			is_swapped = false;


			for (int32 i = 1; i < feet_bone_array.Num(); i++)
			{
				if (feet_bone_array[i - 1].BoneIndex < feet_bone_array[i].BoneIndex)
				{
					FBoneReference temp = feet_bone_array[i - 1];
					feet_bone_array[i - 1] = feet_bone_array[i];
					feet_bone_array[i] = temp;

					is_swapped = true;
				}
			}

		} while (is_swapped == true);





		feet_transform_array.AddDefaulted(feet_bone_array.Num());

		feet_Alpha_array.AddDefaulted(feet_bone_array.Num());



		feet_hit_array.AddDefaulted(feet_bone_array.Num());

		EffectorLocationList.AddDefaulted(feet_bone_array.Num());

		//	feet_mod_transform_array.AddDefaulted(feet_bone_array.Num());


		//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, " F Transform size :- " + FString::SanitizeFloat(feet_transform_array.Num()));





		if (dragon_input_data.Start_Spine == dragon_input_data.Pelvis)
			solve_should_fail = true;

		if (spine_Feet_pair.Num() > 0)
		{
			if (spine_Feet_pair[0].Spine_Involved.BoneIndex > spine_Feet_pair[spine_Feet_pair.Num() - 1].Spine_Involved.BoneIndex)
				solve_should_fail = true;
		}

		
		/*
		dragon_bone_data.Start_Spine = FBoneReference(dragon_input_data.Start_Spine);
		dragon_bone_data.Start_Spine.Initialize(RequiredBones);

		dragon_bone_data.Pelvis = FBoneReference(dragon_input_data.Pelvis);
		dragon_bone_data.Pelvis.Initialize(RequiredBones);
		*/

		dragon_bone_data.FeetBones.Empty();
		dragon_bone_data.KneeBones.Empty();
		dragon_bone_data.ThighBones.Empty();


		for (int i = 0; i < dragon_input_data.FeetBones.Num(); i++)
		{

			dragon_bone_data.FeetBones.Add(FBoneReference(dragon_input_data.FeetBones[i].Feet_Bone_Name));
			dragon_bone_data.FeetBones[i].Initialize(RequiredBones);

			if (automatic_leg_make == false)
			{
				dragon_bone_data.KneeBones.Add(FBoneReference(dragon_input_data.FeetBones[i].Knee_Bone_Name));
				dragon_bone_data.KneeBones[i].Initialize(RequiredBones);


				dragon_bone_data.ThighBones.Add(FBoneReference(dragon_input_data.FeetBones[i].Thigh_Bone_Name));
				dragon_bone_data.ThighBones[i].Initialize(RequiredBones);
			}


		}



		is_initialized = true;
	}


}






FName FAnimNode_DragonFeetSolver::GetChildBone(FName BoneName, USkeletalMeshComponent* skel_mesh)
{

	FName child_bone = skel_mesh->GetBoneName(skel_mesh->GetBoneIndex(BoneName) + 1);

	return child_bone;

}



/*SPINE FUNCS*/





TArray<FDragonData_SpineFeetPair> FAnimNode_DragonFeetSolver::Swap_Spine_Pairs(TArray<FDragonData_SpineFeetPair> test_list)
{



	bool is_swapped = false;

	do
	{
		is_swapped = false;

		for (int32 j = 1; j < test_list.Num(); j++)
		{

			for (int32 i = 1; i < test_list[j].Associated_Feet.Num(); i++)
			{
				if (test_list[j].Associated_Feet[i - 1].BoneIndex < test_list[j].Associated_Feet[i].BoneIndex)
				{
					FBoneReference temp = test_list[j].Associated_Feet[i - 1];
					test_list[j].Associated_Feet[i - 1] = test_list[j].Associated_Feet[i];
					test_list[j].Associated_Feet[i] = temp;

					if (automatic_leg_make == false)
					{

						FBoneReference tempKnees = test_list[j].Associated_Knees[i - 1];
						test_list[j].Associated_Knees[i - 1] = test_list[j].Associated_Knees[i];
						test_list[j].Associated_Knees[i] = tempKnees;


						FBoneReference tempThighs = test_list[j].Associated_Thighs[i - 1];
						test_list[j].Associated_Thighs[i - 1] = test_list[j].Associated_Thighs[i];
						test_list[j].Associated_Thighs[i] = tempThighs;
					}

					int temp_knee_order = test_list[j].order_index[i - 1];
					test_list[j].order_index[i - 1] = test_list[j].order_index[i];
					test_list[j].order_index[i] = temp_knee_order;

					is_swapped = true;
				}
			}
		}

	} while (is_swapped == true);


	return test_list;

}




FVector FAnimNode_DragonFeetSolver::AnimLocLerp(FVector start_pos, FVector end_pos, float delta_seconds)
{
	FVector start_pos_val = start_pos;
	FVector end_pos_val = end_pos;
	FVector vector_difference = (start_pos_val - end_pos_val) / FMath::Clamp<float>(6 - Location_Lerp_Speed, 1, 6);


	if (loc_interp_type == EInterpoLocation_Type_Plugin::ENUM_DivisiveLoc_Interp)
		return start_pos_val - vector_difference;
	else
	{
		return UKismetMathLibrary::VLerp(start_pos_val, end_pos_val, delta_seconds);
	}




}

FRotator FAnimNode_DragonFeetSolver::AnimRotLerp(FRotator start_pos, FRotator end_pos, float delta_seconds)
{
	return FRotator();
}

FQuat FAnimNode_DragonFeetSolver::AnimRotSLerp(FQuat start_pos, FQuat end_pos, float delta_seconds)
{

	FQuat start_pos_val = start_pos;
	FQuat end_pos_val = end_pos;
	//	FQuat rotation_difference = (start_pos_val - end_pos_val)*(1/5);


	FQuat rotation_difference = ((start_pos_val.Inverse()*(end_pos_val * 0.5f).GetNormalized() )).GetNormalized();



	//GEngine->AddOnScreenDebugMessage(-1, 5.01f, FColor::Red, "Foot Rot " + FString::SanitizeFloat(rotation_difference.GetAngle()) );



	if (rot_interp_type == EInterpoRotation_Type_Plugin::ENUM_DivisiveRot_Interp)
	{
		FQuat result = start_pos_val * (rotation_difference);



		return result;
	}
	else
	{
		return FQuat::Slerp(start_pos_val, end_pos_val, delta_seconds);
	}

}



/*
TArray<FName> FAnimNode_DragonFeetSolver::BoneArrayMachine(int32 index, FName starting, FName ending, bool is_foot)
{

	TArray<FName> spine_bones;

	int iteration_count = 0;

	spine_bones.Add(starting);

	if (is_foot == false)
	{
		FDragonData_SpineFeetPair data = FDragonData_SpineFeetPair();
		data.Spine_Involved = FBoneReference(starting);
		data.Spine_Involved.Initialize(*SavedBoneContainer);
		spine_Feet_pair.Add(data);



	}


	bool finish = false;

	do
	{


		if (is_foot)
		{
			if (Check_Loop_Exist(dragon_input_data.FeetBones[index].Feet_Yaw_Offset, dragon_input_data.FeetBones[index].Feet_Heights, starting,FName(""),FName(""), spine_bones[spine_bones.Num() - 1], Total_spine_bones))
			{
				return spine_bones;
			}
		}

		if (is_foot == false)
		{
			//			GEngine->AddOnScreenDebugMessage(-1, 5.01f, FColor::Red, "Spine INIT == " + spine_bones[iteration_count].ToString());
		}

		iteration_count++;
		spine_bones.Add(owning_skel->GetParentBone(spine_bones[iteration_count - 1]));


		if (is_foot == false)
		{

			FDragonData_SpineFeetPair data = FDragonData_SpineFeetPair();
			data.Spine_Involved = FBoneReference(spine_bones[spine_bones.Num() - 1]);
			data.Spine_Involved.Initialize(*SavedBoneContainer);
			spine_Feet_pair.Add(data);


		}


		if (spine_bones[spine_bones.Num() - 1] == ending  && is_foot == false)
		{
			//			GEngine->AddOnScreenDebugMessage(-1, 5.01f, FColor::Red, "Spine INIT == " + spine_bones[spine_bones.Num() - 1].ToString()+" and Ending = "+ending.ToString());


			return spine_bones;
		}

	} while (iteration_count < 50 && finish == false);


	return spine_bones;


}

*/







TArray<FName> FAnimNode_DragonFeetSolver::BoneArrayMachine_Feet(int32 index, FName starting, FName knee, FName thigh, FName ending, bool is_foot)
{

	TArray<FName> spine_bones;

	int iteration_count = 0;

	spine_bones.Add(starting);

	if (is_foot == false)
	{
		FDragonData_SpineFeetPair data = FDragonData_SpineFeetPair();
		data.Spine_Involved = FBoneReference(starting);
		data.Spine_Involved.Initialize(*SavedBoneContainer);
		spine_Feet_pair.Add(data);



	}


	bool finish = false;

	do
	{


		if (is_foot)
		{
			if (Check_Loop_Exist(index,dragon_input_data.FeetBones[index].Feet_Trace_Offset, dragon_input_data.FeetBones[index].Knee_Direction_Offset, dragon_input_data.FeetBones[index].Feet_Rotation_Limit, dragon_input_data.FeetBones[index].Feet_Rotation_Offset, dragon_input_data.FeetBones[index].Feet_Heights, starting, knee, thigh, spine_bones[spine_bones.Num() - 1], Total_spine_bones))
			{
				return spine_bones;
			}
		}

		if (is_foot == false)
		{
			//			GEngine->AddOnScreenDebugMessage(-1, 5.01f, FColor::Red, "Spine INIT == " + spine_bones[iteration_count].ToString());
		}

		iteration_count++;
		spine_bones.Add(owning_skel->GetParentBone(spine_bones[iteration_count - 1]));


		if (is_foot == false)
		{

			FDragonData_SpineFeetPair data = FDragonData_SpineFeetPair();
			data.Spine_Involved = FBoneReference(spine_bones[spine_bones.Num() - 1]);
			data.Spine_Involved.Initialize(*SavedBoneContainer);
			spine_Feet_pair.Add(data);


		}


		if (spine_bones[spine_bones.Num() - 1] == ending && is_foot == false)
		{

			return spine_bones;
		}

	} while (iteration_count < 50 && finish == false);


	return spine_bones;


}







TArray<FName> FAnimNode_DragonFeetSolver::BoneArrayMachine(int32 index, FName starting, FName ending, bool is_foot)
{

	TArray<FName> spine_bones;

	int iteration_count = 0;

	spine_bones.Add(starting);

	if (is_foot == false)
	{
		FDragonData_SpineFeetPair data = FDragonData_SpineFeetPair();
		data.Spine_Involved = FBoneReference(starting);
		data.Spine_Involved.Initialize(*SavedBoneContainer);
		spine_Feet_pair.Add(data);



	}


	bool finish = false;

	do
	{


		if (is_foot)
		{
			if (Check_Loop_Exist(index,dragon_input_data.FeetBones[index].Feet_Trace_Offset, dragon_input_data.FeetBones[index].Knee_Direction_Offset, dragon_input_data.FeetBones[index].Feet_Rotation_Limit, dragon_input_data.FeetBones[index].Feet_Rotation_Offset, dragon_input_data.FeetBones[index].Feet_Heights, starting, FName(""), FName(""), spine_bones[spine_bones.Num() - 1], Total_spine_bones))
			{
				return spine_bones;
			}
		}

		if (is_foot == false)
		{
			//			GEngine->AddOnScreenDebugMessage(-1, 5.01f, FColor::Red, "Spine INIT == " + spine_bones[iteration_count].ToString());
		}

		iteration_count++;
		spine_bones.Add(owning_skel->GetParentBone(spine_bones[iteration_count - 1]));


		if (is_foot == false)
		{

			FDragonData_SpineFeetPair data = FDragonData_SpineFeetPair();
			data.Spine_Involved = FBoneReference(spine_bones[spine_bones.Num() - 1]);
			data.Spine_Involved.Initialize(*SavedBoneContainer);
			spine_Feet_pair.Add(data);


		}


		if (spine_bones[spine_bones.Num() - 1] == ending && is_foot == false)
		{

			return spine_bones;
		}

	} while (iteration_count < 50 && finish == false);


	return spine_bones;


}







bool FAnimNode_DragonFeetSolver::Check_Loop_Exist(int order_index,FVector feet_trace_offset, FVector knee_direction_offset, float feet_rotation_limit, FRotator feet_rotation_offset, float feet_height, FName start_bone, FName knee_bone, FName thigh_bone, FName input_bone, TArray<FName> total_spine_bones)
{

	for (int32 i = 0; i < total_spine_bones.Num(); i++)
	{
		if (input_bone.ToString().TrimStartAndEnd().Equals(total_spine_bones[i].ToString().TrimStartAndEnd()))
		{
			if (spine_Feet_pair.Num() > i) // @tikwon
			{
				FDragonData_SpineFeetPair data = FDragonData_SpineFeetPair();
				data.Spine_Involved = FBoneReference(total_spine_bones[i]);
				data.Spine_Involved.Initialize(*SavedBoneContainer);

				FBoneReference foot_bone = FBoneReference(start_bone);
				foot_bone.Initialize(*SavedBoneContainer);
				data.Associated_Feet.Add(foot_bone);

				spine_Feet_pair[i].Spine_Involved = data.Spine_Involved;
				spine_Feet_pair[i].Associated_Feet.Add(foot_bone);
				spine_Feet_pair[i].feet_rotation_offset.Add(feet_rotation_offset);

				spine_Feet_pair[i].knee_direction_offset.Add(knee_direction_offset);
				spine_Feet_pair[i].order_index.Add(order_index);

				spine_Feet_pair[i].feet_trace_offset.Add(feet_trace_offset);


				spine_Feet_pair[i].Feet_Heights.Add(feet_height);
				spine_Feet_pair[i].feet_rotation_limit.Add(feet_rotation_limit);


				if (automatic_leg_make == false)
				{

					FBoneReference knee_bone_ref = FBoneReference(knee_bone);
					knee_bone_ref.Initialize(*SavedBoneContainer);
					spine_Feet_pair[i].Associated_Knees.Add(knee_bone_ref);


					FBoneReference thigh_bone_ref = FBoneReference(thigh_bone);
					thigh_bone_ref.Initialize(*SavedBoneContainer);
					spine_Feet_pair[i].Associated_Thighs.Add(thigh_bone_ref);





				}



				if (i < 0)
				{
					//		FBoneReference bone_edit = FBoneReference(owning_skel->GetParentBone(spine_Feet_pair[i - 1].Spine_Involved.BoneName));
					//		bone_edit.Initialize(*SavedBoneContainer);
					//	spine_Feet_pair[i - 1].Spine_Involved = bone_edit;
				}

				return true;
			}
		}
	}

	return false;
}




/*SPINE FUNCS END*/






FCollisionQueryParams FAnimNode_DragonFeetSolver::getDefaultColliParams(FName name, AActor *me)
{

	const FName TraceTag(name);

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, me);
	RV_TraceParams.bTraceComplex = true;
	//	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.TraceTag = TraceTag;

	//	me->GetWorld()->DebugDrawTraceTag = TraceTag;


	return RV_TraceParams;
}


void FAnimNode_DragonFeetSolver::line_trace_func(USkeletalMeshComponent *skelmesh, FVector startpoint, FVector endpoint, FHitResult RV_Ragdoll_Hit, FName bone_text, FName trace_tag, FHitResult& Output, const FLinearColor& Fdebug_color)
{

	/*
	skelmesh->GetWorld()->LineTraceSingleByChannel(RV_Ragdoll_Hit,        //result
	startpoint,    //start
	endpoint, //end
	ECC_WorldStatic, //collision channel
	getDefaultParams(trace_tag, skelmesh->GetOwner()));

	TArray<AActor*> selfActor;
	//	selfActor.Add(skelmesh->GetOwner());
	*/


	/*BOX
	FCollisionShape shape = FCollisionShape();

	shape.SetBox(FVector(10,10,5));

	skelmesh->GetWorld()->SweepSingleByChannel(RV_Ragdoll_Hit,        //result
	startpoint,    //start
	endpoint, //end
	FRotator(0,0,0).Quaternion(),
	ECC_WorldStatic, //collision channel
	shape,
	getDefaultParams(trace_tag, skelmesh->GetOwner()));
	*/



	TArray<AActor*> ignoreActors;

	if (owning_skel->GetOwner())
	{
		ignoreActors.Add(owning_skel->GetOwner());

		//float trace_radius_cs = owning_skel->GetComponentToWorld().GetScale3D().Z*Trace_Radius;


		float owner_scale = 1;

		FVector character_direction_vector = character_direction_vector_CS;

		if (owning_skel->GetOwner())
		{
			owner_scale = owning_skel->GetComponentToWorld().GetScale3D().Z;


			const FVector char_up = character_direction_vector_CS;

			character_direction_vector = UKismetMathLibrary::TransformDirection(owning_skel->GetComponentToWorld(), char_up);

		}


		float Scaled_Trace_Radius = Trace_Radius * owner_scale;











	//	FVector up_dir_ws = owning_skel->GetComponentToWorld().TransformVector(character_direction_vector_CS);

		FHitResult RV_Anti_Checker;

		if(Use_Anti_Channel)
		UKismetSystemLibrary::LineTraceSingle(owning_skel->GetOwner(), startpoint - character_direction_vector * line_trace_upper_height * scale_mode, startpoint, Anti_Trace_Channel, true, ignoreActors, EDrawDebugTrace::None, RV_Anti_Checker, true, Fdebug_color);





		if (RV_Anti_Checker.bBlockingHit && Use_Anti_Channel)
		{
			//	if (RV_Anti_Checker.GetComponent())
			{
				ECollisionChannel channel_type = UEngineTypes::ConvertToCollisionChannel(Anti_Trace_Channel.GetValue());

				ECollisionResponse response_channel = ECollisionResponse::ECR_Ignore;

				if (RV_Anti_Checker.bBlockingHit && RV_Anti_Checker.GetComponent())
					response_channel = RV_Anti_Checker.GetComponent()->GetCollisionResponseToChannel(channel_type);


			//	if (RV_Ragdoll_Hit.bBlockingHit && RV_Ragdoll_Hit.GetComponent())
			//		response_channel = RV_Ragdoll_Hit.GetComponent()->GetCollisionResponseToChannel(channel_type);





				if (response_channel == ECollisionResponse::ECR_Block)
				{

				//	FVector New_StartPoint = startpoint - character_direction_vector * line_trace_upper_height * scale_mode;

					FVector New_StartPoint = (RV_Anti_Checker.ImpactPoint - character_direction_vector * 5);


					if (trace_type == EIKTrace_Type_Plugin::ENUM_LineTrace_Type || Trace_Radius < 0.2f)
						UKismetSystemLibrary::LineTraceSingle(owning_skel->GetOwner(), New_StartPoint, endpoint, Trace_Channel, true, ignoreActors, EDrawDebugTrace::None, RV_Ragdoll_Hit, true, Fdebug_color);
					else
						if (trace_type == EIKTrace_Type_Plugin::ENUM_SphereTrace_Type)
						{//(startpoint-endpoint).Size()
							UKismetSystemLibrary::SphereTraceSingle(owning_skel->GetOwner(), New_StartPoint + character_direction_vector * Scaled_Trace_Radius, endpoint + character_direction_vector * Scaled_Trace_Radius, Scaled_Trace_Radius, Trace_Channel, true, ignoreActors, EDrawDebugTrace::None, RV_Ragdoll_Hit, true, Fdebug_color);
						}
						else
							if (trace_type == EIKTrace_Type_Plugin::ENUM_BoxTrace_Type)
								UKismetSystemLibrary::BoxTraceSingle(owning_skel->GetOwner(), New_StartPoint, endpoint, FVector(1, 1, 0)*Scaled_Trace_Radius, FRotator(0, 0, 0), Trace_Channel, true, ignoreActors, EDrawDebugTrace::None, RV_Ragdoll_Hit, true, Fdebug_color);


				//		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, " Anti-Channel FOOT Detected ");
				}
			}
		}
		else
		{

			if (trace_type == EIKTrace_Type_Plugin::ENUM_LineTrace_Type || Trace_Radius < 0.2f)
				UKismetSystemLibrary::LineTraceSingle(owning_skel->GetOwner(), startpoint, endpoint, Trace_Channel, true, ignoreActors, EDrawDebugTrace::None, RV_Ragdoll_Hit, true, Fdebug_color);
			else
				if (trace_type == EIKTrace_Type_Plugin::ENUM_SphereTrace_Type)
				{//(startpoint-endpoint).Size()
					UKismetSystemLibrary::SphereTraceSingle(owning_skel->GetOwner(), startpoint + character_direction_vector * Scaled_Trace_Radius, endpoint + character_direction_vector * Scaled_Trace_Radius, Scaled_Trace_Radius, Trace_Channel, true, ignoreActors, EDrawDebugTrace::None, RV_Ragdoll_Hit, true, Fdebug_color);
				}
				else
					if (trace_type == EIKTrace_Type_Plugin::ENUM_BoxTrace_Type)
						UKismetSystemLibrary::BoxTraceSingle(owning_skel->GetOwner(), startpoint, endpoint, FVector(1, 1, 0)*Scaled_Trace_Radius, FRotator(0, 0, 0), Trace_Channel, true, ignoreActors, EDrawDebugTrace::None, RV_Ragdoll_Hit, true, Fdebug_color);



		}




	}




	TraceStartList.Add(startpoint);
	TraceEndList.Add(endpoint);


















	//BoxTraceSingle(UObject* WorldContextObject, const FVector Start, const FVector End, const FVector HalfSize, const FRotator Orientation, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
	//SphereTraceSingle(UObject* WorldContextObject, const FVector Start, const FVector End, float Radius, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)



	//UKismetSystemLibrary::BoxTraceSingle(owning_skel->GetOwner()->GetWorld(), startpoint, endpoint, FVector(15,15,15), FRotator(0,0,0), Trace_Channel, true, ignoreActors, EDrawDebugTrace::ForOneFrame, RV_Ragdoll_Hit, true, FLinearColor::White, FLinearColor::Black, 0.01f);


	//UKismetSystemLibrary::SphereTraceSingle(owning_skel->GetOwner()->GetWorld(), startpoint, endpoint,25, Trace_Channel, true, ignoreActors, EDrawDebugTrace::ForOneFrame, RV_Ragdoll_Hit, true, FLinearColor::White, FLinearColor::Black, 0.01f);

	/*
	skelmesh->GetWorld()->LineTraceSingleByChannel(RV_Ragdoll_Hit,        //result
	startpoint,    //start
	endpoint, //end
	ECC_WorldStatic, //collision channel
	getDefaultParams(trace_tag, skelmesh->GetOwner()));
	*/

	Output = RV_Ragdoll_Hit;

}




FRotator FAnimNode_DragonFeetSolver::BoneRelativeConversion(FRotator feet_data, FCompactPoseBoneIndex ModifyBoneIndex, FRotator target_rotation, const FBoneContainer & BoneContainer, FCSPose<FCompactPose>& MeshBases)
{


	//FCompactPoseBoneIndex CompactPoseBoneToModify = BoneToModify.GetCompactPoseIndex(BoneContainer);
	FTransform NewBoneTM = MeshBases.GetComponentSpaceTransform(ModifyBoneIndex);
	//	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();
	FTransform ComponentTransform = owning_skel->GetComponentTransform();

	const FTransform C_ComponentTransform = owning_skel->GetComponentTransform();


	// Convert to Bone Space.
	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, MeshBases, NewBoneTM, ModifyBoneIndex, EBoneControlSpace::BCS_WorldSpace);

	float temp_yaw = ComponentTransform.Rotator().Yaw;

	FQuat actor_rotation_world(0, 0, 0, 0);

	if (owning_skel->GetOwner() != nullptr)
	{
		actor_rotation_world = owning_skel->GetOwner()->GetActorRotation().Quaternion().Inverse()*FRotator(NewBoneTM.GetRotation()).Quaternion();

		FRotator make_rot = NewBoneTM.Rotator();


		//		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, MeshBases, EndBoneCSTransform, ModifyBoneIndex, EBoneControlSpace::BCS_WorldSpace);

		//		make_rot.Yaw = EndBoneCSTransform.Rotator().Yaw;
//		make_rot.Yaw = FRotator(actor_rotation_world).Yaw + feet_data;


//		make_rot = FRotator(actor_rotation_world) + FRotator(45,45,45);


		//		make_rot.Yaw = 0;

		//New Mod
		//make_rot.Yaw = NewBoneTM.Rotator().Yaw;

		NewBoneTM.SetRotation(make_rot.Quaternion());

	}

	const FQuat BoneQuat(target_rotation);

	NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());

	// Convert back to Component Space.
	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, MeshBases, NewBoneTM, ModifyBoneIndex, EBoneControlSpace::BCS_WorldSpace);


	FTransform EndBoneCSTransform = MeshBases.GetComponentSpaceTransform(ModifyBoneIndex);

	//	NewBoneTM.SetRotation(FRotator(NewBoneTM.Rotator().Pitch, EndBoneCSTransform.Rotator().Yaw, NewBoneTM.Rotator().Roll).Quaternion());

	return NewBoneTM.Rotator();


}

FRotator FAnimNode_DragonFeetSolver::BoneInverseConversion(FCompactPoseBoneIndex ModifyBoneIndex, FRotator target_rotation, const FBoneContainer & BoneContainer, FCSPose<FCompactPose>& MeshBases)
{

	FTransform NewBoneTM = MeshBases.GetComponentSpaceTransform(ModifyBoneIndex);
	//	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();
	FTransform ComponentTransform = owning_skel->GetComponentTransform();
	const FTransform C_ComponentTransform = owning_skel->GetComponentTransform();


	// Convert to Bone Space.ConvertBoneSpaceTransformToCS
	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, MeshBases, NewBoneTM, ModifyBoneIndex, EBoneControlSpace::BCS_WorldSpace);
	float temp_yaw = ComponentTransform.Rotator().Yaw;
	FQuat actor_rotation_world(0, 0, 0, 0);

	if (owning_skel->GetOwner() != nullptr)
	{
		actor_rotation_world = owning_skel->GetOwner()->GetActorRotation().Quaternion()*FRotator(NewBoneTM.GetRotation()).Quaternion();
		FRotator make_rot = NewBoneTM.Rotator();
		make_rot.Yaw = FRotator(actor_rotation_world).Yaw;
		NewBoneTM.SetRotation(make_rot.Quaternion());
	}
	const FQuat BoneQuat(target_rotation);
	NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());
	// Convert back to Component Space.
	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, MeshBases, NewBoneTM, ModifyBoneIndex, EBoneControlSpace::BCS_WorldSpace);
	return NewBoneTM.Rotator();

}





FVector FAnimNode_DragonFeetSolver::GetCurrentLocation(FCSPose<FCompactPose>& MeshBases, const FCompactPoseBoneIndex& BoneIndex)
{
	return MeshBases.GetComponentSpaceTransform(BoneIndex).GetLocation();
}


