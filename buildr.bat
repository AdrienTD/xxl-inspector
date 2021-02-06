@set PATCHVER=0,1,1,1
@set PATCHVERSTR="0.1.1X"
@rc /dPATCHVER=%PATCHVER% /dXXLVER=%XXLVER% resource.rc
@cl %* /c /DPATCHVERSTR=%PATCHVERSTR% /DXXLVER=%XXLVER% /LD /EHsc *.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_demo.cpp imgui/imgui_impl_dx9.cpp imgui/imgui_impl_win32.cpp
@link /DLL /DEF:d3d9.def /OUT:d3d9.dll *.obj resource.res user32.lib version.lib