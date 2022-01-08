add_library(secrets STATIC Secrets.cpp Secrets.h)
target_link_libraries(secrets Qt5::Core)
target_compile_definitions(secrets PUBLIC -DEMBED_SECRETS)
target_include_directories(secrets PUBLIC .)

set(Launcher_CommonName "Blockgame")

set(Launcher_Copyright "Blockgame Contributors" PARENT_SCOPE)
set(Launcher_Domain "https://yukiisbo.red" PARENT_SCOPE)
set(Launcher_Name "${Launcher_CommonName}" PARENT_SCOPE)
set(Launcher_DisplayName "${Launcher_CommonName} 5" PARENT_SCOPE)
set(Launcher_UserAgent "${Launcher_CommonName}/5.0" PARENT_SCOPE)
set(Launcher_ConfigFile "blockgame.cfg" PARENT_SCOPE)
set(Launcher_Git "https://github.com/yukiisbored/Launcher" PARENT_SCOPE)

set(Launcher_Branding_ICNS "notsecrets/Launcher.icns" PARENT_SCOPE)
set(Launcher_Branding_WindowsRC "notsecrets/launcher.rc" PARENT_SCOPE)
set(Launcher_Branding_LogoQRC "notsecrets/logo.qrc" PARENT_SCOPE)


if(BGL_MSA_CLIENT_ID)
    message(STATUS "Microsoft Client ID token is registered")
    target_compile_definitions(secrets PRIVATE "-DBGL_MSA_CLIENT_ID=${BGL_MSA_CLIENT_ID}")
endif()
