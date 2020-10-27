// Copyright 2017-2020 David Romanski(Socke). All Rights Reserved.

#include "JSONParserBPLibrary.h"
#include "JSONParser.h"

UJSONParserBPLibrary::UJSONParserBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//create tmap
TMap<FString, FString> UJSONParserBPLibrary::MakeMapString(FString Key, FString Value) {

	TMap<FString, FString> map;
	map.Add(Key, Value);
	return map;
}

TMap<FString, float> UJSONParserBPLibrary::MakeMapNumber(FString Key, float Value) {

	TMap<FString, float> map;
	map.Add(Key, Value);
	return map;
}

TMap<FString, bool> UJSONParserBPLibrary::MakeMapBool(FString Key, bool Value) {

	TMap<FString, bool> map;
	map.Add(Key, Value);
	return map;
}

TMap<FString, FSJsonObject> UJSONParserBPLibrary::MakeMapObject(FString Key, FSJsonObject Value) {

	TMap<FString, FSJsonObject> map;
	map.Add(Key, Value);
	return map;
}

//add elements to tmap
TMap<FString, FString> UJSONParserBPLibrary::AddMapString(TMap<FString, FString> map, FString Key, FString Value) {
	map.Add(Key, Value);
	return map;
}

TMap<FString, float> UJSONParserBPLibrary::AddMapNumber(TMap<FString, float> map, FString Key, float Value) {
	map.Add(Key, Value);
	return map;
}

TMap<FString, bool> UJSONParserBPLibrary::AddMapBool(TMap<FString, bool> map, FString Key, bool Value) {
	map.Add(Key, Value);
	return map;
}

TMap<FString, FSJsonObject> UJSONParserBPLibrary::AddMapObject(TMap<FString, FSJsonObject> map, FString Key, FSJsonObject Value) {
	map.Add(Key, Value);
	return map;
}

//change values in tmap
TMap<FString, FString> UJSONParserBPLibrary::ChangeMapString(TMap<FString, FString> map, FString Key, FString Value) {

	map.Remove(Key);
	map.Add(Key, Value);
	return map;
}

TMap<FString, float> UJSONParserBPLibrary::ChangeMapNumber(TMap<FString, float> map, FString Key, float Value) {

	map.Remove(Key);
	map.Add(Key, Value);
	return map;
}

TMap<FString, bool> UJSONParserBPLibrary::ChangeMapBool(TMap<FString, bool> map, FString Key, bool Value) {

	map.Remove(Key);
	map.Add(Key, Value);
	return map;
}

TMap<FString, FSJsonObject> UJSONParserBPLibrary::ChangeMapObject(TMap<FString, FSJsonObject> map, FString Key, FSJsonObject Value) {

	map.Remove(Key);
	map.Add(Key, Value);
	return map;
}

//json stuff
FString UJSONParserBPLibrary::MakeString(FString Value) {
	return Value;
}

float UJSONParserBPLibrary::MakeNumber(float Value) {
	return Value;
}

bool UJSONParserBPLibrary::MakeBool(bool Value) {
	return Value;
}




struct FSJsonObject UJSONParserBPLibrary::MakeJson_Object(TMap<FString, FSJsonObject> ObjectMap, TMap<FString, bool> BoolMap, TMap<FString, float> NumberMap, TMap<FString, FString> StringMap, TArray<FSJsonObject> ObjectArray, TArray<bool> BoolArray, TArray<float> NumberArray, TArray<FString> StringArray) {
	FSJsonObject mainObject = generateFSJsonObjectWithParameter(ObjectMap, BoolMap, NumberMap, StringMap, ObjectArray, BoolArray, NumberArray, StringArray);
	return mainObject;
}


struct FSJsonObject UJSONParserBPLibrary::generateFSJsonObjectWithParameter(TMap<FString, FSJsonObject> ObjectMap, TMap<FString, bool> BoolMap, TMap<FString, float> NumberMap, TMap<FString, FString> StringMap, TArray<FSJsonObject> ObjectArray, TArray<bool> BoolArray, TArray<float> NumberArray, TArray<FString> StringArray) {

	FSJsonObject mainObject;
	mainObject.ue4Object = MakeShareable(new FJsonObject());

	//maps
	for (auto& element : ObjectMap) {
		if (element.Value.ue4Object.IsValid()) {
			mainObject.ue4Object->SetObjectField(element.Key, element.Value.ue4Object);
			if (element.Value.ue4Array.Num() > 0) {
				mainObject.ue4Object->SetArrayField(element.Key, element.Value.ue4Array);
			}
		}
		else {
			//array as value like {"answers": ["asd","zxc","qwe","qaz","wsx"]}
			if (element.Value.asStringArray.Num() > 0) {
				TArray< TSharedPtr<FJsonValue>> a;
				for (int32 i = 0; i < element.Value.asStringArray.Num(); i++) {
					a.Add(MakeShareable(new FJsonValueString(element.Value.asStringArray[i])));
				}
				mainObject.ue4Object->SetArrayField(element.Key, a);
			}

			if (element.Value.asObjectArray.Num() > 0) {
				TArray< TSharedPtr<FJsonValue>> a;
				for (int32 i = 0; i < element.Value.asObjectArray.Num(); i++) {
					a.Add(MakeShareable(new FJsonValueObject(element.Value.asObjectArray[i].ue4Object)));
				}
				mainObject.ue4Object->SetArrayField(element.Key, a);
			}

			if (element.Value.asBoolArray.Num() > 0) {
				TArray< TSharedPtr<FJsonValue>> a;
				for (int32 i = 0; i < element.Value.asBoolArray.Num(); i++) {
					a.Add(MakeShareable(new FJsonValueBoolean(element.Value.asBoolArray[i])));
				}
				mainObject.ue4Object->SetArrayField(element.Key, a);
			}

			if (element.Value.asNumberArray.Num() > 0) {
				TArray< TSharedPtr<FJsonValue>> a;
				for (int32 i = 0; i < element.Value.asNumberArray.Num(); i++) {
					a.Add(MakeShareable(new FJsonValueNumber(element.Value.asNumberArray[i])));
				}
				mainObject.ue4Object->SetArrayField(element.Key, a);
			}
		}
	}
	for (auto& element : StringMap) {
		mainObject.ue4Object->SetStringField(element.Key, element.Value);
	}
	for (auto& element : BoolMap) {
		mainObject.ue4Object->SetBoolField(element.Key, element.Value);
	}
	for (auto& element : NumberMap) {
		mainObject.ue4Object->SetNumberField(element.Key, element.Value);
	}


	//arrays
	mainObject.asObjectArray = ObjectArray;
	mainObject.asBoolArray = BoolArray;
	mainObject.asNumberArray = NumberArray;
	mainObject.asStringArray = StringArray;


	if (mainObject.asObjectArray.Num() > 0) {
		for (int i = 0; i < mainObject.asObjectArray.Num(); i++) {
			if (mainObject.asObjectArray[i].ue4Object.IsValid())
				mainObject.ue4Array.Add(MakeShareable(new FJsonValueObject(mainObject.asObjectArray[i].ue4Object)));
		}
	}

	if (mainObject.asStringArray.Num() > 0) {
		for (int i = 0; i < mainObject.asStringArray.Num(); i++) {
			mainObject.ue4Array.Add(MakeShareable(new FJsonValueString(mainObject.asStringArray[i])));
		}
	}
	if (mainObject.asBoolArray.Num() > 0) {
		for (int i = 0; i < mainObject.asBoolArray.Num(); i++) {
			mainObject.ue4Array.Add(MakeShareable(new FJsonValueBoolean(mainObject.asBoolArray[i])));
		}
	}
	if (mainObject.asNumberArray.Num() > 0) {
		for (int i = 0; i < mainObject.asNumberArray.Num(); i++) {
			mainObject.ue4Array.Add(MakeShareable(new FJsonValueNumber(mainObject.asNumberArray[i])));
		}
	}

	return mainObject;
}

struct FSJsonObject UJSONParserBPLibrary::generateFSJsonObject(struct FSJsonObject object) {
	TMap<FString, FSJsonObject> ObjectMap;
	TMap<FString, bool> BoolMap;
	TMap<FString, float> NumberMap;
	TMap<FString, FString> StringMap;
	FSJsonObject mainObject = generateFSJsonObjectWithParameter(ObjectMap, BoolMap, NumberMap, StringMap, object.asObjectArray, object.asBoolArray, object.asNumberArray, object.asStringArray);
	return mainObject;
}


FString UJSONParserBPLibrary::JsonObjectToString(struct FSJsonObject mainObject, bool removeLineBreaks, bool addPadding) {

	FString data;
	TSharedRef<TJsonWriter<>> json_writer = TJsonWriterFactory<>::Create(&data);

	if (!mainObject.ue4Object.IsValid()) {
		mainObject = generateFSJsonObject(mainObject);
	}
	if (mainObject.ue4Object->Values.Num() == 0 && mainObject.ue4Array.Num() > 0) {
		FJsonSerializer::Serialize(mainObject.ue4Array, json_writer);
	}
	else {
		FJsonSerializer::Serialize(mainObject.ue4Object.ToSharedRef(), json_writer);
	}

	if (addPadding) {
		data = mainObject.jsonPBefore + data + mainObject.jsonPAfter;
	}

	if (removeLineBreaks) {

		//UE_LOG(LogTemp, Warning, TEXT("1:%s"), *data);
		data = data.Replace(TEXT("\r"), TEXT(""));
		data = data.Replace(TEXT("\n"), TEXT(""));
		data = data.Replace(TEXT("\t"), TEXT(""));
		//UE_LOG(LogTemp, Warning, TEXT("2:%s"), *data);
	}



	return data;

}





FSJsonObject UJSONParserBPLibrary::JsonStringToObject(FString data, bool isJsonP) {

	FSJsonObject mainObject;

	if (isJsonP) {
		if (data.Contains("{")) {
			if (data.Split("{", &mainObject.jsonPBefore, &data)) {
				data = "{" + data;
			}
		}
		if (data.Contains("}")) {
			int32 index = 0;
			if (data.FindLastChar(*"}", index)) {
				if ((index + 1) < data.Len())
					mainObject.jsonPAfter = data.RightChop(index + 1);
				data = data.Left(index);
				data += "}";
			}
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("Before:%s"), *mainObject.jsonPBefore);
	//UE_LOG(LogTemp, Display, TEXT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"));
	//UE_LOG(LogTemp, Display, TEXT("After:%s"), *mainObject.jsonPAfter);
	//UE_LOG(LogTemp, Display, TEXT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"));
	//UE_LOG(LogTemp, Display, TEXT("%s"), *data);

	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(data);
	TArray< TSharedPtr<FJsonValue> > ue4ObjectArray;
	bool bSuccessful = FJsonSerializer::Deserialize(reader, ue4ObjectArray);
	if (bSuccessful) {

		for (int i = 0; i < ue4ObjectArray.Num(); i++) {
			//FMixedArrayObject mo;
			if (ue4ObjectArray[i]->Type == EJson::Object) {
				FSJsonObject o;
				o.ue4Object = ue4ObjectArray[i]->AsObject();
				mainObject.asObjectArray.Add(o);
				continue;
			}

			if (ue4ObjectArray[i]->Type == EJson::String) {
				mainObject.asStringArray.Add(ue4ObjectArray[i]->AsString());
				//mo.asString = ue4ObjectArray[i]->AsString();
				continue;
			}
			if (ue4ObjectArray[i]->Type == EJson::Boolean) {
				mainObject.asBoolArray.Add(ue4ObjectArray[i]->AsBool());
				//mo.asBool = ue4ObjectArray[i]->AsBool();
				continue;
			}
			if (ue4ObjectArray[i]->Type == EJson::Number) {
				mainObject.asNumberArray.Add(ue4ObjectArray[i]->AsNumber());
				//mo.asNumber= ue4ObjectArray[i]->AsNumber();
				continue;
			}
			//mainObject.asMixedArrayObject.Add(mo);
		}
		if (ue4ObjectArray.Num() > 0)
			mainObject.ue4Array = ue4ObjectArray;
	}
	else {
		TSharedPtr<FJsonObject> ue4Object;
		reader = TJsonReaderFactory<>::Create(data);
		if (FJsonSerializer::Deserialize(reader, ue4Object)) {
			mainObject.ue4Object = ue4Object;
		}
	}



	return mainObject;
}



void UJSONParserBPLibrary::BreakJson_Object(const FSJsonObject JSONObject, TMap<FString, FSJsonObject>& ObjectMap, TMap<FString, bool>& BoolMap, TMap<FString, float>& NumberMap, TMap<FString, FString>& StringMap, TArray<FSJsonObject>& ObjectArray, TArray<bool>& BoolArray, TArray<float>& NumberArray, TArray<FString>& StringArray) {

	ObjectMap.Empty();
	BoolMap.Empty();
	NumberMap.Empty();
	StringMap.Empty();
	ObjectArray.Empty();
	BoolArray.Empty();
	NumberArray.Empty();
	StringArray.Empty();

	TSharedPtr<FJsonObject> ue4Object = JSONObject.ue4Object;
	if (ue4Object.IsValid()) {
		//StringMap = JSONObject.asStringMap;
		TMap <FString, TSharedPtr<FJsonValue>> values = ue4Object->Values;
		TArray<FString> keys;
		int keyCount = values.GetKeys(keys);
		for (int i = 0; i < keys.Num(); i++) {
			if (ue4Object->HasTypedField<EJson::Object>(keys[i])) {
				FSJsonObject o;
				o.ue4Object = ue4Object->GetObjectField(keys[i]);
				ObjectMap.Add(keys[i], o);
				continue;
			}
			if (ue4Object->HasTypedField<EJson::String>(keys[i])) {
				StringMap.Add(keys[i], ue4Object->GetStringField(keys[i]));
				continue;
			}
			if (ue4Object->HasTypedField<EJson::Boolean>(keys[i])) {
				BoolMap.Add(keys[i], ue4Object->GetBoolField(keys[i]));
				continue;
			}
			if (ue4Object->HasTypedField<EJson::Number>(keys[i])) {
				NumberMap.Add(keys[i], ue4Object->GetNumberField(keys[i]));
				continue;
			}
			if (ue4Object->HasTypedField<EJson::Array>(keys[i])) {
				//UE_LOG(LogTemp, Warning, TEXT("Oh no, its an array"));
				FSJsonObject o;
				o = generateFSJsonObjectAndArray(o, ue4Object->GetArrayField(keys[i]));
				ObjectMap.Add(keys[i], o);
				continue;
			}
		}
	}


	ObjectArray = JSONObject.asObjectArray;
	BoolArray = JSONObject.asBoolArray;
	NumberArray = JSONObject.asNumberArray;
	StringArray = JSONObject.asStringArray;

}

void UJSONParserBPLibrary::BreakMixedJSONArray(const int32 index, const FSJsonObject JSONObject, int32& length, EJsonDataType& type, FSJsonObject& Object, bool& Bool, float& Number, FString& String) {

	length = 0;
	Object = {};
	Bool = false;
	Number = 0;
	String = "";

	if (JSONObject.ue4Array.Num() == 0) {
		length = 0;
		return;
	}

	length = JSONObject.ue4Array.Num() - 1;
	//UE_LOG(LogTemp, Warning, TEXT("loop:%i"), index);

	if (JSONObject.ue4Array[index]->Type == EJson::Object) {
		Object.ue4Object = JSONObject.ue4Array[index]->AsObject();
		type = EJsonDataType::E_Object;
		return;
	}
	if (JSONObject.ue4Array[index]->Type == EJson::String) {
		String = JSONObject.ue4Array[index]->AsString();
		type = EJsonDataType::E_String;
		return;
	}
	if (JSONObject.ue4Array[index]->Type == EJson::Boolean) {
		Bool = JSONObject.ue4Array[index]->AsBool();
		type = EJsonDataType::E_Bool;
		return;
	}
	if (JSONObject.ue4Array[index]->Type == EJson::Number) {
		Number = JSONObject.ue4Array[index]->AsNumber();
		type = EJsonDataType::E_Number;
		return;
	}

}

FSJsonObject UJSONParserBPLibrary::MakeMixedJSONArray(const FSJsonObject oldJSONObject, EJsonDataType type, FSJsonObject Object, bool Bool, float Number, FString String) {


	FSJsonObject mainObject;

	if (type == EJsonDataType::E_String) {
		mainObject.ue4Array.Add(MakeShareable(new FJsonValueString(String)));
	}
	if (type == EJsonDataType::E_Number) {
		mainObject.ue4Array.Add(MakeShareable(new FJsonValueNumber(Number)));
	}
	if (type == EJsonDataType::E_Bool) {
		mainObject.ue4Array.Add(MakeShareable(new FJsonValueBoolean(Bool)));
	}
	if (type == EJsonDataType::E_Object) {
		mainObject.ue4Array.Add(MakeShareable(new FJsonValueObject(Object.ue4Object)));
	}

	if (oldJSONObject.ue4Array.Num() > 0) {
		TArray< TSharedPtr<FJsonValue> > ue4Array;
		ue4Array.Append(oldJSONObject.ue4Array);
		ue4Array.Append(mainObject.ue4Array);
		mainObject.ue4Array = ue4Array;
	}
	if (oldJSONObject.ue4Object.IsValid()) {
		mainObject.ue4Object = oldJSONObject.ue4Object;
	}
	else {
		mainObject.ue4Object = MakeShareable(new FJsonObject());
	}
	return mainObject;
}



struct FSJsonObject UJSONParserBPLibrary::generateFSJsonObjectAndArray(FSJsonObject mainObject, TArray< TSharedPtr<FJsonValue> > ue4ObjectArray) {

	for (int i = 0; i < ue4ObjectArray.Num(); i++) {

		if (ue4ObjectArray[i]->Type == EJson::Object) {
			FSJsonObject o;
			o.ue4Object = ue4ObjectArray[i]->AsObject();
			mainObject.asObjectArray.Add(o);
			continue;
		}

		if (ue4ObjectArray[i]->Type == EJson::String) {
			mainObject.asStringArray.Add(ue4ObjectArray[i]->AsString());
			continue;
		}
		if (ue4ObjectArray[i]->Type == EJson::Boolean) {
			mainObject.asBoolArray.Add(ue4ObjectArray[i]->AsBool());
			continue;
		}
		if (ue4ObjectArray[i]->Type == EJson::Number) {
			mainObject.asNumberArray.Add(ue4ObjectArray[i]->AsNumber());
			continue;
		}
		if (ue4ObjectArray[i]->Type == EJson::Array) {
			FSJsonObject o;
			o = generateFSJsonObjectAndArray(o, ue4ObjectArray[i]->AsArray());
			mainObject.asObjectArray.Add(o);
			continue;
		}
	}

	return mainObject;
}


void UJSONParserBPLibrary::fileToBase64String(EJSONDirectoryType directoryType, FString filePath, FString& base64String, FString& fileName) {

	if (filePath.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: FilePath not set."));
		return;
	}


	FString dir;
	if (directoryType == EJSONDirectoryType::E_ad) {
		dir = FPaths::ConvertRelativePathToFull(filePath);
	}
	else {
		FString ProjectDir = FPaths::ProjectDir();
		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
	}


	if (!FPaths::FileExists(dir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: File not found: %s"), *dir);
		return;
	}

	TArray<uint8> fileData;
	if (!FFileHelper::LoadFileToArray(fileData, *dir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Can't read File: %s"), *dir);
		return;
	}

	fileName = FPaths::GetCleanFilename(dir);
	base64String = FBase64::Encode(fileData);
}

bool UJSONParserBPLibrary::base64StringToFile(EJSONDirectoryType directoryType, FString base64String, FString filePath) {

	if (base64String.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Base64String not set."));
		return false;
	}

	if (filePath.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: FilePath not set."));
		return false;
	}

	FString dir;
	if (directoryType == EJSONDirectoryType::E_ad) {
		dir = FPaths::ConvertRelativePathToFull(filePath);
	}
	else {
		FString ProjectDir = FPaths::ProjectDir();
		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
	}

	FString fileName = FPaths::GetCleanFilename(dir);
	FString tmpDir = dir;
	tmpDir.RemoveFromEnd(fileName);
	if (!FPaths::DirectoryExists(tmpDir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Directory not exists: %s"), *tmpDir);
		return false;
	}

	TArray<uint8> fileData;
	if (!FBase64::Decode(*base64String, fileData)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Base64 decode failed: %s"), *dir);
		return false;
	}

	if (!FFileHelper::SaveArrayToFile(fileData, *dir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Can't save File: %s"), *dir);
		return false;
	}

	return FPaths::FileExists(dir);
}

void UJSONParserBPLibrary::readStringFromFile(EJSONDirectoryType directoryType, FString filePath, FString& JSON_String) {
	if (filePath.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: FilePath not set."));
		return;
	}


	FString dir;
	if (directoryType == EJSONDirectoryType::E_ad) {
		dir = FPaths::ConvertRelativePathToFull(filePath);
	}
	else {
		FString ProjectDir = FPaths::ProjectDir();
		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
	}


	if (!FPaths::FileExists(dir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: File not found: %s"), *dir);
		return;
	}

	if (!FFileHelper::LoadFileToString(JSON_String, *dir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Can't read File: %s"), *dir);
		return;
	}
}

bool UJSONParserBPLibrary::writeStringToFile(EJSONDirectoryType directoryType, FString JSON_String, FString filePath) {
	if (JSON_String.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: JSON String is empty."));
		return false;
	}

	if (filePath.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: FilePath not set."));
		return false;
	}

	FString dir;
	if (directoryType == EJSONDirectoryType::E_ad) {
		dir = FPaths::ConvertRelativePathToFull(filePath);
	}
	else {
		FString ProjectDir = FPaths::ProjectDir();
		dir = FPaths::ConvertRelativePathToFull(ProjectDir + filePath);
	}

	FString fileName = FPaths::GetCleanFilename(dir);
	FString tmpDir = dir;
	tmpDir.RemoveFromEnd(fileName);
	if (!FPaths::DirectoryExists(tmpDir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Directory not exists: %s"), *tmpDir);
		return false;
	}



	if (!FFileHelper::SaveStringToFile(JSON_String, *dir)) {
		UE_LOG(LogTemp, Error, TEXT("JSONParser: Can't save File: %s"), *dir);
		return false;
	}

	return FPaths::FileExists(dir);
}
