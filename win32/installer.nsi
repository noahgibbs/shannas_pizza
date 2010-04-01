# define name of installer
!searchparse ${__DATE__} `` MONTH `/` DAY `/` YEAR
outFile "shannas_pizza_demo_${YEAR}_${MONTH}_${DAY}.exe"

# define installation directory
installDir "$DESKTOP\shannas_pizza"

# start default section
section

    # set the installation directory as the destination for the following actions
    setOutPath $INSTDIR

    # create the uninstaller
    writeUninstaller "$INSTDIR\uninstall.exe"

    CreateDirectory "$SMPROGRAMS\Shanna's Daddy Software"
    createShortCut "$SMPROGRAMS\Shanna's Daddy Software\Shanna's Pizza.lnk" "$INSTDIR\pizza.exe"
    createShortCut "$SMPROGRAMS\Shanna's Daddy Software\Shanna's Pizza (full-screen).lnk" "$INSTDIR\pizza.exe --fullscreen"
    createShortCut "$SMPROGRAMS\Shanna's Daddy Software\Uninstall Shanna's Pizza.lnk" "$INSTDIR\uninstall.exe"

    #MkDir "$INSTDIR/resources"

    file /r ..\resources
    file pizza.exe
    file dlls\*.dll
    file dlls\*.exe
sectionEnd

# uninstaller section start
section "uninstall"

    # second, remove the link from the start menu
    RmDir /r "$SMPROGRAMS\Shanna's Daddy Software"

    RMDir /r $INSTDIR
# uninstaller section end
sectionEnd
