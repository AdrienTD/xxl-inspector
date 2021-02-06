#ifdef REMASTER

//#include "global.h"
#include <Windows.h>
#include <io.h>
#define naked __declspec(naked)

void *glFuncPointers[368];
void InitGL()
{
    static bool isGLInitialized = false;
    if(isGLInitialized)
        return;
    isGLInitialized = true;
    static HMODULE hgl = 0;
    wchar_t tbuf[512];
	if(!hgl)
	{
		if(_access("apopengl32.dll", 0) != -1) // If apopengl32.dll exists.
		{
			hgl = LoadLibraryA("apopengl32.dll");
		}
		else
		{
			GetSystemDirectoryW(tbuf, 511);
			wcscat_s(tbuf, 255, L"\\opengl32.dll");
            MessageBoxW(NULL, tbuf, NULL, 64);
			hgl = LoadLibraryW(tbuf);
		}
	}

    glFuncPointers[0] = GetProcAddress(hgl, "GlmfBeginGlsBlock");
    glFuncPointers[1] = GetProcAddress(hgl, "GlmfCloseMetaFile");
    glFuncPointers[2] = GetProcAddress(hgl, "GlmfEndGlsBlock");
    glFuncPointers[3] = GetProcAddress(hgl, "GlmfEndPlayback");
    glFuncPointers[4] = GetProcAddress(hgl, "GlmfInitPlayback");
    glFuncPointers[5] = GetProcAddress(hgl, "GlmfPlayGlsRecord");
    glFuncPointers[6] = GetProcAddress(hgl, "glAccum");
    glFuncPointers[7] = GetProcAddress(hgl, "glAlphaFunc");
    glFuncPointers[8] = GetProcAddress(hgl, "glAreTexturesResident");
    glFuncPointers[9] = GetProcAddress(hgl, "glArrayElement");
    glFuncPointers[10] = GetProcAddress(hgl, "glBegin");
    glFuncPointers[11] = GetProcAddress(hgl, "glBindTexture");
    glFuncPointers[12] = GetProcAddress(hgl, "glBitmap");
    glFuncPointers[13] = GetProcAddress(hgl, "glBlendFunc");
    glFuncPointers[14] = GetProcAddress(hgl, "glCallList");
    glFuncPointers[15] = GetProcAddress(hgl, "glCallLists");
    glFuncPointers[16] = GetProcAddress(hgl, "glClear");
    glFuncPointers[17] = GetProcAddress(hgl, "glClearAccum");
    glFuncPointers[18] = GetProcAddress(hgl, "glClearColor");
    glFuncPointers[19] = GetProcAddress(hgl, "glClearDepth");
    glFuncPointers[20] = GetProcAddress(hgl, "glClearIndex");
    glFuncPointers[21] = GetProcAddress(hgl, "glClearStencil");
    glFuncPointers[22] = GetProcAddress(hgl, "glClipPlane");
    glFuncPointers[23] = GetProcAddress(hgl, "glColor3b");
    glFuncPointers[24] = GetProcAddress(hgl, "glColor3bv");
    glFuncPointers[25] = GetProcAddress(hgl, "glColor3d");
    glFuncPointers[26] = GetProcAddress(hgl, "glColor3dv");
    glFuncPointers[27] = GetProcAddress(hgl, "glColor3f");
    glFuncPointers[28] = GetProcAddress(hgl, "glColor3fv");
    glFuncPointers[29] = GetProcAddress(hgl, "glColor3i");
    glFuncPointers[30] = GetProcAddress(hgl, "glColor3iv");
    glFuncPointers[31] = GetProcAddress(hgl, "glColor3s");
    glFuncPointers[32] = GetProcAddress(hgl, "glColor3sv");
    glFuncPointers[33] = GetProcAddress(hgl, "glColor3ub");
    glFuncPointers[34] = GetProcAddress(hgl, "glColor3ubv");
    glFuncPointers[35] = GetProcAddress(hgl, "glColor3ui");
    glFuncPointers[36] = GetProcAddress(hgl, "glColor3uiv");
    glFuncPointers[37] = GetProcAddress(hgl, "glColor3us");
    glFuncPointers[38] = GetProcAddress(hgl, "glColor3usv");
    glFuncPointers[39] = GetProcAddress(hgl, "glColor4b");
    glFuncPointers[40] = GetProcAddress(hgl, "glColor4bv");
    glFuncPointers[41] = GetProcAddress(hgl, "glColor4d");
    glFuncPointers[42] = GetProcAddress(hgl, "glColor4dv");
    glFuncPointers[43] = GetProcAddress(hgl, "glColor4f");
    glFuncPointers[44] = GetProcAddress(hgl, "glColor4fv");
    glFuncPointers[45] = GetProcAddress(hgl, "glColor4i");
    glFuncPointers[46] = GetProcAddress(hgl, "glColor4iv");
    glFuncPointers[47] = GetProcAddress(hgl, "glColor4s");
    glFuncPointers[48] = GetProcAddress(hgl, "glColor4sv");
    glFuncPointers[49] = GetProcAddress(hgl, "glColor4ub");
    glFuncPointers[50] = GetProcAddress(hgl, "glColor4ubv");
    glFuncPointers[51] = GetProcAddress(hgl, "glColor4ui");
    glFuncPointers[52] = GetProcAddress(hgl, "glColor4uiv");
    glFuncPointers[53] = GetProcAddress(hgl, "glColor4us");
    glFuncPointers[54] = GetProcAddress(hgl, "glColor4usv");
    glFuncPointers[55] = GetProcAddress(hgl, "glColorMask");
    glFuncPointers[56] = GetProcAddress(hgl, "glColorMaterial");
    glFuncPointers[57] = GetProcAddress(hgl, "glColorPointer");
    glFuncPointers[58] = GetProcAddress(hgl, "glCopyPixels");
    glFuncPointers[59] = GetProcAddress(hgl, "glCopyTexImage1D");
    glFuncPointers[60] = GetProcAddress(hgl, "glCopyTexImage2D");
    glFuncPointers[61] = GetProcAddress(hgl, "glCopyTexSubImage1D");
    glFuncPointers[62] = GetProcAddress(hgl, "glCopyTexSubImage2D");
    glFuncPointers[63] = GetProcAddress(hgl, "glCullFace");
    glFuncPointers[64] = GetProcAddress(hgl, "glDebugEntry");
    glFuncPointers[65] = GetProcAddress(hgl, "glDeleteLists");
    glFuncPointers[66] = GetProcAddress(hgl, "glDeleteTextures");
    glFuncPointers[67] = GetProcAddress(hgl, "glDepthFunc");
    glFuncPointers[68] = GetProcAddress(hgl, "glDepthMask");
    glFuncPointers[69] = GetProcAddress(hgl, "glDepthRange");
    glFuncPointers[70] = GetProcAddress(hgl, "glDisable");
    glFuncPointers[71] = GetProcAddress(hgl, "glDisableClientState");
    glFuncPointers[72] = GetProcAddress(hgl, "glDrawArrays");
    glFuncPointers[73] = GetProcAddress(hgl, "glDrawBuffer");
    glFuncPointers[74] = GetProcAddress(hgl, "glDrawElements");
    glFuncPointers[75] = GetProcAddress(hgl, "glDrawPixels");
    glFuncPointers[76] = GetProcAddress(hgl, "glEdgeFlag");
    glFuncPointers[77] = GetProcAddress(hgl, "glEdgeFlagPointer");
    glFuncPointers[78] = GetProcAddress(hgl, "glEdgeFlagv");
    glFuncPointers[79] = GetProcAddress(hgl, "glEnable");
    glFuncPointers[80] = GetProcAddress(hgl, "glEnableClientState");
    glFuncPointers[81] = GetProcAddress(hgl, "glEnd");
    glFuncPointers[82] = GetProcAddress(hgl, "glEndList");
    glFuncPointers[83] = GetProcAddress(hgl, "glEvalCoord1d");
    glFuncPointers[84] = GetProcAddress(hgl, "glEvalCoord1dv");
    glFuncPointers[85] = GetProcAddress(hgl, "glEvalCoord1f");
    glFuncPointers[86] = GetProcAddress(hgl, "glEvalCoord1fv");
    glFuncPointers[87] = GetProcAddress(hgl, "glEvalCoord2d");
    glFuncPointers[88] = GetProcAddress(hgl, "glEvalCoord2dv");
    glFuncPointers[89] = GetProcAddress(hgl, "glEvalCoord2f");
    glFuncPointers[90] = GetProcAddress(hgl, "glEvalCoord2fv");
    glFuncPointers[91] = GetProcAddress(hgl, "glEvalMesh1");
    glFuncPointers[92] = GetProcAddress(hgl, "glEvalMesh2");
    glFuncPointers[93] = GetProcAddress(hgl, "glEvalPoint1");
    glFuncPointers[94] = GetProcAddress(hgl, "glEvalPoint2");
    glFuncPointers[95] = GetProcAddress(hgl, "glFeedbackBuffer");
    glFuncPointers[96] = GetProcAddress(hgl, "glFinish");
    glFuncPointers[97] = GetProcAddress(hgl, "glFlush");
    glFuncPointers[98] = GetProcAddress(hgl, "glFogf");
    glFuncPointers[99] = GetProcAddress(hgl, "glFogfv");
    glFuncPointers[100] = GetProcAddress(hgl, "glFogi");
    glFuncPointers[101] = GetProcAddress(hgl, "glFogiv");
    glFuncPointers[102] = GetProcAddress(hgl, "glFrontFace");
    glFuncPointers[103] = GetProcAddress(hgl, "glFrustum");
    glFuncPointers[104] = GetProcAddress(hgl, "glGenLists");
    glFuncPointers[105] = GetProcAddress(hgl, "glGenTextures");
    glFuncPointers[106] = GetProcAddress(hgl, "glGetBooleanv");
    glFuncPointers[107] = GetProcAddress(hgl, "glGetClipPlane");
    glFuncPointers[108] = GetProcAddress(hgl, "glGetDoublev");
    glFuncPointers[109] = GetProcAddress(hgl, "glGetError");
    glFuncPointers[110] = GetProcAddress(hgl, "glGetFloatv");
    glFuncPointers[111] = GetProcAddress(hgl, "glGetIntegerv");
    glFuncPointers[112] = GetProcAddress(hgl, "glGetLightfv");
    glFuncPointers[113] = GetProcAddress(hgl, "glGetLightiv");
    glFuncPointers[114] = GetProcAddress(hgl, "glGetMapdv");
    glFuncPointers[115] = GetProcAddress(hgl, "glGetMapfv");
    glFuncPointers[116] = GetProcAddress(hgl, "glGetMapiv");
    glFuncPointers[117] = GetProcAddress(hgl, "glGetMaterialfv");
    glFuncPointers[118] = GetProcAddress(hgl, "glGetMaterialiv");
    glFuncPointers[119] = GetProcAddress(hgl, "glGetPixelMapfv");
    glFuncPointers[120] = GetProcAddress(hgl, "glGetPixelMapuiv");
    glFuncPointers[121] = GetProcAddress(hgl, "glGetPixelMapusv");
    glFuncPointers[122] = GetProcAddress(hgl, "glGetPointerv");
    glFuncPointers[123] = GetProcAddress(hgl, "glGetPolygonStipple");
    glFuncPointers[124] = GetProcAddress(hgl, "glGetString");
    glFuncPointers[125] = GetProcAddress(hgl, "glGetTexEnvfv");
    glFuncPointers[126] = GetProcAddress(hgl, "glGetTexEnviv");
    glFuncPointers[127] = GetProcAddress(hgl, "glGetTexGendv");
    glFuncPointers[128] = GetProcAddress(hgl, "glGetTexGenfv");
    glFuncPointers[129] = GetProcAddress(hgl, "glGetTexGeniv");
    glFuncPointers[130] = GetProcAddress(hgl, "glGetTexImage");
    glFuncPointers[131] = GetProcAddress(hgl, "glGetTexLevelParameterfv");
    glFuncPointers[132] = GetProcAddress(hgl, "glGetTexLevelParameteriv");
    glFuncPointers[133] = GetProcAddress(hgl, "glGetTexParameterfv");
    glFuncPointers[134] = GetProcAddress(hgl, "glGetTexParameteriv");
    glFuncPointers[135] = GetProcAddress(hgl, "glHint");
    glFuncPointers[136] = GetProcAddress(hgl, "glIndexMask");
    glFuncPointers[137] = GetProcAddress(hgl, "glIndexPointer");
    glFuncPointers[138] = GetProcAddress(hgl, "glIndexd");
    glFuncPointers[139] = GetProcAddress(hgl, "glIndexdv");
    glFuncPointers[140] = GetProcAddress(hgl, "glIndexf");
    glFuncPointers[141] = GetProcAddress(hgl, "glIndexfv");
    glFuncPointers[142] = GetProcAddress(hgl, "glIndexi");
    glFuncPointers[143] = GetProcAddress(hgl, "glIndexiv");
    glFuncPointers[144] = GetProcAddress(hgl, "glIndexs");
    glFuncPointers[145] = GetProcAddress(hgl, "glIndexsv");
    glFuncPointers[146] = GetProcAddress(hgl, "glIndexub");
    glFuncPointers[147] = GetProcAddress(hgl, "glIndexubv");
    glFuncPointers[148] = GetProcAddress(hgl, "glInitNames");
    glFuncPointers[149] = GetProcAddress(hgl, "glInterleavedArrays");
    glFuncPointers[150] = GetProcAddress(hgl, "glIsEnabled");
    glFuncPointers[151] = GetProcAddress(hgl, "glIsList");
    glFuncPointers[152] = GetProcAddress(hgl, "glIsTexture");
    glFuncPointers[153] = GetProcAddress(hgl, "glLightModelf");
    glFuncPointers[154] = GetProcAddress(hgl, "glLightModelfv");
    glFuncPointers[155] = GetProcAddress(hgl, "glLightModeli");
    glFuncPointers[156] = GetProcAddress(hgl, "glLightModeliv");
    glFuncPointers[157] = GetProcAddress(hgl, "glLightf");
    glFuncPointers[158] = GetProcAddress(hgl, "glLightfv");
    glFuncPointers[159] = GetProcAddress(hgl, "glLighti");
    glFuncPointers[160] = GetProcAddress(hgl, "glLightiv");
    glFuncPointers[161] = GetProcAddress(hgl, "glLineStipple");
    glFuncPointers[162] = GetProcAddress(hgl, "glLineWidth");
    glFuncPointers[163] = GetProcAddress(hgl, "glListBase");
    glFuncPointers[164] = GetProcAddress(hgl, "glLoadIdentity");
    glFuncPointers[165] = GetProcAddress(hgl, "glLoadMatrixd");
    glFuncPointers[166] = GetProcAddress(hgl, "glLoadMatrixf");
    glFuncPointers[167] = GetProcAddress(hgl, "glLoadName");
    glFuncPointers[168] = GetProcAddress(hgl, "glLogicOp");
    glFuncPointers[169] = GetProcAddress(hgl, "glMap1d");
    glFuncPointers[170] = GetProcAddress(hgl, "glMap1f");
    glFuncPointers[171] = GetProcAddress(hgl, "glMap2d");
    glFuncPointers[172] = GetProcAddress(hgl, "glMap2f");
    glFuncPointers[173] = GetProcAddress(hgl, "glMapGrid1d");
    glFuncPointers[174] = GetProcAddress(hgl, "glMapGrid1f");
    glFuncPointers[175] = GetProcAddress(hgl, "glMapGrid2d");
    glFuncPointers[176] = GetProcAddress(hgl, "glMapGrid2f");
    glFuncPointers[177] = GetProcAddress(hgl, "glMaterialf");
    glFuncPointers[178] = GetProcAddress(hgl, "glMaterialfv");
    glFuncPointers[179] = GetProcAddress(hgl, "glMateriali");
    glFuncPointers[180] = GetProcAddress(hgl, "glMaterialiv");
    glFuncPointers[181] = GetProcAddress(hgl, "glMatrixMode");
    glFuncPointers[182] = GetProcAddress(hgl, "glMultMatrixd");
    glFuncPointers[183] = GetProcAddress(hgl, "glMultMatrixf");
    glFuncPointers[184] = GetProcAddress(hgl, "glNewList");
    glFuncPointers[185] = GetProcAddress(hgl, "glNormal3b");
    glFuncPointers[186] = GetProcAddress(hgl, "glNormal3bv");
    glFuncPointers[187] = GetProcAddress(hgl, "glNormal3d");
    glFuncPointers[188] = GetProcAddress(hgl, "glNormal3dv");
    glFuncPointers[189] = GetProcAddress(hgl, "glNormal3f");
    glFuncPointers[190] = GetProcAddress(hgl, "glNormal3fv");
    glFuncPointers[191] = GetProcAddress(hgl, "glNormal3i");
    glFuncPointers[192] = GetProcAddress(hgl, "glNormal3iv");
    glFuncPointers[193] = GetProcAddress(hgl, "glNormal3s");
    glFuncPointers[194] = GetProcAddress(hgl, "glNormal3sv");
    glFuncPointers[195] = GetProcAddress(hgl, "glNormalPointer");
    glFuncPointers[196] = GetProcAddress(hgl, "glOrtho");
    glFuncPointers[197] = GetProcAddress(hgl, "glPassThrough");
    glFuncPointers[198] = GetProcAddress(hgl, "glPixelMapfv");
    glFuncPointers[199] = GetProcAddress(hgl, "glPixelMapuiv");
    glFuncPointers[200] = GetProcAddress(hgl, "glPixelMapusv");
    glFuncPointers[201] = GetProcAddress(hgl, "glPixelStoref");
    glFuncPointers[202] = GetProcAddress(hgl, "glPixelStorei");
    glFuncPointers[203] = GetProcAddress(hgl, "glPixelTransferf");
    glFuncPointers[204] = GetProcAddress(hgl, "glPixelTransferi");
    glFuncPointers[205] = GetProcAddress(hgl, "glPixelZoom");
    glFuncPointers[206] = GetProcAddress(hgl, "glPointSize");
    glFuncPointers[207] = GetProcAddress(hgl, "glPolygonMode");
    glFuncPointers[208] = GetProcAddress(hgl, "glPolygonOffset");
    glFuncPointers[209] = GetProcAddress(hgl, "glPolygonStipple");
    glFuncPointers[210] = GetProcAddress(hgl, "glPopAttrib");
    glFuncPointers[211] = GetProcAddress(hgl, "glPopClientAttrib");
    glFuncPointers[212] = GetProcAddress(hgl, "glPopMatrix");
    glFuncPointers[213] = GetProcAddress(hgl, "glPopName");
    glFuncPointers[214] = GetProcAddress(hgl, "glPrioritizeTextures");
    glFuncPointers[215] = GetProcAddress(hgl, "glPushAttrib");
    glFuncPointers[216] = GetProcAddress(hgl, "glPushClientAttrib");
    glFuncPointers[217] = GetProcAddress(hgl, "glPushMatrix");
    glFuncPointers[218] = GetProcAddress(hgl, "glPushName");
    glFuncPointers[219] = GetProcAddress(hgl, "glRasterPos2d");
    glFuncPointers[220] = GetProcAddress(hgl, "glRasterPos2dv");
    glFuncPointers[221] = GetProcAddress(hgl, "glRasterPos2f");
    glFuncPointers[222] = GetProcAddress(hgl, "glRasterPos2fv");
    glFuncPointers[223] = GetProcAddress(hgl, "glRasterPos2i");
    glFuncPointers[224] = GetProcAddress(hgl, "glRasterPos2iv");
    glFuncPointers[225] = GetProcAddress(hgl, "glRasterPos2s");
    glFuncPointers[226] = GetProcAddress(hgl, "glRasterPos2sv");
    glFuncPointers[227] = GetProcAddress(hgl, "glRasterPos3d");
    glFuncPointers[228] = GetProcAddress(hgl, "glRasterPos3dv");
    glFuncPointers[229] = GetProcAddress(hgl, "glRasterPos3f");
    glFuncPointers[230] = GetProcAddress(hgl, "glRasterPos3fv");
    glFuncPointers[231] = GetProcAddress(hgl, "glRasterPos3i");
    glFuncPointers[232] = GetProcAddress(hgl, "glRasterPos3iv");
    glFuncPointers[233] = GetProcAddress(hgl, "glRasterPos3s");
    glFuncPointers[234] = GetProcAddress(hgl, "glRasterPos3sv");
    glFuncPointers[235] = GetProcAddress(hgl, "glRasterPos4d");
    glFuncPointers[236] = GetProcAddress(hgl, "glRasterPos4dv");
    glFuncPointers[237] = GetProcAddress(hgl, "glRasterPos4f");
    glFuncPointers[238] = GetProcAddress(hgl, "glRasterPos4fv");
    glFuncPointers[239] = GetProcAddress(hgl, "glRasterPos4i");
    glFuncPointers[240] = GetProcAddress(hgl, "glRasterPos4iv");
    glFuncPointers[241] = GetProcAddress(hgl, "glRasterPos4s");
    glFuncPointers[242] = GetProcAddress(hgl, "glRasterPos4sv");
    glFuncPointers[243] = GetProcAddress(hgl, "glReadBuffer");
    glFuncPointers[244] = GetProcAddress(hgl, "glReadPixels");
    glFuncPointers[245] = GetProcAddress(hgl, "glRectd");
    glFuncPointers[246] = GetProcAddress(hgl, "glRectdv");
    glFuncPointers[247] = GetProcAddress(hgl, "glRectf");
    glFuncPointers[248] = GetProcAddress(hgl, "glRectfv");
    glFuncPointers[249] = GetProcAddress(hgl, "glRecti");
    glFuncPointers[250] = GetProcAddress(hgl, "glRectiv");
    glFuncPointers[251] = GetProcAddress(hgl, "glRects");
    glFuncPointers[252] = GetProcAddress(hgl, "glRectsv");
    glFuncPointers[253] = GetProcAddress(hgl, "glRenderMode");
    glFuncPointers[254] = GetProcAddress(hgl, "glRotated");
    glFuncPointers[255] = GetProcAddress(hgl, "glRotatef");
    glFuncPointers[256] = GetProcAddress(hgl, "glScaled");
    glFuncPointers[257] = GetProcAddress(hgl, "glScalef");
    glFuncPointers[258] = GetProcAddress(hgl, "glScissor");
    glFuncPointers[259] = GetProcAddress(hgl, "glSelectBuffer");
    glFuncPointers[260] = GetProcAddress(hgl, "glShadeModel");
    glFuncPointers[261] = GetProcAddress(hgl, "glStencilFunc");
    glFuncPointers[262] = GetProcAddress(hgl, "glStencilMask");
    glFuncPointers[263] = GetProcAddress(hgl, "glStencilOp");
    glFuncPointers[264] = GetProcAddress(hgl, "glTexCoord1d");
    glFuncPointers[265] = GetProcAddress(hgl, "glTexCoord1dv");
    glFuncPointers[266] = GetProcAddress(hgl, "glTexCoord1f");
    glFuncPointers[267] = GetProcAddress(hgl, "glTexCoord1fv");
    glFuncPointers[268] = GetProcAddress(hgl, "glTexCoord1i");
    glFuncPointers[269] = GetProcAddress(hgl, "glTexCoord1iv");
    glFuncPointers[270] = GetProcAddress(hgl, "glTexCoord1s");
    glFuncPointers[271] = GetProcAddress(hgl, "glTexCoord1sv");
    glFuncPointers[272] = GetProcAddress(hgl, "glTexCoord2d");
    glFuncPointers[273] = GetProcAddress(hgl, "glTexCoord2dv");
    glFuncPointers[274] = GetProcAddress(hgl, "glTexCoord2f");
    glFuncPointers[275] = GetProcAddress(hgl, "glTexCoord2fv");
    glFuncPointers[276] = GetProcAddress(hgl, "glTexCoord2i");
    glFuncPointers[277] = GetProcAddress(hgl, "glTexCoord2iv");
    glFuncPointers[278] = GetProcAddress(hgl, "glTexCoord2s");
    glFuncPointers[279] = GetProcAddress(hgl, "glTexCoord2sv");
    glFuncPointers[280] = GetProcAddress(hgl, "glTexCoord3d");
    glFuncPointers[281] = GetProcAddress(hgl, "glTexCoord3dv");
    glFuncPointers[282] = GetProcAddress(hgl, "glTexCoord3f");
    glFuncPointers[283] = GetProcAddress(hgl, "glTexCoord3fv");
    glFuncPointers[284] = GetProcAddress(hgl, "glTexCoord3i");
    glFuncPointers[285] = GetProcAddress(hgl, "glTexCoord3iv");
    glFuncPointers[286] = GetProcAddress(hgl, "glTexCoord3s");
    glFuncPointers[287] = GetProcAddress(hgl, "glTexCoord3sv");
    glFuncPointers[288] = GetProcAddress(hgl, "glTexCoord4d");
    glFuncPointers[289] = GetProcAddress(hgl, "glTexCoord4dv");
    glFuncPointers[290] = GetProcAddress(hgl, "glTexCoord4f");
    glFuncPointers[291] = GetProcAddress(hgl, "glTexCoord4fv");
    glFuncPointers[292] = GetProcAddress(hgl, "glTexCoord4i");
    glFuncPointers[293] = GetProcAddress(hgl, "glTexCoord4iv");
    glFuncPointers[294] = GetProcAddress(hgl, "glTexCoord4s");
    glFuncPointers[295] = GetProcAddress(hgl, "glTexCoord4sv");
    glFuncPointers[296] = GetProcAddress(hgl, "glTexCoordPointer");
    glFuncPointers[297] = GetProcAddress(hgl, "glTexEnvf");
    glFuncPointers[298] = GetProcAddress(hgl, "glTexEnvfv");
    glFuncPointers[299] = GetProcAddress(hgl, "glTexEnvi");
    glFuncPointers[300] = GetProcAddress(hgl, "glTexEnviv");
    glFuncPointers[301] = GetProcAddress(hgl, "glTexGend");
    glFuncPointers[302] = GetProcAddress(hgl, "glTexGendv");
    glFuncPointers[303] = GetProcAddress(hgl, "glTexGenf");
    glFuncPointers[304] = GetProcAddress(hgl, "glTexGenfv");
    glFuncPointers[305] = GetProcAddress(hgl, "glTexGeni");
    glFuncPointers[306] = GetProcAddress(hgl, "glTexGeniv");
    glFuncPointers[307] = GetProcAddress(hgl, "glTexImage1D");
    glFuncPointers[308] = GetProcAddress(hgl, "glTexImage2D");
    glFuncPointers[309] = GetProcAddress(hgl, "glTexParameterf");
    glFuncPointers[310] = GetProcAddress(hgl, "glTexParameterfv");
    glFuncPointers[311] = GetProcAddress(hgl, "glTexParameteri");
    glFuncPointers[312] = GetProcAddress(hgl, "glTexParameteriv");
    glFuncPointers[313] = GetProcAddress(hgl, "glTexSubImage1D");
    glFuncPointers[314] = GetProcAddress(hgl, "glTexSubImage2D");
    glFuncPointers[315] = GetProcAddress(hgl, "glTranslated");
    glFuncPointers[316] = GetProcAddress(hgl, "glTranslatef");
    glFuncPointers[317] = GetProcAddress(hgl, "glVertex2d");
    glFuncPointers[318] = GetProcAddress(hgl, "glVertex2dv");
    glFuncPointers[319] = GetProcAddress(hgl, "glVertex2f");
    glFuncPointers[320] = GetProcAddress(hgl, "glVertex2fv");
    glFuncPointers[321] = GetProcAddress(hgl, "glVertex2i");
    glFuncPointers[322] = GetProcAddress(hgl, "glVertex2iv");
    glFuncPointers[323] = GetProcAddress(hgl, "glVertex2s");
    glFuncPointers[324] = GetProcAddress(hgl, "glVertex2sv");
    glFuncPointers[325] = GetProcAddress(hgl, "glVertex3d");
    glFuncPointers[326] = GetProcAddress(hgl, "glVertex3dv");
    glFuncPointers[327] = GetProcAddress(hgl, "glVertex3f");
    glFuncPointers[328] = GetProcAddress(hgl, "glVertex3fv");
    glFuncPointers[329] = GetProcAddress(hgl, "glVertex3i");
    glFuncPointers[330] = GetProcAddress(hgl, "glVertex3iv");
    glFuncPointers[331] = GetProcAddress(hgl, "glVertex3s");
    glFuncPointers[332] = GetProcAddress(hgl, "glVertex3sv");
    glFuncPointers[333] = GetProcAddress(hgl, "glVertex4d");
    glFuncPointers[334] = GetProcAddress(hgl, "glVertex4dv");
    glFuncPointers[335] = GetProcAddress(hgl, "glVertex4f");
    glFuncPointers[336] = GetProcAddress(hgl, "glVertex4fv");
    glFuncPointers[337] = GetProcAddress(hgl, "glVertex4i");
    glFuncPointers[338] = GetProcAddress(hgl, "glVertex4iv");
    glFuncPointers[339] = GetProcAddress(hgl, "glVertex4s");
    glFuncPointers[340] = GetProcAddress(hgl, "glVertex4sv");
    glFuncPointers[341] = GetProcAddress(hgl, "glVertexPointer");
    glFuncPointers[342] = GetProcAddress(hgl, "glViewport");
    glFuncPointers[343] = GetProcAddress(hgl, "wglChoosePixelFormat");
    glFuncPointers[344] = GetProcAddress(hgl, "wglCopyContext");
    glFuncPointers[345] = GetProcAddress(hgl, "wglCreateContext");
    glFuncPointers[346] = GetProcAddress(hgl, "wglCreateLayerContext");
    glFuncPointers[347] = GetProcAddress(hgl, "wglDeleteContext");
    glFuncPointers[348] = GetProcAddress(hgl, "wglDescribeLayerPlane");
    glFuncPointers[349] = GetProcAddress(hgl, "wglDescribePixelFormat");
    glFuncPointers[350] = GetProcAddress(hgl, "wglGetCurrentContext");
    glFuncPointers[351] = GetProcAddress(hgl, "wglGetCurrentDC");
    glFuncPointers[352] = GetProcAddress(hgl, "wglGetDefaultProcAddress");
    glFuncPointers[353] = GetProcAddress(hgl, "wglGetLayerPaletteEntries");
    glFuncPointers[354] = GetProcAddress(hgl, "wglGetPixelFormat");
    glFuncPointers[355] = GetProcAddress(hgl, "wglGetProcAddress");
    glFuncPointers[356] = GetProcAddress(hgl, "wglMakeCurrent");
    glFuncPointers[357] = GetProcAddress(hgl, "wglRealizeLayerPalette");
    glFuncPointers[358] = GetProcAddress(hgl, "wglSetLayerPaletteEntries");
    glFuncPointers[359] = GetProcAddress(hgl, "wglSetPixelFormat");
    glFuncPointers[360] = GetProcAddress(hgl, "wglShareLists");
    glFuncPointers[361] = GetProcAddress(hgl, "wglSwapBuffers");
    glFuncPointers[362] = GetProcAddress(hgl, "wglSwapLayerBuffers");
    glFuncPointers[363] = GetProcAddress(hgl, "wglSwapMultipleBuffers");
    glFuncPointers[364] = GetProcAddress(hgl, "wglUseFontBitmapsA");
    glFuncPointers[365] = GetProcAddress(hgl, "wglUseFontBitmapsW");
    glFuncPointers[366] = GetProcAddress(hgl, "wglUseFontOutlinesA");
    glFuncPointers[367] = GetProcAddress(hgl, "wglUseFontOutlinesW");
}

#define GLHOOK(M_NUM, M_NAME)               \
void naked my##M_NAME() {                   \
    __asm call InitGL                       \
    __asm mov eax, glFuncPointers[M_NUM*4]  \
    __asm jmp eax                           \
}

GLHOOK(0, GlmfBeginGlsBlock)
GLHOOK(1, GlmfCloseMetaFile)
GLHOOK(2, GlmfEndGlsBlock)
GLHOOK(3, GlmfEndPlayback)
GLHOOK(4, GlmfInitPlayback)
GLHOOK(5, GlmfPlayGlsRecord)
GLHOOK(6, glAccum)
GLHOOK(7, glAlphaFunc)
GLHOOK(8, glAreTexturesResident)
GLHOOK(9, glArrayElement)
GLHOOK(10, glBegin)
GLHOOK(11, glBindTexture)
GLHOOK(12, glBitmap)
GLHOOK(13, glBlendFunc)
GLHOOK(14, glCallList)
GLHOOK(15, glCallLists)
GLHOOK(16, glClear)
GLHOOK(17, glClearAccum)
GLHOOK(18, glClearColor)
GLHOOK(19, glClearDepth)
GLHOOK(20, glClearIndex)
GLHOOK(21, glClearStencil)
GLHOOK(22, glClipPlane)
GLHOOK(23, glColor3b)
GLHOOK(24, glColor3bv)
GLHOOK(25, glColor3d)
GLHOOK(26, glColor3dv)
GLHOOK(27, glColor3f)
GLHOOK(28, glColor3fv)
GLHOOK(29, glColor3i)
GLHOOK(30, glColor3iv)
GLHOOK(31, glColor3s)
GLHOOK(32, glColor3sv)
GLHOOK(33, glColor3ub)
GLHOOK(34, glColor3ubv)
GLHOOK(35, glColor3ui)
GLHOOK(36, glColor3uiv)
GLHOOK(37, glColor3us)
GLHOOK(38, glColor3usv)
GLHOOK(39, glColor4b)
GLHOOK(40, glColor4bv)
GLHOOK(41, glColor4d)
GLHOOK(42, glColor4dv)
GLHOOK(43, glColor4f)
GLHOOK(44, glColor4fv)
GLHOOK(45, glColor4i)
GLHOOK(46, glColor4iv)
GLHOOK(47, glColor4s)
GLHOOK(48, glColor4sv)
GLHOOK(49, glColor4ub)
GLHOOK(50, glColor4ubv)
GLHOOK(51, glColor4ui)
GLHOOK(52, glColor4uiv)
GLHOOK(53, glColor4us)
GLHOOK(54, glColor4usv)
GLHOOK(55, glColorMask)
GLHOOK(56, glColorMaterial)
GLHOOK(57, glColorPointer)
GLHOOK(58, glCopyPixels)
GLHOOK(59, glCopyTexImage1D)
GLHOOK(60, glCopyTexImage2D)
GLHOOK(61, glCopyTexSubImage1D)
GLHOOK(62, glCopyTexSubImage2D)
GLHOOK(63, glCullFace)
GLHOOK(64, glDebugEntry)
GLHOOK(65, glDeleteLists)
GLHOOK(66, glDeleteTextures)
GLHOOK(67, glDepthFunc)
GLHOOK(68, glDepthMask)
GLHOOK(69, glDepthRange)
GLHOOK(70, glDisable)
GLHOOK(71, glDisableClientState)
GLHOOK(72, glDrawArrays)
GLHOOK(73, glDrawBuffer)
GLHOOK(74, glDrawElements)
GLHOOK(75, glDrawPixels)
GLHOOK(76, glEdgeFlag)
GLHOOK(77, glEdgeFlagPointer)
GLHOOK(78, glEdgeFlagv)
GLHOOK(79, glEnable)
GLHOOK(80, glEnableClientState)
GLHOOK(81, glEnd)
GLHOOK(82, glEndList)
GLHOOK(83, glEvalCoord1d)
GLHOOK(84, glEvalCoord1dv)
GLHOOK(85, glEvalCoord1f)
GLHOOK(86, glEvalCoord1fv)
GLHOOK(87, glEvalCoord2d)
GLHOOK(88, glEvalCoord2dv)
GLHOOK(89, glEvalCoord2f)
GLHOOK(90, glEvalCoord2fv)
GLHOOK(91, glEvalMesh1)
GLHOOK(92, glEvalMesh2)
GLHOOK(93, glEvalPoint1)
GLHOOK(94, glEvalPoint2)
GLHOOK(95, glFeedbackBuffer)
GLHOOK(96, glFinish)
GLHOOK(97, glFlush)
GLHOOK(98, glFogf)
GLHOOK(99, glFogfv)
GLHOOK(100, glFogi)
GLHOOK(101, glFogiv)
GLHOOK(102, glFrontFace)
GLHOOK(103, glFrustum)
GLHOOK(104, glGenLists)
GLHOOK(105, glGenTextures)
GLHOOK(106, glGetBooleanv)
GLHOOK(107, glGetClipPlane)
GLHOOK(108, glGetDoublev)
GLHOOK(109, glGetError)
GLHOOK(110, glGetFloatv)
GLHOOK(111, glGetIntegerv)
GLHOOK(112, glGetLightfv)
GLHOOK(113, glGetLightiv)
GLHOOK(114, glGetMapdv)
GLHOOK(115, glGetMapfv)
GLHOOK(116, glGetMapiv)
GLHOOK(117, glGetMaterialfv)
GLHOOK(118, glGetMaterialiv)
GLHOOK(119, glGetPixelMapfv)
GLHOOK(120, glGetPixelMapuiv)
GLHOOK(121, glGetPixelMapusv)
GLHOOK(122, glGetPointerv)
GLHOOK(123, glGetPolygonStipple)
GLHOOK(124, glGetString)
GLHOOK(125, glGetTexEnvfv)
GLHOOK(126, glGetTexEnviv)
GLHOOK(127, glGetTexGendv)
GLHOOK(128, glGetTexGenfv)
GLHOOK(129, glGetTexGeniv)
GLHOOK(130, glGetTexImage)
GLHOOK(131, glGetTexLevelParameterfv)
GLHOOK(132, glGetTexLevelParameteriv)
GLHOOK(133, glGetTexParameterfv)
GLHOOK(134, glGetTexParameteriv)
GLHOOK(135, glHint)
GLHOOK(136, glIndexMask)
GLHOOK(137, glIndexPointer)
GLHOOK(138, glIndexd)
GLHOOK(139, glIndexdv)
GLHOOK(140, glIndexf)
GLHOOK(141, glIndexfv)
GLHOOK(142, glIndexi)
GLHOOK(143, glIndexiv)
GLHOOK(144, glIndexs)
GLHOOK(145, glIndexsv)
GLHOOK(146, glIndexub)
GLHOOK(147, glIndexubv)
GLHOOK(148, glInitNames)
GLHOOK(149, glInterleavedArrays)
GLHOOK(150, glIsEnabled)
GLHOOK(151, glIsList)
GLHOOK(152, glIsTexture)
GLHOOK(153, glLightModelf)
GLHOOK(154, glLightModelfv)
GLHOOK(155, glLightModeli)
GLHOOK(156, glLightModeliv)
GLHOOK(157, glLightf)
GLHOOK(158, glLightfv)
GLHOOK(159, glLighti)
GLHOOK(160, glLightiv)
GLHOOK(161, glLineStipple)
GLHOOK(162, glLineWidth)
GLHOOK(163, glListBase)
GLHOOK(164, glLoadIdentity)
GLHOOK(165, glLoadMatrixd)
GLHOOK(166, glLoadMatrixf)
GLHOOK(167, glLoadName)
GLHOOK(168, glLogicOp)
GLHOOK(169, glMap1d)
GLHOOK(170, glMap1f)
GLHOOK(171, glMap2d)
GLHOOK(172, glMap2f)
GLHOOK(173, glMapGrid1d)
GLHOOK(174, glMapGrid1f)
GLHOOK(175, glMapGrid2d)
GLHOOK(176, glMapGrid2f)
GLHOOK(177, glMaterialf)
GLHOOK(178, glMaterialfv)
GLHOOK(179, glMateriali)
GLHOOK(180, glMaterialiv)
GLHOOK(181, glMatrixMode)
GLHOOK(182, glMultMatrixd)
GLHOOK(183, glMultMatrixf)
GLHOOK(184, glNewList)
GLHOOK(185, glNormal3b)
GLHOOK(186, glNormal3bv)
GLHOOK(187, glNormal3d)
GLHOOK(188, glNormal3dv)
GLHOOK(189, glNormal3f)
GLHOOK(190, glNormal3fv)
GLHOOK(191, glNormal3i)
GLHOOK(192, glNormal3iv)
GLHOOK(193, glNormal3s)
GLHOOK(194, glNormal3sv)
GLHOOK(195, glNormalPointer)
GLHOOK(196, glOrtho)
GLHOOK(197, glPassThrough)
GLHOOK(198, glPixelMapfv)
GLHOOK(199, glPixelMapuiv)
GLHOOK(200, glPixelMapusv)
GLHOOK(201, glPixelStoref)
GLHOOK(202, glPixelStorei)
GLHOOK(203, glPixelTransferf)
GLHOOK(204, glPixelTransferi)
GLHOOK(205, glPixelZoom)
GLHOOK(206, glPointSize)
GLHOOK(207, glPolygonMode)
GLHOOK(208, glPolygonOffset)
GLHOOK(209, glPolygonStipple)
GLHOOK(210, glPopAttrib)
GLHOOK(211, glPopClientAttrib)
GLHOOK(212, glPopMatrix)
GLHOOK(213, glPopName)
GLHOOK(214, glPrioritizeTextures)
GLHOOK(215, glPushAttrib)
GLHOOK(216, glPushClientAttrib)
GLHOOK(217, glPushMatrix)
GLHOOK(218, glPushName)
GLHOOK(219, glRasterPos2d)
GLHOOK(220, glRasterPos2dv)
GLHOOK(221, glRasterPos2f)
GLHOOK(222, glRasterPos2fv)
GLHOOK(223, glRasterPos2i)
GLHOOK(224, glRasterPos2iv)
GLHOOK(225, glRasterPos2s)
GLHOOK(226, glRasterPos2sv)
GLHOOK(227, glRasterPos3d)
GLHOOK(228, glRasterPos3dv)
GLHOOK(229, glRasterPos3f)
GLHOOK(230, glRasterPos3fv)
GLHOOK(231, glRasterPos3i)
GLHOOK(232, glRasterPos3iv)
GLHOOK(233, glRasterPos3s)
GLHOOK(234, glRasterPos3sv)
GLHOOK(235, glRasterPos4d)
GLHOOK(236, glRasterPos4dv)
GLHOOK(237, glRasterPos4f)
GLHOOK(238, glRasterPos4fv)
GLHOOK(239, glRasterPos4i)
GLHOOK(240, glRasterPos4iv)
GLHOOK(241, glRasterPos4s)
GLHOOK(242, glRasterPos4sv)
GLHOOK(243, glReadBuffer)
GLHOOK(244, glReadPixels)
GLHOOK(245, glRectd)
GLHOOK(246, glRectdv)
GLHOOK(247, glRectf)
GLHOOK(248, glRectfv)
GLHOOK(249, glRecti)
GLHOOK(250, glRectiv)
GLHOOK(251, glRects)
GLHOOK(252, glRectsv)
GLHOOK(253, glRenderMode)
GLHOOK(254, glRotated)
GLHOOK(255, glRotatef)
GLHOOK(256, glScaled)
GLHOOK(257, glScalef)
GLHOOK(258, glScissor)
GLHOOK(259, glSelectBuffer)
GLHOOK(260, glShadeModel)
GLHOOK(261, glStencilFunc)
GLHOOK(262, glStencilMask)
GLHOOK(263, glStencilOp)
GLHOOK(264, glTexCoord1d)
GLHOOK(265, glTexCoord1dv)
GLHOOK(266, glTexCoord1f)
GLHOOK(267, glTexCoord1fv)
GLHOOK(268, glTexCoord1i)
GLHOOK(269, glTexCoord1iv)
GLHOOK(270, glTexCoord1s)
GLHOOK(271, glTexCoord1sv)
GLHOOK(272, glTexCoord2d)
GLHOOK(273, glTexCoord2dv)
GLHOOK(274, glTexCoord2f)
GLHOOK(275, glTexCoord2fv)
GLHOOK(276, glTexCoord2i)
GLHOOK(277, glTexCoord2iv)
GLHOOK(278, glTexCoord2s)
GLHOOK(279, glTexCoord2sv)
GLHOOK(280, glTexCoord3d)
GLHOOK(281, glTexCoord3dv)
GLHOOK(282, glTexCoord3f)
GLHOOK(283, glTexCoord3fv)
GLHOOK(284, glTexCoord3i)
GLHOOK(285, glTexCoord3iv)
GLHOOK(286, glTexCoord3s)
GLHOOK(287, glTexCoord3sv)
GLHOOK(288, glTexCoord4d)
GLHOOK(289, glTexCoord4dv)
GLHOOK(290, glTexCoord4f)
GLHOOK(291, glTexCoord4fv)
GLHOOK(292, glTexCoord4i)
GLHOOK(293, glTexCoord4iv)
GLHOOK(294, glTexCoord4s)
GLHOOK(295, glTexCoord4sv)
GLHOOK(296, glTexCoordPointer)
GLHOOK(297, glTexEnvf)
GLHOOK(298, glTexEnvfv)
GLHOOK(299, glTexEnvi)
GLHOOK(300, glTexEnviv)
GLHOOK(301, glTexGend)
GLHOOK(302, glTexGendv)
GLHOOK(303, glTexGenf)
GLHOOK(304, glTexGenfv)
GLHOOK(305, glTexGeni)
GLHOOK(306, glTexGeniv)
GLHOOK(307, glTexImage1D)
GLHOOK(308, glTexImage2D)
GLHOOK(309, glTexParameterf)
GLHOOK(310, glTexParameterfv)
GLHOOK(311, glTexParameteri)
GLHOOK(312, glTexParameteriv)
GLHOOK(313, glTexSubImage1D)
GLHOOK(314, glTexSubImage2D)
GLHOOK(315, glTranslated)
GLHOOK(316, glTranslatef)
GLHOOK(317, glVertex2d)
GLHOOK(318, glVertex2dv)
GLHOOK(319, glVertex2f)
GLHOOK(320, glVertex2fv)
GLHOOK(321, glVertex2i)
GLHOOK(322, glVertex2iv)
GLHOOK(323, glVertex2s)
GLHOOK(324, glVertex2sv)
GLHOOK(325, glVertex3d)
GLHOOK(326, glVertex3dv)
GLHOOK(327, glVertex3f)
GLHOOK(328, glVertex3fv)
GLHOOK(329, glVertex3i)
GLHOOK(330, glVertex3iv)
GLHOOK(331, glVertex3s)
GLHOOK(332, glVertex3sv)
GLHOOK(333, glVertex4d)
GLHOOK(334, glVertex4dv)
GLHOOK(335, glVertex4f)
GLHOOK(336, glVertex4fv)
GLHOOK(337, glVertex4i)
GLHOOK(338, glVertex4iv)
GLHOOK(339, glVertex4s)
GLHOOK(340, glVertex4sv)
GLHOOK(341, glVertexPointer)
GLHOOK(342, glViewport)
GLHOOK(343, wglChoosePixelFormat)
GLHOOK(344, wglCopyContext)
GLHOOK(345, wglCreateContext)
GLHOOK(346, wglCreateLayerContext)
GLHOOK(347, wglDeleteContext)
GLHOOK(348, wglDescribeLayerPlane)
GLHOOK(349, wglDescribePixelFormat)
GLHOOK(350, wglGetCurrentContext)
GLHOOK(351, wglGetCurrentDC)
GLHOOK(352, wglGetDefaultProcAddress)
GLHOOK(353, wglGetLayerPaletteEntries)
GLHOOK(354, wglGetPixelFormat)
GLHOOK(355, wglGetProcAddress)
GLHOOK(356, wglMakeCurrent)
GLHOOK(357, wglRealizeLayerPalette)
GLHOOK(358, wglSetLayerPaletteEntries)
GLHOOK(359, wglSetPixelFormat)
GLHOOK(360, wglShareLists)
GLHOOK(361, wglSwapBuffers)
GLHOOK(362, wglSwapLayerBuffers)
GLHOOK(363, wglSwapMultipleBuffers)
GLHOOK(364, wglUseFontBitmapsA)
GLHOOK(365, wglUseFontBitmapsW)
GLHOOK(366, wglUseFontOutlinesA)
GLHOOK(367, wglUseFontOutlinesW)

#endif