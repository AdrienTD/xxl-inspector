@set PATCHVER=0,1,0,0
@rc /dPATCHVER=%PATCHVER% /dXXLVER=%XXLVER% resource.rc
@cl %* /c /DPATCHVER=%PATCHVER% /DXXLVER=%XXLVER% /LD /EHsc *.cpp imgui/*.cpp
@link /DLL /DEF:d3d9.def /OUT:d3d9.dll *.obj resource.res user32.lib version.lib