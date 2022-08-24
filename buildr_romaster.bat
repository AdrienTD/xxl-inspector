@set XXLVER=1

@set PATCHVER=0,2,0,0
@set PATCHVERSTR="0.2.0"
@rc /dPATCHVER=%PATCHVER% /dXXLVER=%XXLVER% /dREMASTER resource.rc
@cl %* /c /DPATCHVERSTR=%PATCHVERSTR% /DXXLVER=%XXLVER% /DREMASTER /Iinclude /LD /EHsc *.c *.cpp imgui/*.cpp
@link /DLL /DEF:opengl32.def /OUT:opengl32.dll *.obj resource.res user32.lib version.lib gdi32.lib
@copy opengl32.dll C:\Users\Adrien\Desktop\kthings\romaster_debug\opengl32.dll