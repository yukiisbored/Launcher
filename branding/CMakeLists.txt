add_library(secrets STATIC Secrets.cpp Secrets.h)
target_link_libraries(secrets Qt5::Core)
target_compile_definitions(secrets PUBLIC -DEMBED_SECRETS)
target_include_directories(secrets PUBLIC .)

set(Launcher_CommonName "Blockgame")

set(Launcher_Copyright "Blockgame Contributors" PARENT_SCOPE)
set(Launcher_Domain "yukiisbo.red" PARENT_SCOPE)
set(Launcher_Name "${Launcher_CommonName}" PARENT_SCOPE)
set(Launcher_DisplayName "${Launcher_CommonName}" PARENT_SCOPE)
set(Launcher_UserAgent "Mozilla/5.0" PARENT_SCOPE)
set(Launcher_ConfigFile "blockgame.cfg" PARENT_SCOPE)
set(Launcher_Git "https://github.com/yukiisbored/Launcher" PARENT_SCOPE)

set(Launcher_Branding_ICNS "branding/Launcher.icns" PARENT_SCOPE)
set(Launcher_Branding_WindowsRC "branding/launcher.rc" PARENT_SCOPE)
set(Launcher_Branding_LogoQRC "branding/logo.qrc" PARENT_SCOPE)
