// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAssetManager.h"
#include "Misc/FileHelper.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "GameAppExtern.h"

URPGAssetManager& URPGAssetManager::Get()
{
	URPGAssetManager* This = Cast<URPGAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogOutputDevice, Log, TEXT("Invalid AssetManager in DefaultEngine.ini, must be RPGAssetManager!"));
		return *NewObject<URPGAssetManager>(); // never calls this
	}
}

bool CopyInternalResourceToExternal(const FString& InternalResourcePath, const FString& ExternalResourcePath)
{
	// 确保外部文件路径的目录存在
	FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*ExternalResourcePath);

	// 复制文件
	return FPlatformFileManager::Get().GetPlatformFile().CopyFile(*ExternalResourcePath, *InternalResourcePath);
}

void URPGAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 获取平台文件管理器实例
	FPlatformFileManager& PlatformFileManager = FPlatformFileManager::Get();
	// 获取IPlatformFile接口
	IPlatformFile& PlatformFile = PlatformFileManager.GetPlatformFile();

	UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading new ProjectSavedDir %s"), *FPaths::ProjectSavedDir());
	UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading new ProjectUserDir %s"), *FPaths::ProjectUserDir());

	FString DocumentsDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectUserDir());
	UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading relevie DocumentsDir %s"), *DocumentsDir);

	FString SaveDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading relevie SaveDir %s"), *SaveDir);

	FString SetDir = FPlatformProcess::UserSettingsDir();
	UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading relevie UserSettingsDir %s"), *SetDir);



#if PLATFORM_ANDROID || PLATFORM_IOS
	//extern FString GFilePathBase;
	// Obb File Path base - setup during load
	//extern FString GOBBFilePathBase;
	// Internal File Direcory Path (for application) - setup during load
	//extern FString GInternalFilePath;
	// External File Direcory Path (for application) - setup during load

	FString strExPath;
#if PLATFORM_ANDROID
	extern FString GExternalFilePath;
	strExPath = GExternalFilePath;
#else
	strExPath = FPlatformProcess::UserSettingsDir();
#endif

	//extern FString GFilePathBase;
	//ExternalPath = GFilePathBase + FString("/UE4Game/") + UKismetSystemLibrary::GetGameName() + FString("/config/trunk.mo");

	//UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading GFilePathBase %s"), *GFilePathBase);
	//UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading GOBBFilePathBase %s"), *GOBBFilePathBase);
	//UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading GInternalFilePath %s"), *GInternalFilePath);
	UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading GExternalFilePath %s"), *strExPath);

	int nPreVersion = 0;
	int nCurVersioin = 0;

	FString FileName = FPaths::ProjectContentDir() + "config/version.dat"; // 版本比较
	FString FileContents;
	if (FFileHelper::LoadFileToString(FileContents, *FileName)) {
		UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading pre version success FileContents = %s"), *FileContents);
		nPreVersion = FCString::Atoi(*FileContents);
	}
	else {
		UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading pre version failed = %s"), *FileName);
	}
	
#if PLATFORM_ANDROID
	FileName = strExPath + FString("/Content/config/version.dat"); // 版本比较
#else
	FileName = strExPath + FString("config/version.dat"); // 版本比较
#endif

	if (FFileHelper::LoadFileToString(FileContents, *FileName)) {
		UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading nxt version success FileContents = %s"), *FileContents);
		nCurVersioin = FCString::Atoi(*FileContents);
	}
	else {
		UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading nxt version failed = %s"), *FileName);
	}

	if (nPreVersion > nCurVersioin) {
		TArray<FString> arrStr;
		FString srcFonder = FPaths::ProjectContentDir() / TEXT("config"); //  + "config/";

#if PLATFORM_ANDROID
		FString dstFonder = strExPath + FString("/Content/config/");
#else
		FString dstFonder = strExPath + FString("config/");
#endif

		IFileManager::Get().FindFilesRecursive(arrStr, *srcFonder, TEXT("*.*"), true, false);

		UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading arrStr size = %d"), arrStr.Num());
		for (int i = 0; i < arrStr.Num(); i++)
		{
			FString strSrc = arrStr[i];
			TCHAR delimiter = '/';
			int32 FoundIndex = 0;
			FString strDst = dstFonder + arrStr[i];
			if (strSrc.FindLastChar(delimiter, FoundIndex))
			{
				if (FoundIndex != INDEX_NONE)
				{
					FString ExtractedString = strSrc.Right(strSrc.Len() - FoundIndex - 1);
					strDst = dstFonder + ExtractedString;
				}
				else {
					continue;
				}
			}
			else {
				continue;
			}


			UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading arrStr strSrc = %s strDst = %s"), *strSrc, *strDst);
			if (IFileManager::Get().Copy(*strDst, *strSrc) == COPY_OK) {
				UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading copy suc"));
#if PLATFORM_IOS
				PlatformFile.SetReadOnly(*strDst, false);
#endif
			}
			else {
				UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading copy failed"));
			}
		}
	}
#endif
	//UGameplayStatics::ScanDirectory(Files, FilePath, Extension);
}


void URPGAssetManager::EntryConfigFiles()
{
	TArray<FString> arrStr;
	FString srcFonder = FPaths::ProjectContentDir() + "src_config/";

	IFileManager::Get().FindFilesRecursive(arrStr, *srcFonder, TEXT("*.xml"), true, false);

	UE_LOG(LogOutputDevice, Log, TEXT("StartInitialLoading arrStr size = %d"), arrStr.Num());
	for (int i = 0; i < arrStr.Num(); i++)
	{
		// Unreal Engine 的 FString 转 std::string
		FString strSrc = arrStr[i];
		std::string stdStr(TCHAR_TO_UTF8(*strSrc));
		std::string strOut;
		std::string strContent;

		FString FileNameWithoutExtension = FPaths::GetBaseFilename(strSrc);
		FileNameWithoutExtension.Append(".bin");
		FString strOutPath = FPaths::ProjectContentDir() + "config/" + FileNameWithoutExtension;

		std::string stdFileOutStr(TCHAR_TO_UTF8(*strOutPath));

		GameAppExtern::GameEncryptFile(stdStr, strOut, strContent, stdFileOutStr);
	}
}
