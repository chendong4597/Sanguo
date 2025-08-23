// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Sanguo : ModuleRules
{
	public Sanguo(ReadOnlyTargetRules Target) : base(Target)
	{

        // 基础配置
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;


        //AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        bUseUnity = true;
        if (Target.Platform == UnrealTargetPlatform.IOS)
        {

           
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            
        }
        else
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "InputCore",
                "EnhancedInput", "HeadMountedDisplay", "NavigationSystem", "Niagara", "AIModule", "PakFile", "UMG", "AIModule",
                "ThirdGameModel"});

            PublicDefinitions.Add("FMT_GCC_VERSION=11"); // 根据实际 GCC 版本修改
            PublicDefinitions.Add("FMT_USE_NOEXCEPT=1");
            PublicDefinitions.Add("FMT_USE_DELETED_FUNCTIONS=1");

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

                //PublicAdditionalLibraries.Add(ConnectorImportLibraryName);

                PrivateIncludePaths.Add(ConnectorIncludePath);    
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
                //PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                PrivateIncludePaths.Add(ConnectorIncludePath);    
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
                //PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                PrivateIncludePaths.Add(ConnectorIncludePath);    
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
               // PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                PrivateIncludePaths.Add(ConnectorIncludePath);   
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
                //PublicAdditionalLibraries.Add(ConnectorImportLibraryName);
                PrivateIncludePaths.Add(ConnectorIncludePath);   
            }

            {
                string ConnectorIncludePath = ConnectorPath + "Source/Sanguo";
                PrivateIncludePaths.Add(ConnectorIncludePath);

                string ConnectorIncludePath1 = ConnectorPath + "Source";
                PrivateIncludePaths.Add(ConnectorIncludePath1);
            }
        }

        //PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
