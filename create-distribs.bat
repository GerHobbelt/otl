rem
rem create distribution files for www.hebbut.net
rem
                          
set name=OTL-4.0.202.i_a
                                                       
rmdir /q /s  .\distrib
mkdir .\distrib

pushd .\distrib

del *.bak
del *.exe 
del *.tar
del *.bz2

popd

rem create 7z files for the source distros, etc.

7z a -r -ssw -scsWIN -y -x@create-distribs.exclude -x@create-distribs.src.exclude .\distrib\%name%.full-src.7z *
7z a -r -ssw -scsWIN -y -x@create-distribs.exclude -x@create-distribs.exe.exclude .\distrib\%name%.bin-win32.7z *.exe *.dll bin\*.* ChangeLog* ReadMe* COPYING* CREDITS* NEWS* AUTHORS*
                                                          
                                                          
