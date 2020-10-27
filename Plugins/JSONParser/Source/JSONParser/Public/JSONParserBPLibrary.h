// Copyright 2017-2020 David Romanski(Socke). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Base64.h"
#include "JSONParserBPLibrary.generated.h"



UENUM(BlueprintType)
enum class EJsonDataType : uint8
{
	E_String 	UMETA(DisplayName = "String"),
	E_Number	UMETA(DisplayName = "Number"),
	E_Bool		UMETA(DisplayName = "Bool"),
	E_Object 	UMETA(DisplayName = "Object")

};

UENUM(BlueprintType)
enum class EJSONDirectoryType : uint8
{
	E_gd	UMETA(DisplayName = "Game directory"),
	E_ad 	UMETA(DisplayName = "Absolute directory")
};

USTRUCT(BlueprintType)
struct FSJsonObject
{
	GENERATED_USTRUCT_BODY()

		TSharedPtr<FJsonObject>			 ue4Object;
	TArray< TSharedPtr<FJsonValue> > ue4Array;

	TArray<FSJsonObject>	asObjectArray;
	TArray<bool>			asBoolArray;
	TArray<float>			asNumberArray;
	TArray<FString>			asStringArray;

	FString jsonPBefore;
	FString jsonPAfter;
};





UCLASS()
class UJSONParserBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


public:

	//create tmap
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Make")
		static TMap<FString, FString> MakeMapString(FString Key, FString Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Make")
		static TMap<FString, float> MakeMapNumber(FString Key, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Make")
		static TMap<FString, bool> MakeMapBool(FString Key, bool Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Make")
		static TMap<FString, FSJsonObject> MakeMapObject(FString Key, FSJsonObject Value);

	//add elements to tmap
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Add")
		static TMap<FString, FString> AddMapString(TMap<FString, FString> map, FString Key, FString Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Add")
		static TMap<FString, float> AddMapNumber(TMap<FString, float> map, FString Key, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Add")
		static TMap<FString, bool> AddMapBool(TMap<FString, bool> map, FString Key, bool Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Add")
		static TMap<FString, FSJsonObject> AddMapObject(TMap<FString, FSJsonObject> map, FString Key, FSJsonObject Value);

	//change values in tmap
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Change")
		static TMap<FString, FString> ChangeMapString(TMap<FString, FString> map, FString Key, FString Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Change")
		static TMap<FString, float> ChangeMapNumber(TMap<FString, float> map, FString Key, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Change")
		static TMap<FString, bool> ChangeMapBool(TMap<FString, bool> map, FString Key, bool Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Change")
		static TMap<FString, FSJsonObject> ChangeMapObject(TMap<FString, FSJsonObject> map, FString Key, FSJsonObject Value);

	//create variables
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Make")
		static FString MakeString(FString Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Make")
		static float MakeNumber(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JSON|Make")
		static bool MakeBool(bool Value);

	//json stuff
	UFUNCTION(BlueprintCallable, Category = "JSON", meta = (AutoCreateRefTerm = "ObjectMap,BoolMap,NumberMap,StringMap,ObjectArray,BoolArray,NumberArray,StringArray"))
		static struct FSJsonObject MakeJson_Object(TMap<FString, FSJsonObject> ObjectMap, TMap<FString, bool> BoolMap, TMap<FString, float> NumberMap, TMap<FString, FString> StringMap, TArray<FSJsonObject> ObjectArray, TArray<bool> BoolArray, TArray<float> NumberArray, TArray<FString> StringArray);


	/**
	*Create a JSON String from a JSON Struct.
	*@param mainObject JSON Struct
	*@param removeLineBreaks removes \r, \n and \t
	*@param addPadding If there is truncated JSON padding in the JSON object, it is appended again.
	*/
	UFUNCTION(BlueprintCallable, Category = "JSON")
		static FString JsonObjectToString(struct FSJsonObject mainObject, bool removeLineBreaks = true, bool addPadding = false);


	/**
	*Create a JSON Struct from a JSON String.
	*@param JSON JSON String
	*@param removeAndSavePadding If it is JSON with padding, everything before { and after } is truncated and stored in the JSON object that is returned.
	*/
	UFUNCTION(BlueprintCallable, Category = "JSON")
		static FSJsonObject JsonStringToObject(FString JSON, bool removeAndSavePadding);

	UFUNCTION(BlueprintCallable, Category = "JSON")
		static void BreakJson_Object(const FSJsonObject JSONObject, TMap<FString, FSJsonObject>& ObjectMap, TMap<FString, bool>& BoolMap, TMap<FString, float>& NumberMap, TMap<FString, FString>& StringMap, TArray<FSJsonObject>& ObjectArray, TArray<bool>& BoolArray, TArray<float>& NumberArray, TArray<FString>& StringArray);

	UFUNCTION(BlueprintCallable, Category = "JSON")
		static void BreakMixedJSONArray(const int32 index, const FSJsonObject JSONObject, int32& lastIndex, EJsonDataType& type, FSJsonObject& Object, bool& Bool, float& Number, FString& String);

	UFUNCTION(BlueprintCallable, Category = "JSON")
		static struct FSJsonObject MakeMixedJSONArray(const FSJsonObject oldJSONMixedArray, EJsonDataType ChooseOneType, FSJsonObject Object, bool Bool, float Number, FString String);

	//File helper
	UFUNCTION(BlueprintCallable, Category = "JSON|File")
		static void fileToBase64String(EJSONDirectoryType directoryType, FString filePath, FString& base64String, FString& fileName);
	UFUNCTION(BlueprintCallable, Category = "JSON|File")
		static bool base64StringToFile(EJSONDirectoryType directoryType, FString base64String, FString filePath);
	UFUNCTION(BlueprintCallable, Category = "JSON|File")
		static void readStringFromFile(EJSONDirectoryType directoryType, FString filePath, FString& JSON_String);
	UFUNCTION(BlueprintCallable, Category = "JSON|File")
		static bool writeStringToFile(EJSONDirectoryType directoryType, FString JSON_String, FString filePath);

private:
	static struct FSJsonObject generateFSJsonObjectWithParameter(TMap<FString, FSJsonObject> ObjectMap, TMap<FString, bool> BoolMap, TMap<FString, float> NumberMap, TMap<FString, FString> StringMap, TArray<FSJsonObject> ObjectArray, TArray<bool> BoolArray, TArray<float> NumberArray, TArray<FString> StringArray);
	static struct FSJsonObject generateFSJsonObject(struct FSJsonObject object);
	static struct FSJsonObject generateFSJsonObjectAndArray(FSJsonObject JSONObject, TArray< TSharedPtr<FJsonValue> > ue4Array);
};