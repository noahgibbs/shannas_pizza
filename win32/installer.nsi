# define name of installer
outFile "shannas_pizza.exe"

# define installation directory
installDir "$DESKTOP\shannas_pizza"

# start default section
section

    # set the installation directory as the destination for the following actions
    setOutPath $INSTDIR

    # create the uninstaller
    writeUninstaller "$INSTDIR\uninstall.exe"

    # create a shortcut named "new shortcut" in the start menu programs directory
    # point the new shortcut at the program uninstaller
    createShortCut "$SMPROGRAMS\Uninstall Shannas Pizza.lnk" "$INSTDIR\uninstall.exe"

    #MkDir "$INSTDIR/resources"

    file /r ..\resources
    file pizza.exe
    file dlls\*.dll
sectionEnd

# uninstaller section start
section "uninstall"

    # first, delete the uninstaller
    delete "$INSTDIR\uninstall.exe"

    # second, remove the link from the start menu
    delete "$SMPROGRAMS\Uninstall Shannas Pizza.lnk"

    delete "$INSTDIR\resources\*.*"
    RMDir "$INSTDIR\resources"

    delete "$INSTDIR\*.*"
    RMDir $INSTDIR
# uninstaller section end
sectionEnd
