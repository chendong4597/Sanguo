// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ThirdGameModel : ModuleRules
{
	public ThirdGameModel(ReadOnlyTargetRules Target) : base(Target)
	{
        // 基础配置
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;


        //Type = ModuleType.External;


        PublicDependencyModuleNames.AddRange(new string[] { "Core"});

        bUseRTTI = true;
        bEnableExceptions = true;
        //bUseUnity = true;
        //bEnableUndefinedIdentifierWarnings = false;
        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            string ModulePath = ModuleDirectory;

            string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModulePath, "../../../"));
            // gets the ThirdParty folder directory path
            string ConnectorPath = ThirdPartyPath;

            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libAnl.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libJsoncpp.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libMinizip.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libZlib.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libProtobuf.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libCryptopp.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libBaseutil.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libNetwork.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libFramework.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libStaticMsg.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libStaticData.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libStaticLogic.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "IOS", "Lib", "libStaticLib.a"));

            //string BuildPath = Path.Combine(ConnectorPath, "Source", "ThirdGameModel");
            //AdditionalPropertiesForReceipt.Add("IosPlugin", Path.Combine(BuildPath, "Ios_thirdreich.xml"));

            ////Taptap
            //string FrameworkPath = Path.Combine(ConnectorPath, "Tap");
            //string FrameworkName;
            //string FrameworkModuleName;

            //// 添加PublicAdditionalFrameworks
            //FrameworkName = "tapsdkcorecpp.xcframework";
            //FrameworkModuleName = "tapsdkcorecpp";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName)
            //    )
            //);

            //FrameworkName = "TapTapBasicToolsSDK.xcframework";
            //FrameworkModuleName = "TapTapBasicToolsSDK";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName)
            //    )
            //);

            //FrameworkName = "TapTapCoreSDK.xcframework";
            //FrameworkModuleName = "TapTapCoreSDK";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName)
            //    )
            //);

            //FrameworkName = "TapTapGidSDK.xcframework";
            //FrameworkModuleName = "TapTapGidSDK";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName)
            //    )
            //);

            //FrameworkName = "TapTapNetworkSDK.xcframework";
            //FrameworkModuleName = "TapTapNetworkSDK";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName)
            //    )
            //);

            //FrameworkName = "THEMISLite.xcframework";
            //FrameworkModuleName = "THEMISLite";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName)
            //    )
            //);

            //FrameworkName = "TapTapLoginSDK.xcframework";
            //FrameworkModuleName = "TapTapLoginSDK";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName)
            //    )
            //);

            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        "SwiftUnistd",
            //        "", // 系统库不需要路径
            //        ""
            //    )
            //);

            //FrameworkName = "TapTapComplianceSDK.xcframework";
            //FrameworkModuleName = "TapTapComplianceSDK";
            //PublicAdditionalFrameworks.Add(
            //    new Framework(
            //        FrameworkModuleName,
            //        Path.Combine(FrameworkPath, FrameworkName),
            //        "TapTapComplianceResource.bundle"
            //    )
            //);

            //PublicAdditionalLibraries.AddRange(new string[] {
            //"swiftCompatibility56",
            //"swiftCompatibilityDynamicReplacements",
            //"swift_signal"
            //});

            //PublicDependencyModuleNames.AddRange(new string[] {
            //    "tapsdkcorecpp", // 添加模块依赖
            //    "TapTapBasicToolsSDK", // 添加模块依赖
            //    "TapTapCoreSDK", // 添加模块依赖
            //    "TapTapGidSDK", // 添加模块依赖
            //    "TapTapNetworkSDK", // 添加模块依赖
            //    "THEMISLite", // 添加模块依赖
            //    "TapTapLoginSDK" // 添加模块依赖
            //});


            //// 添加Bundle资源
            //string BundlePath = Path.Combine(ConnectorPath, "Tap");
            //string BundleName = "TapTapLoginResource.bundle";

            // 添加到运行时依赖
            //RuntimeDependencies.Add(
            //    Path.Combine("$(ProjectDir)/../Resources", BundleName), // 安装路径
            //    Path.Combine(BundlePath, BundleName) // 源文件路径
            //);

            // 在生成Xcode项目后执行脚本
            //string ScriptPath = Path.Combine(ModuleDirectory, "../../SetSwiftEmbedding.ps1");
            //string ProjectPath = Path.GetDirectoryName(Target.ProjectFile.FullName);

            //PostBuildSteps.Add(
            //    $"powershell -ExecutionPolicy Bypass -File \"{ScriptPath}\" -ProjectPath \"{ProjectPath}\""
            //    );

            // 添加Swift库

            //AdditionalFrameworks.Add(new Framework("SwiftUI", "SwiftUI.framework"));

            //AdditionalFrameworks.Add(new Framework("Combine", "Combine.framework"));

            // 注意：根据你的Swift库需要，可能还需要添加其他框架

            // 启用Swift支持

            //bEnableSwift = true;
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string ModulePath = ModuleDirectory;

            string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModulePath, "../../"));
            // gets the ThirdParty folder directory path
            string ConnectorPath = ThirdPartyPath;

            //PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI=1");
            //PublicDefinitions.Add("GOOGLE_PROTOBUF_CMAKE_BUILD");

            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj" , "local", "armeabi-v7a", "libanl.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libzlib.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libminizip.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libjsoncpp.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libcurl.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "arm64-v8a", "libcurl.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libprotobuf.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libbaseutil.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libframework.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libnetwork.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libmsg.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "libdata.a"));
            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "obj", "local", "armeabi-v7a", "liblogic.a"));

            //PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "libs", "armeabi-v7a", "libthirdreich.so"));
            PublicAdditionalLibraries.Add(Path.Combine(ConnectorPath, "Android", "libs", "arm64-v8a", "libthirdreich.so"));

            string BuildPath = Path.Combine(ConnectorPath, "Source", "ThirdGameModel");

            //AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(BuildPath, "copy_thirdreich.xml"));
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(BuildPath, "copy_thirdreich_arm64.xml"));
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(BuildPath, "thirdreich_tap3.xml"));

            // 添加AAR依赖
            PrivateDependencyModuleNames.Add("Launch");
            //AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(BuildPath, "thirdreich_tap.xml")));


            //AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(BuildPath, "copy_msg.xml"))); 
        }
        else
        {
            string ModulePath = ModuleDirectory;

            string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModulePath, "../../"));
            // gets the ThirdParty folder directory path
            string ConnectorPath = ThirdPartyPath;

            string[] strExternalLibPath = { "libminizip", "libjsoncpp", "libzlib", "libcurl", "libanl", "libprotobuf", "libcryptopp" };
            string[] strExternalLibName = { "static_minizip.lib", "static_jsoncpp.lib", "static_zlib.lib", "static_curl.lib", "static_anl.lib", "static_protobuf.lib", "static_cryptopp.lib" };

            for (int i = 0; i < strExternalLibPath.Length; i++)
            {
                string ConnectorLibraryPath = ConnectorPath + "x64/RelWithDebInfo";
                // gets the path of the lib folder      
                string ConnectorIncludePath = ConnectorPath + "external/" + strExternalLibPath[i] + "/source/";
                // gets the path of the include folder      
                string ConnectorImportLibraryName = Path.Combine(ConnectorLibraryPath, strExternalLibName[i]);

                // gets the file path and name of the libmysql.lib static import library 

                if (!File.Exists(ConnectorImportLibraryName))
                // check to ensure the static import lib can be located, or else we'll be in trouble        
                {
                    throw new BuildException(string.Format("{0} could not be found.", ConnectorImportLibraryName));
                    // log an error message explaining what went wrong if not found       
                }

                PrivateIncludePaths.Add(ConnectorIncludePath);
                // add the "include" folder to our dependencies. I've chosen PrivateIncludePaths since I hide the mysql headers from external code 
                //PublicLibraryPaths.Add(ConnectorLibraryPath);
                // add the "lib" folder to our dependencies        
                PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                // add libmysql.lib static import library as a dependency      
            }

            string[] strBzbeeLibPath = { "libbaseutil", "libnetwork", "libframework" };
            string[] strBzbeeLibName = { "static_baseutil.lib", "static_network.lib", "static_framework.lib" };

            for (int i = 0; i < strBzbeeLibPath.Length; i++)
            {
                string ConnectorLibraryPath = ConnectorPath + "x64/RelWithDebInfo";
                // gets the path of the lib folder      
                string ConnectorIncludePath = ConnectorPath + "bzbee/" + strBzbeeLibPath[i] + "/source/";
                // gets the path of the include folder      
                string ConnectorImportLibraryName = Path.Combine(ConnectorLibraryPath, strBzbeeLibName[i]);

                // gets the file path and name of the libmysql.lib static import library 

                if (!File.Exists(ConnectorImportLibraryName))
                // check to ensure the static import lib can be located, or else we'll be in trouble        
                {
                    throw new BuildException(string.Format("{0} could not be found.", ConnectorImportLibraryName));
                    // log an error message explaining what went wrong if not found       
                }

                PrivateIncludePaths.Add(ConnectorIncludePath);
                // add the "include" folder to our dependencies. I've chosen PrivateIncludePaths since I hide the mysql headers from external code 
                // PublicLibraryPaths.Add(ConnectorLibraryPath);
                // add the "lib" folder to our dependencies        
                PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                // add libmysql.lib static import library as a dependency      
            }




            {
                string strLibPath = "static_data";// { , "static_msg" };
                string strLibName = "static_data.lib";// { , "static_msg.lib" };
                string ConnectorLibraryPath = ConnectorPath + "x64/Release";
                // gets the path of the lib folder      
                string ConnectorIncludePath = ConnectorPath + "static/" + strLibPath;
                // gets the path of the include folder      
                string ConnectorImportLibraryName = Path.Combine(ConnectorLibraryPath, strLibName);

                // gets the file path and name of the libmysql.lib static import library 

                if (!File.Exists(ConnectorImportLibraryName))
                // check to ensure the static import lib can be located, or else we'll be in trouble        
                {
                    throw new BuildException(string.Format("{0} could not be found.", ConnectorImportLibraryName));
                    // log an error message explaining what went wrong if not found       
                }

                PrivateIncludePaths.Add(ConnectorIncludePath);
                // add the "include" folder to our dependencies. I've chosen PrivateIncludePaths since I hide the mysql headers from external code 
                //PublicLibraryPaths.Add(ConnectorLibraryPath);
                // add the "lib" folder to our dependencies        
                PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                // add libmysql.lib static import library as a dependency      
            }


            {
                string strLibPath = "static_msg";// { , "static_msg" };
                string strLibName = "static_msg.lib";// { , "static_msg.lib" };
                string ConnectorLibraryPath = ConnectorPath + "x64/Release";
                // gets the path of the lib folder      
                string ConnectorIncludePath = ConnectorPath + "static/" + strLibPath;
                // gets the path of the include folder      
                string ConnectorImportLibraryName = Path.Combine(ConnectorLibraryPath, strLibName);

                // gets the file path and name of the libmysql.lib static import library 

                if (!File.Exists(ConnectorImportLibraryName))
                // check to ensure the static import lib can be located, or else we'll be in trouble        
                {
                    throw new BuildException(string.Format("{0} could not be found.", ConnectorImportLibraryName));
                    // log an error message explaining what went wrong if not found       
                }

                PrivateIncludePaths.Add(ConnectorIncludePath);
                // add the "include" folder to our dependencies. I've chosen PrivateIncludePaths since I hide the mysql headers from external code 
                //PublicLibraryPaths.Add(ConnectorLibraryPath);
                // add the "lib" folder to our dependencies        
                PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                // add libmysql.lib static import library as a dependency      
            }

            {
                string strLibPath = "static_logic";// { , "static_msg" };
                string strLibName = "static_logic.lib";// { , "static_msg.lib" };
                string ConnectorLibraryPath = ConnectorPath + "x64/Release";
                // gets the path of the lib folder      
                string ConnectorIncludePath = ConnectorPath + "static/" + strLibPath;
                // gets the path of the include folder      
                string ConnectorImportLibraryName = Path.Combine(ConnectorLibraryPath, strLibName);

                // gets the file path and name of the libmysql.lib static import library 

                if (!File.Exists(ConnectorImportLibraryName))
                // check to ensure the static import lib can be located, or else we'll be in trouble        
                {
                    throw new BuildException(string.Format("{0} could not be found.", ConnectorImportLibraryName));
                    // log an error message explaining what went wrong if not found       
                }

                PrivateIncludePaths.Add(ConnectorIncludePath);
                // add the "include" folder to our dependencies. I've chosen PrivateIncludePaths since I hide the mysql headers from external code 
                //PublicLibraryPaths.Add(ConnectorLibraryPath);
                // add the "lib" folder to our dependencies        
                PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                // add libmysql.lib static import library as a dependency      
            }
        }

    }
}
