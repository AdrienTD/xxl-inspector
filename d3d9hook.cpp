#ifndef REMASTER

#include "global.h"

typedef IDirect3D9* (WINAPI *ftDirect3DCreate9)(int SDKVersion);

HMODULE d3d9 = 0;
ftDirect3DCreate9 oriDirect3DCreate9;

IDirect3D9 *WINAPI myDirect3DCreate9(int SDKVersion)
{
	wchar_t tbuf[256]; HDirect3D9 *hd; IDirect3D9 *id;
	if(!d3d9)
	{
		if(_access("apd3d9.dll", 0) != -1) // If apd3d9.dll exists.
		{
			d3d9 = LoadLibraryA("apd3d9.dll");
		}
		else
		{
			GetSystemDirectoryW(tbuf, 255);
			wcscat_s(tbuf, 255, L"\\d3d9.dll");
			d3d9 = LoadLibraryW(tbuf);
		}
	}
	oriDirect3DCreate9 = (ftDirect3DCreate9)GetProcAddress(d3d9, "Direct3DCreate9");
	id = oriDirect3DCreate9(SDKVersion);
	if(!id) return 0;
	hd = (HDirect3D9*)malloc(sizeof(HDirect3D9));
	hd->lpVtbl = odmvtbld;
	hd->original = id;
	return (IDirect3D9*)hd;
}

IDirect3DDevice9 *gamed3ddev = 0;

HRESULT STDMETHODCALLTYPE DDQueryInterface(HDirect3D9 *This, REFIID a, void** b) {return This->original->lpVtbl->QueryInterface(This->original,a,b);}
ULONG STDMETHODCALLTYPE DDAddRef(HDirect3D9 *This) {return This->original->lpVtbl->AddRef(This->original);}
ULONG STDMETHODCALLTYPE DDRelease(HDirect3D9 *This) {return This->original->lpVtbl->Release(This->original);}
HRESULT STDMETHODCALLTYPE DDRegisterSoftwareDevice(HDirect3D9 *This, void* a) {return This->original->lpVtbl->RegisterSoftwareDevice(This->original,a);}
UINT STDMETHODCALLTYPE DDGetAdapterCount(HDirect3D9 *This) {return This->original->lpVtbl->GetAdapterCount(This->original);}
HRESULT STDMETHODCALLTYPE DDGetAdapterIdentifier(HDirect3D9 *This, UINT a,DWORD b,D3DADAPTER_IDENTIFIER9* c) {return This->original->lpVtbl->GetAdapterIdentifier(This->original,a,b,c);}
UINT STDMETHODCALLTYPE DDGetAdapterModeCount(HDirect3D9 *This, UINT a,D3DFORMAT b) {return This->original->lpVtbl->GetAdapterModeCount(This->original,a,b);}
HRESULT STDMETHODCALLTYPE DDEnumAdapterModes(HDirect3D9 *This, UINT a,D3DFORMAT b,UINT c,D3DDISPLAYMODE* d) {return This->original->lpVtbl->EnumAdapterModes(This->original,a,b,c,d);}
HRESULT STDMETHODCALLTYPE DDGetAdapterDisplayMode(HDirect3D9 *This, UINT a,D3DDISPLAYMODE* b) {return This->original->lpVtbl->GetAdapterDisplayMode(This->original,a,b);}
HRESULT STDMETHODCALLTYPE DDCheckDeviceType(HDirect3D9 *This, UINT a,D3DDEVTYPE b,D3DFORMAT c,D3DFORMAT d,BOOL e) {return This->original->lpVtbl->CheckDeviceType(This->original,a,b,c,d,e);}
HRESULT STDMETHODCALLTYPE DDCheckDeviceFormat(HDirect3D9 *This, UINT a,D3DDEVTYPE b,D3DFORMAT c,DWORD d,D3DRESOURCETYPE e,D3DFORMAT f) {return This->original->lpVtbl->CheckDeviceFormat(This->original,a,b,c,d,e,f);}
HRESULT STDMETHODCALLTYPE DDCheckDeviceMultiSampleType(HDirect3D9 *This, UINT a,D3DDEVTYPE b,D3DFORMAT c,BOOL d,D3DMULTISAMPLE_TYPE e,DWORD* f) {return This->original->lpVtbl->CheckDeviceMultiSampleType(This->original,a,b,c,d,e,f);}
HRESULT STDMETHODCALLTYPE DDCheckDepthStencilMatch(HDirect3D9 *This, UINT a,D3DDEVTYPE b,D3DFORMAT c,D3DFORMAT d,D3DFORMAT e) {return This->original->lpVtbl->CheckDepthStencilMatch(This->original,a,b,c,d,e);}
HRESULT STDMETHODCALLTYPE DDCheckDeviceFormatConversion(HDirect3D9 *This, UINT a,D3DDEVTYPE b,D3DFORMAT c,D3DFORMAT d) {return This->original->lpVtbl->CheckDeviceFormatConversion(This->original,a,b,c,d);}
HRESULT STDMETHODCALLTYPE DDGetDeviceCaps(HDirect3D9 *This, UINT a,D3DDEVTYPE b,D3DCAPS9* c) {return This->original->lpVtbl->GetDeviceCaps(This->original,a,b,c);}
HMONITOR STDMETHODCALLTYPE DDGetAdapterMonitor(HDirect3D9 *This, UINT a) {return This->original->lpVtbl->GetAdapterMonitor(This->original,a);}
//HRESULT STDMETHODCALLTYPE DDCreateDevice(HDirect3D9 *This, UINT a,D3DDEVTYPE b,HWND c,DWORD d,D3DPRESENT_PARAMETERS* e,IDirect3DDevice9** f) {return This->original->lpVtbl->CreateDevice(This->original,a,b,c,d,e,f);}
/*
HRESULT STDMETHODCALLTYPE DDGetAdapterDisplayMode(HDirect3D9 *This, UINT a,D3DDISPLAYMODE* b)
{
	sprintf(tbuf, "id = %i", This->original); MessageBox(0, tbuf, title, 64);
	return This->original->lpVtbl->GetAdapterDisplayMode(This->original,a,b);
}
*/
HRESULT STDMETHODCALLTYPE DDCreateDevice(HDirect3D9 *This, UINT a,D3DDEVTYPE b,HWND c,DWORD d,D3DPRESENT_PARAMETERS* e,IDirect3DDevice9** f)
{
	HRESULT r; HDirect3DDevice9 *hd; IDirect3DDevice9 *id;
	r = This->original->lpVtbl->CreateDevice(This->original,a,b,c,d,e,&id);
	if(FAILED(r)) return r;
	hd = (HDirect3DDevice9 *)malloc(sizeof(HDirect3DDevice9));
	hd->lpVtbl = devvtbld;
	hd->original = id;
	hd->d3d = This;
	*f = (IDirect3DDevice9*)hd;
	//MessageBox(0, "Device created!", title, 64);
	if(logfile) fprintf(logfile, "Created device %p -> %p\n", *f, id);
	gamed3ddev = id;
	IGInit();
	return r;
}

HRESULT STDMETHODCALLTYPE DEVQueryInterface(HDirect3DDevice9 *This, REFIID riid, void** ppvObj) {return This->original->lpVtbl->QueryInterface(This->original, riid, ppvObj);}
ULONG STDMETHODCALLTYPE DEVAddRef(HDirect3DDevice9 *This) {return This->original->lpVtbl->AddRef(This->original);}
ULONG STDMETHODCALLTYPE DEVRelease(HDirect3DDevice9 *This) {return This->original->lpVtbl->Release(This->original);}
HRESULT STDMETHODCALLTYPE DEVTestCooperativeLevel(HDirect3DDevice9 *This) {return This->original->lpVtbl->TestCooperativeLevel(This->original);}
UINT STDMETHODCALLTYPE DEVGetAvailableTextureMem(HDirect3DDevice9 *This) {return This->original->lpVtbl->GetAvailableTextureMem(This->original);}
HRESULT STDMETHODCALLTYPE DEVEvictManagedResources(HDirect3DDevice9 *This) {return This->original->lpVtbl->EvictManagedResources(This->original);}
//HRESULT STDMETHODCALLTYPE DEVGetDirect3D(HDirect3DDevice9 *This, IDirect3D9** ppD3D9) {return This->original->lpVtbl->GetDirect3D(This->original, ppD3D9);}
HRESULT STDMETHODCALLTYPE DEVGetDeviceCaps(HDirect3DDevice9 *This, D3DCAPS9* pCaps) {return This->original->lpVtbl->GetDeviceCaps(This->original, pCaps);}
HRESULT STDMETHODCALLTYPE DEVGetDisplayMode(HDirect3DDevice9 *This, UINT iSwapChain,D3DDISPLAYMODE* pMode) {return This->original->lpVtbl->GetDisplayMode(This->original, iSwapChain, pMode);}
HRESULT STDMETHODCALLTYPE DEVGetCreationParameters(HDirect3DDevice9 *This, D3DDEVICE_CREATION_PARAMETERS *pParameters) {return This->original->lpVtbl->GetCreationParameters(This->original, pParameters);}
HRESULT STDMETHODCALLTYPE DEVSetCursorProperties(HDirect3DDevice9 *This, UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap) {return This->original->lpVtbl->SetCursorProperties(This->original, XHotSpot, YHotSpot, pCursorBitmap);}
void STDMETHODCALLTYPE DEVSetCursorPosition(HDirect3DDevice9 *This, int X,int Y,DWORD Flags) {This->original->lpVtbl->SetCursorPosition(This->original, X, Y, Flags);}
BOOL STDMETHODCALLTYPE DEVShowCursor(HDirect3DDevice9 *This, BOOL bShow) {return This->original->lpVtbl->ShowCursor(This->original, bShow);}
HRESULT STDMETHODCALLTYPE DEVCreateAdditionalSwapChain(HDirect3DDevice9 *This, D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain) {return This->original->lpVtbl->CreateAdditionalSwapChain(This->original, pPresentationParameters, pSwapChain);}
HRESULT STDMETHODCALLTYPE DEVGetSwapChain(HDirect3DDevice9 *This, UINT iSwapChain,IDirect3DSwapChain9** pSwapChain) {return This->original->lpVtbl->GetSwapChain(This->original, iSwapChain, pSwapChain);}
UINT STDMETHODCALLTYPE DEVGetNumberOfSwapChains(HDirect3DDevice9 *This) {return This->original->lpVtbl->GetNumberOfSwapChains(This->original);}
//HRESULT STDMETHODCALLTYPE DEVReset(HDirect3DDevice9 *This, D3DPRESENT_PARAMETERS* pPresentationParameters) {return This->original->lpVtbl->Reset(This->original, pPresentationParameters);}
HRESULT STDMETHODCALLTYPE DEVPresent(HDirect3DDevice9 *This, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) {return This->original->lpVtbl->Present(This->original, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);}
HRESULT STDMETHODCALLTYPE DEVGetBackBuffer(HDirect3DDevice9 *This, UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) {return This->original->lpVtbl->GetBackBuffer(This->original, iSwapChain, iBackBuffer, Type, ppBackBuffer);}
HRESULT STDMETHODCALLTYPE DEVGetRasterStatus(HDirect3DDevice9 *This, UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus) {return This->original->lpVtbl->GetRasterStatus(This->original, iSwapChain, pRasterStatus);}
HRESULT STDMETHODCALLTYPE DEVSetDialogBoxMode(HDirect3DDevice9 *This, BOOL bEnableDialogs) {return This->original->lpVtbl->SetDialogBoxMode(This->original, bEnableDialogs);}
void STDMETHODCALLTYPE DEVSetGammaRamp(HDirect3DDevice9 *This, UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp) {This->original->lpVtbl->SetGammaRamp(This->original, iSwapChain, Flags, pRamp);}
void STDMETHODCALLTYPE DEVGetGammaRamp(HDirect3DDevice9 *This, UINT iSwapChain,D3DGAMMARAMP* pRamp) {This->original->lpVtbl->GetGammaRamp(This->original, iSwapChain, pRamp);}
HRESULT STDMETHODCALLTYPE DEVCreateTexture(HDirect3DDevice9 *This, UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateTexture(This->original, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVCreateVolumeTexture(HDirect3DDevice9 *This, UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateVolumeTexture(This->original, Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVCreateCubeTexture(HDirect3DDevice9 *This, UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateCubeTexture(This->original, EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVCreateVertexBuffer(HDirect3DDevice9 *This, UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateVertexBuffer(This->original, Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVCreateIndexBuffer(HDirect3DDevice9 *This, UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateIndexBuffer(This->original, Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVCreateRenderTarget(HDirect3DDevice9 *This, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateRenderTarget(This->original, Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVCreateDepthStencilSurface(HDirect3DDevice9 *This, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateDepthStencilSurface(This->original, Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVUpdateSurface(HDirect3DDevice9 *This, IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) {return This->original->lpVtbl->UpdateSurface(This->original, pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);}
HRESULT STDMETHODCALLTYPE DEVUpdateTexture(HDirect3DDevice9 *This, IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture) {return This->original->lpVtbl->UpdateTexture(This->original, pSourceTexture, pDestinationTexture);}
HRESULT STDMETHODCALLTYPE DEVGetRenderTargetData(HDirect3DDevice9 *This, IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface) {return This->original->lpVtbl->GetRenderTargetData(This->original, pRenderTarget, pDestSurface);}
HRESULT STDMETHODCALLTYPE DEVGetFrontBufferData(HDirect3DDevice9 *This, UINT iSwapChain,IDirect3DSurface9* pDestSurface) {return This->original->lpVtbl->GetFrontBufferData(This->original, iSwapChain, pDestSurface);}
HRESULT STDMETHODCALLTYPE DEVStretchRect(HDirect3DDevice9 *This, IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter) {return This->original->lpVtbl->StretchRect(This->original, pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);}
HRESULT STDMETHODCALLTYPE DEVColorFill(HDirect3DDevice9 *This, IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color) {return This->original->lpVtbl->ColorFill(This->original, pSurface, pRect, color);}
HRESULT STDMETHODCALLTYPE DEVCreateOffscreenPlainSurface(HDirect3DDevice9 *This, UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) {return This->original->lpVtbl->CreateOffscreenPlainSurface(This->original, Width, Height, Format, Pool, ppSurface, pSharedHandle);}
HRESULT STDMETHODCALLTYPE DEVSetRenderTarget(HDirect3DDevice9 *This, DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget) {return This->original->lpVtbl->SetRenderTarget(This->original, RenderTargetIndex, pRenderTarget);}
HRESULT STDMETHODCALLTYPE DEVGetRenderTarget(HDirect3DDevice9 *This, DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget) {return This->original->lpVtbl->GetRenderTarget(This->original, RenderTargetIndex, ppRenderTarget);}
HRESULT STDMETHODCALLTYPE DEVSetDepthStencilSurface(HDirect3DDevice9 *This, IDirect3DSurface9* pNewZStencil) {return This->original->lpVtbl->SetDepthStencilSurface(This->original, pNewZStencil);}
HRESULT STDMETHODCALLTYPE DEVGetDepthStencilSurface(HDirect3DDevice9 *This, IDirect3DSurface9** ppZStencilSurface) {return This->original->lpVtbl->GetDepthStencilSurface(This->original, ppZStencilSurface);}
//HRESULT STDMETHODCALLTYPE DEVBeginScene(HDirect3DDevice9 *This) {return This->original->lpVtbl->BeginScene(This->original);}
//HRESULT STDMETHODCALLTYPE DEVEndScene(HDirect3DDevice9 *This) {return This->original->lpVtbl->EndScene(This->original);}
HRESULT STDMETHODCALLTYPE DEVClear(HDirect3DDevice9 *This, DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil) {return This->original->lpVtbl->Clear(This->original, Count, pRects, Flags, Color, Z, Stencil);}
HRESULT STDMETHODCALLTYPE DEVSetTransform(HDirect3DDevice9 *This, D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) {return This->original->lpVtbl->SetTransform(This->original, State, pMatrix);}
HRESULT STDMETHODCALLTYPE DEVGetTransform(HDirect3DDevice9 *This, D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix) {return This->original->lpVtbl->GetTransform(This->original, State, pMatrix);}
HRESULT STDMETHODCALLTYPE DEVMultiplyTransform(HDirect3DDevice9 *This, D3DTRANSFORMSTATETYPE a,CONST D3DMATRIX* b) {return This->original->lpVtbl->MultiplyTransform(This->original, a, b);}
HRESULT STDMETHODCALLTYPE DEVSetViewport(HDirect3DDevice9 *This, CONST D3DVIEWPORT9* pViewport) {return This->original->lpVtbl->SetViewport(This->original, pViewport);}
HRESULT STDMETHODCALLTYPE DEVGetViewport(HDirect3DDevice9 *This, D3DVIEWPORT9* pViewport) {return This->original->lpVtbl->GetViewport(This->original, pViewport);}
HRESULT STDMETHODCALLTYPE DEVSetMaterial(HDirect3DDevice9 *This, CONST D3DMATERIAL9* pMaterial) {return This->original->lpVtbl->SetMaterial(This->original, pMaterial);}
HRESULT STDMETHODCALLTYPE DEVGetMaterial(HDirect3DDevice9 *This, D3DMATERIAL9* pMaterial) {return This->original->lpVtbl->GetMaterial(This->original, pMaterial);}
HRESULT STDMETHODCALLTYPE DEVSetLight(HDirect3DDevice9 *This, DWORD Index,CONST D3DLIGHT9*c) {return This->original->lpVtbl->SetLight(This->original, Index, c);}
HRESULT STDMETHODCALLTYPE DEVGetLight(HDirect3DDevice9 *This, DWORD Index,D3DLIGHT9*c) {return This->original->lpVtbl->GetLight(This->original, Index, c);}
HRESULT STDMETHODCALLTYPE DEVLightEnable(HDirect3DDevice9 *This, DWORD Index,BOOL Enable) {return This->original->lpVtbl->LightEnable(This->original, Index, Enable);}
HRESULT STDMETHODCALLTYPE DEVGetLightEnable(HDirect3DDevice9 *This, DWORD Index,BOOL* pEnable) {return This->original->lpVtbl->GetLightEnable(This->original, Index, pEnable);}
HRESULT STDMETHODCALLTYPE DEVSetClipPlane(HDirect3DDevice9 *This, DWORD Index,CONST float* pPlane) {return This->original->lpVtbl->SetClipPlane(This->original, Index, pPlane);}
HRESULT STDMETHODCALLTYPE DEVGetClipPlane(HDirect3DDevice9 *This, DWORD Index,float* pPlane) {return This->original->lpVtbl->GetClipPlane(This->original, Index, pPlane);}
HRESULT STDMETHODCALLTYPE DEVSetRenderState(HDirect3DDevice9 *This, D3DRENDERSTATETYPE State,DWORD Value) {return This->original->lpVtbl->SetRenderState(This->original, State, Value);}
HRESULT STDMETHODCALLTYPE DEVGetRenderState(HDirect3DDevice9 *This, D3DRENDERSTATETYPE State,DWORD* pValue) {return This->original->lpVtbl->GetRenderState(This->original, State, pValue);}
HRESULT STDMETHODCALLTYPE DEVCreateStateBlock(HDirect3DDevice9 *This, D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB) {return This->original->lpVtbl->CreateStateBlock(This->original, Type, ppSB);}
HRESULT STDMETHODCALLTYPE DEVBeginStateBlock(HDirect3DDevice9 *This) {return This->original->lpVtbl->BeginStateBlock(This->original);}
HRESULT STDMETHODCALLTYPE DEVEndStateBlock(HDirect3DDevice9 *This, IDirect3DStateBlock9** ppSB) {return This->original->lpVtbl->EndStateBlock(This->original, ppSB);}
HRESULT STDMETHODCALLTYPE DEVSetClipStatus(HDirect3DDevice9 *This, CONST D3DCLIPSTATUS9* pClipStatus) {return This->original->lpVtbl->SetClipStatus(This->original, pClipStatus);}
HRESULT STDMETHODCALLTYPE DEVGetClipStatus(HDirect3DDevice9 *This, D3DCLIPSTATUS9* pClipStatus) {return This->original->lpVtbl->GetClipStatus(This->original, pClipStatus);}
HRESULT STDMETHODCALLTYPE DEVGetTexture(HDirect3DDevice9 *This, DWORD Stage,IDirect3DBaseTexture9** ppTexture) {return This->original->lpVtbl->GetTexture(This->original, Stage, ppTexture);}
HRESULT STDMETHODCALLTYPE DEVSetTexture(HDirect3DDevice9 *This, DWORD Stage,IDirect3DBaseTexture9* pTexture) {return This->original->lpVtbl->SetTexture(This->original, Stage, pTexture);}
HRESULT STDMETHODCALLTYPE DEVGetTextureStageState(HDirect3DDevice9 *This, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue) {return This->original->lpVtbl->GetTextureStageState(This->original, Stage, Type, pValue);}
HRESULT STDMETHODCALLTYPE DEVSetTextureStageState(HDirect3DDevice9 *This, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value) {return This->original->lpVtbl->SetTextureStageState(This->original, Stage, Type, Value);}
HRESULT STDMETHODCALLTYPE DEVGetSamplerState(HDirect3DDevice9 *This, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue) {return This->original->lpVtbl->GetSamplerState(This->original, Sampler, Type, pValue);}
HRESULT STDMETHODCALLTYPE DEVSetSamplerState(HDirect3DDevice9 *This, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value) {return This->original->lpVtbl->SetSamplerState(This->original, Sampler, Type, Value);}
HRESULT STDMETHODCALLTYPE DEVValidateDevice(HDirect3DDevice9 *This, DWORD* pNumPasses) {return This->original->lpVtbl->ValidateDevice(This->original, pNumPasses);}
HRESULT STDMETHODCALLTYPE DEVSetPaletteEntries(HDirect3DDevice9 *This, UINT PaletteNumber,CONST PALETTEENTRY* pEntries) {return This->original->lpVtbl->SetPaletteEntries(This->original, PaletteNumber, pEntries);}
HRESULT STDMETHODCALLTYPE DEVGetPaletteEntries(HDirect3DDevice9 *This, UINT PaletteNumber,PALETTEENTRY* pEntries) {return This->original->lpVtbl->GetPaletteEntries(This->original, PaletteNumber, pEntries);}
HRESULT STDMETHODCALLTYPE DEVSetCurrentTexturePalette(HDirect3DDevice9 *This, UINT PaletteNumber) {return This->original->lpVtbl->SetCurrentTexturePalette(This->original, PaletteNumber);}
HRESULT STDMETHODCALLTYPE DEVGetCurrentTexturePalette(HDirect3DDevice9 *This, UINT *PaletteNumber) {return This->original->lpVtbl->GetCurrentTexturePalette(This->original, PaletteNumber);}
HRESULT STDMETHODCALLTYPE DEVSetScissorRect(HDirect3DDevice9 *This, CONST RECT* pRect) {return This->original->lpVtbl->SetScissorRect(This->original, pRect);}
HRESULT STDMETHODCALLTYPE DEVGetScissorRect(HDirect3DDevice9 *This, RECT* pRect) {return This->original->lpVtbl->GetScissorRect(This->original, pRect);}
HRESULT STDMETHODCALLTYPE DEVSetSoftwareVertexProcessing(HDirect3DDevice9 *This, BOOL bSoftware) {return This->original->lpVtbl->SetSoftwareVertexProcessing(This->original, bSoftware);}
BOOL STDMETHODCALLTYPE DEVGetSoftwareVertexProcessing(HDirect3DDevice9 *This) {return This->original->lpVtbl->GetSoftwareVertexProcessing(This->original);}
HRESULT STDMETHODCALLTYPE DEVSetNPatchMode(HDirect3DDevice9 *This, float nSegments) {return This->original->lpVtbl->SetNPatchMode(This->original, nSegments);}
float STDMETHODCALLTYPE DEVGetNPatchMode(HDirect3DDevice9 *This) {return This->original->lpVtbl->GetNPatchMode(This->original);}
HRESULT STDMETHODCALLTYPE DEVDrawPrimitive(HDirect3DDevice9 *This, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount) {return This->original->lpVtbl->DrawPrimitive(This->original, PrimitiveType, StartVertex, PrimitiveCount);}
HRESULT STDMETHODCALLTYPE DEVDrawIndexedPrimitive(HDirect3DDevice9 *This, D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount) {return This->original->lpVtbl->DrawIndexedPrimitive(This->original, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);}
HRESULT STDMETHODCALLTYPE DEVDrawPrimitiveUP(HDirect3DDevice9 *This, D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) {return This->original->lpVtbl->DrawPrimitiveUP(This->original, PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);}
HRESULT STDMETHODCALLTYPE DEVDrawIndexedPrimitiveUP(HDirect3DDevice9 *This, D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) {return This->original->lpVtbl->DrawIndexedPrimitiveUP(This->original, PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);}
HRESULT STDMETHODCALLTYPE DEVProcessVertices(HDirect3DDevice9 *This, UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags) {return This->original->lpVtbl->ProcessVertices(This->original, SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);}
HRESULT STDMETHODCALLTYPE DEVCreateVertexDeclaration(HDirect3DDevice9 *This, CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl) {return This->original->lpVtbl->CreateVertexDeclaration(This->original, pVertexElements, ppDecl);}
HRESULT STDMETHODCALLTYPE DEVSetVertexDeclaration(HDirect3DDevice9 *This, IDirect3DVertexDeclaration9* pDecl) {return This->original->lpVtbl->SetVertexDeclaration(This->original, pDecl);}
HRESULT STDMETHODCALLTYPE DEVGetVertexDeclaration(HDirect3DDevice9 *This, IDirect3DVertexDeclaration9** ppDecl) {return This->original->lpVtbl->GetVertexDeclaration(This->original, ppDecl);}
HRESULT STDMETHODCALLTYPE DEVSetFVF(HDirect3DDevice9 *This, DWORD FVF) {return This->original->lpVtbl->SetFVF(This->original, FVF);}
HRESULT STDMETHODCALLTYPE DEVGetFVF(HDirect3DDevice9 *This, DWORD* pFVF) {return This->original->lpVtbl->GetFVF(This->original, pFVF);}
HRESULT STDMETHODCALLTYPE DEVCreateVertexShader(HDirect3DDevice9 *This, CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader) {return This->original->lpVtbl->CreateVertexShader(This->original, pFunction, ppShader);}
HRESULT STDMETHODCALLTYPE DEVSetVertexShader(HDirect3DDevice9 *This, IDirect3DVertexShader9* pShader) {return This->original->lpVtbl->SetVertexShader(This->original, pShader);}
HRESULT STDMETHODCALLTYPE DEVGetVertexShader(HDirect3DDevice9 *This, IDirect3DVertexShader9** ppShader) {return This->original->lpVtbl->GetVertexShader(This->original, ppShader);}
HRESULT STDMETHODCALLTYPE DEVSetVertexShaderConstantF(HDirect3DDevice9 *This, UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) {return This->original->lpVtbl->SetVertexShaderConstantF(This->original, StartRegister, pConstantData, Vector4fCount);}
HRESULT STDMETHODCALLTYPE DEVGetVertexShaderConstantF(HDirect3DDevice9 *This, UINT StartRegister,float* pConstantData,UINT Vector4fCount) {return This->original->lpVtbl->GetVertexShaderConstantF(This->original, StartRegister, pConstantData, Vector4fCount);}
HRESULT STDMETHODCALLTYPE DEVSetVertexShaderConstantI(HDirect3DDevice9 *This, UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) {return This->original->lpVtbl->SetVertexShaderConstantI(This->original, StartRegister, pConstantData, Vector4iCount);}
HRESULT STDMETHODCALLTYPE DEVGetVertexShaderConstantI(HDirect3DDevice9 *This, UINT StartRegister,int* pConstantData,UINT Vector4iCount) {return This->original->lpVtbl->GetVertexShaderConstantI(This->original, StartRegister, pConstantData, Vector4iCount);}
HRESULT STDMETHODCALLTYPE DEVSetVertexShaderConstantB(HDirect3DDevice9 *This, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) {return This->original->lpVtbl->SetVertexShaderConstantB(This->original, StartRegister, pConstantData, BoolCount);}
HRESULT STDMETHODCALLTYPE DEVGetVertexShaderConstantB(HDirect3DDevice9 *This, UINT StartRegister,BOOL* pConstantData,UINT BoolCount) {return This->original->lpVtbl->GetVertexShaderConstantB(This->original, StartRegister, pConstantData, BoolCount);}
HRESULT STDMETHODCALLTYPE DEVSetStreamSource(HDirect3DDevice9 *This, UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride) {return This->original->lpVtbl->SetStreamSource(This->original, StreamNumber, pStreamData, OffsetInBytes, Stride);}
HRESULT STDMETHODCALLTYPE DEVGetStreamSource(HDirect3DDevice9 *This, UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride) {return This->original->lpVtbl->GetStreamSource(This->original, StreamNumber, ppStreamData, pOffsetInBytes, pStride);}
HRESULT STDMETHODCALLTYPE DEVSetStreamSourceFreq(HDirect3DDevice9 *This, UINT StreamNumber,UINT Setting) {return This->original->lpVtbl->SetStreamSourceFreq(This->original, StreamNumber, Setting);}
HRESULT STDMETHODCALLTYPE DEVGetStreamSourceFreq(HDirect3DDevice9 *This, UINT StreamNumber,UINT* pSetting) {return This->original->lpVtbl->GetStreamSourceFreq(This->original, StreamNumber, pSetting);}
HRESULT STDMETHODCALLTYPE DEVSetIndices(HDirect3DDevice9 *This, IDirect3DIndexBuffer9* pIndexData) {return This->original->lpVtbl->SetIndices(This->original, pIndexData);}
HRESULT STDMETHODCALLTYPE DEVGetIndices(HDirect3DDevice9 *This, IDirect3DIndexBuffer9** ppIndexData) {return This->original->lpVtbl->GetIndices(This->original, ppIndexData);}
HRESULT STDMETHODCALLTYPE DEVCreatePixelShader(HDirect3DDevice9 *This, CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader) {return This->original->lpVtbl->CreatePixelShader(This->original, pFunction, ppShader);}
HRESULT STDMETHODCALLTYPE DEVSetPixelShader(HDirect3DDevice9 *This, IDirect3DPixelShader9* pShader) {return This->original->lpVtbl->SetPixelShader(This->original, pShader);}
HRESULT STDMETHODCALLTYPE DEVGetPixelShader(HDirect3DDevice9 *This, IDirect3DPixelShader9** ppShader) {return This->original->lpVtbl->GetPixelShader(This->original, ppShader);}
HRESULT STDMETHODCALLTYPE DEVSetPixelShaderConstantF(HDirect3DDevice9 *This, UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) {return This->original->lpVtbl->SetPixelShaderConstantF(This->original, StartRegister, pConstantData, Vector4fCount);}
HRESULT STDMETHODCALLTYPE DEVGetPixelShaderConstantF(HDirect3DDevice9 *This, UINT StartRegister,float* pConstantData,UINT Vector4fCount) {return This->original->lpVtbl->GetPixelShaderConstantF(This->original, StartRegister, pConstantData, Vector4fCount);}
HRESULT STDMETHODCALLTYPE DEVSetPixelShaderConstantI(HDirect3DDevice9 *This, UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) {return This->original->lpVtbl->SetPixelShaderConstantI(This->original, StartRegister, pConstantData, Vector4iCount);}
HRESULT STDMETHODCALLTYPE DEVGetPixelShaderConstantI(HDirect3DDevice9 *This, UINT StartRegister,int* pConstantData,UINT Vector4iCount) {return This->original->lpVtbl->GetPixelShaderConstantI(This->original, StartRegister, pConstantData, Vector4iCount);}
HRESULT STDMETHODCALLTYPE DEVSetPixelShaderConstantB(HDirect3DDevice9 *This, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) {return This->original->lpVtbl->SetPixelShaderConstantB(This->original, StartRegister, pConstantData, BoolCount);}
HRESULT STDMETHODCALLTYPE DEVGetPixelShaderConstantB(HDirect3DDevice9 *This, UINT StartRegister,BOOL* pConstantData,UINT BoolCount) {return This->original->lpVtbl->GetPixelShaderConstantB(This->original, StartRegister, pConstantData, BoolCount);}
HRESULT STDMETHODCALLTYPE DEVDrawRectPatch(HDirect3DDevice9 *This, UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo) {return This->original->lpVtbl->DrawRectPatch(This->original, Handle, pNumSegs, pRectPatchInfo);}
HRESULT STDMETHODCALLTYPE DEVDrawTriPatch(HDirect3DDevice9 *This, UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo) {return This->original->lpVtbl->DrawTriPatch(This->original, Handle, pNumSegs, pTriPatchInfo);}
HRESULT STDMETHODCALLTYPE DEVDeletePatch(HDirect3DDevice9 *This, UINT Handle) {return This->original->lpVtbl->DeletePatch(This->original, Handle);}
HRESULT STDMETHODCALLTYPE DEVCreateQuery(HDirect3DDevice9 *This, D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery) {return This->original->lpVtbl->CreateQuery(This->original, Type, ppQuery);}

HRESULT STDMETHODCALLTYPE DEVGetDirect3D(HDirect3DDevice9 *This, IDirect3D9** ppD3D9)
{
	This->d3d->original->lpVtbl->AddRef(This->d3d->original);
	*ppD3D9 = (IDirect3D9*)This->d3d;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE DEVReset(HDirect3DDevice9 *This, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	IGDX9Reset();
	return This->original->lpVtbl->Reset(This->original, pPresentationParameters);
}

HRESULT STDMETHODCALLTYPE DEVBeginScene(HDirect3DDevice9 *This)
{
	IGNewFrame();
	return This->original->lpVtbl->BeginScene(This->original);
}

HRESULT STDMETHODCALLTYPE DEVEndScene(HDirect3DDevice9 *This)
{
	IDirect3DDevice9 *dd = This->original;
	D3DMATRIX mtx;
	for(int x = 0; x < 4; x++) for(int y = 0; y < 4; y++) mtx.m[x][y] = 0;
	mtx._11 = mtx._22 = mtx._33 = mtx._44 = 1;
	dd->lpVtbl->SetTransform(dd, D3DTS_TEXTURE0, &mtx);
	dd->lpVtbl->SetTransform(dd, D3DTS_TEXTURE1, &mtx);
	IGDX9Render();
	return This->original->lpVtbl->EndScene(This->original);
}

void *odmvtbld[] = {DDQueryInterface, DDAddRef, DDRelease, DDRegisterSoftwareDevice,
	DDGetAdapterCount, DDGetAdapterIdentifier, DDGetAdapterModeCount,
	DDEnumAdapterModes, DDGetAdapterDisplayMode, DDCheckDeviceType,
	DDCheckDeviceFormat, DDCheckDeviceMultiSampleType, DDCheckDepthStencilMatch,
	DDCheckDeviceFormatConversion, DDGetDeviceCaps, DDGetAdapterMonitor,
	DDCreateDevice};
void *devvtbld[] = {DEVQueryInterface, DEVAddRef, DEVRelease, DEVTestCooperativeLevel,
	DEVGetAvailableTextureMem, DEVEvictManagedResources, DEVGetDirect3D,
	DEVGetDeviceCaps, DEVGetDisplayMode, DEVGetCreationParameters,
	DEVSetCursorProperties, DEVSetCursorPosition, DEVShowCursor,
	DEVCreateAdditionalSwapChain, DEVGetSwapChain, DEVGetNumberOfSwapChains,
	DEVReset, DEVPresent, DEVGetBackBuffer, DEVGetRasterStatus,
	DEVSetDialogBoxMode, DEVSetGammaRamp, DEVGetGammaRamp, DEVCreateTexture,
	DEVCreateVolumeTexture, DEVCreateCubeTexture, DEVCreateVertexBuffer,
	DEVCreateIndexBuffer, DEVCreateRenderTarget, DEVCreateDepthStencilSurface,
	DEVUpdateSurface, DEVUpdateTexture, DEVGetRenderTargetData,
	DEVGetFrontBufferData, DEVStretchRect, DEVColorFill,
	DEVCreateOffscreenPlainSurface, DEVSetRenderTarget, DEVGetRenderTarget,
	DEVSetDepthStencilSurface, DEVGetDepthStencilSurface, DEVBeginScene,
	DEVEndScene, DEVClear, DEVSetTransform, DEVGetTransform,
	DEVMultiplyTransform, DEVSetViewport, DEVGetViewport, DEVSetMaterial,
	DEVGetMaterial, DEVSetLight, DEVGetLight, DEVLightEnable,
	DEVGetLightEnable, DEVSetClipPlane, DEVGetClipPlane, DEVSetRenderState,
	DEVGetRenderState, DEVCreateStateBlock, DEVBeginStateBlock,
	DEVEndStateBlock, DEVSetClipStatus, DEVGetClipStatus, DEVGetTexture,
	DEVSetTexture, DEVGetTextureStageState, DEVSetTextureStageState,
	DEVGetSamplerState, DEVSetSamplerState, DEVValidateDevice,
	DEVSetPaletteEntries, DEVGetPaletteEntries, DEVSetCurrentTexturePalette,
	DEVGetCurrentTexturePalette, DEVSetScissorRect, DEVGetScissorRect,
	DEVSetSoftwareVertexProcessing, DEVGetSoftwareVertexProcessing,
	DEVSetNPatchMode, DEVGetNPatchMode, DEVDrawPrimitive,
	DEVDrawIndexedPrimitive, DEVDrawPrimitiveUP, DEVDrawIndexedPrimitiveUP,
	DEVProcessVertices, DEVCreateVertexDeclaration, DEVSetVertexDeclaration,
	DEVGetVertexDeclaration, DEVSetFVF, DEVGetFVF, DEVCreateVertexShader,
	DEVSetVertexShader, DEVGetVertexShader, DEVSetVertexShaderConstantF,
	DEVGetVertexShaderConstantF, DEVSetVertexShaderConstantI,
	DEVGetVertexShaderConstantI, DEVSetVertexShaderConstantB,
	DEVGetVertexShaderConstantB, DEVSetStreamSource, DEVGetStreamSource,
	DEVSetStreamSourceFreq, DEVGetStreamSourceFreq, DEVSetIndices,
	DEVGetIndices, DEVCreatePixelShader, DEVSetPixelShader,
	DEVGetPixelShader, DEVSetPixelShaderConstantF,
	DEVGetPixelShaderConstantF, DEVSetPixelShaderConstantI,
	DEVGetPixelShaderConstantI, DEVSetPixelShaderConstantB,
	DEVGetPixelShaderConstantB, DEVDrawRectPatch, DEVDrawTriPatch,
	DEVDeletePatch, DEVCreateQuery};

#endif