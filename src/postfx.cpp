#include "skygfx.h"
#include "ModuleList.hpp"
#include <stdarg.h>

RwIm2DVertex *colorfilterVerts = (RwIm2DVertex*)0xC400D8;
RwImVertexIndex *colorfilterIndices = (RwImVertexIndex*)0x8D5174;

Imf &CPostEffects::ms_imf = *(Imf*)0xC40150;

WRAPPER void CPostEffects::DarknessFilter(uint8 alpha) { EAXJMP(0x702F00); }
WRAPPER void CPostEffects::Grain(int strengh, bool generate) { EAXJMP(0x7037C0); }
WRAPPER void CPostEffects::SpeedFX(float) { EAXJMP(0x7030A0); }
RwRaster *&CPostEffects::pRasterFrontBuffer = *(RwRaster**)0xC402D8;
float &CPostEffects::m_fInfraredVisionFilterRadius = *(float*)0x8D50B8;
RwRaster *&CPostEffects::m_pGrainRaster = *(RwRaster**)0xC402B0;
WRAPPER void CPostEffects::InfraredVision(RwRGBA color1, RwRGBA color2) { EAXJMP(0x703F80); }
WRAPPER void CPostEffects::ImmediateModeRenderStatesStore(void) { EAXJMP(0x700CC0); }
WRAPPER void CPostEffects::ImmediateModeRenderStatesSet(void) { EAXJMP(0x700D70); }
WRAPPER void CPostEffects::ImmediateModeRenderStatesReStore(void) { EAXJMP(0x700E00); }
WRAPPER void CPostEffects::SetFilterMainColour(RwRaster *raster, RwRGBA color) { EAXJMP(0x703520); }
WRAPPER void CPostEffects::DrawQuad(float x1, float y1, float x2, float y2, uchar r, uchar g, uchar b, uchar alpha, RwRaster *ras) { EAXJMP(0x700EC0); }
WRAPPER void CPostEffects::NightVision(RwRGBA color) { EAXJMP(0x7011C0); }
WRAPPER void CPostEffects::ColourFilter(RwRGBA rgb1, RwRGBA rgb2) { EAXJMP(0x703650); }
float &CPostEffects::m_fNightVisionSwitchOnFXCount = *(float*)0xC40300;
int &CPostEffects::m_InfraredVisionGrainStrength = *(int*)0x8D50B4;
int &CPostEffects::m_NightVisionGrainStrength = *(int*)0x8D50A8;
bool &CPostEffects::m_bInfraredVision = *(bool*)0xC402B9;

bool &CPostEffects::m_bDisableAllPostEffect = *(bool*)0xC402CF;

bool &CPostEffects::m_bColorEnable = *(bool*)0x8D518C;
int &CPostEffects::m_colourLeftUOffset = *(int*)0x8D5150;
int &CPostEffects::m_colourRightUOffset = *(int*)0x8D5154;
int &CPostEffects::m_colourTopVOffset = *(int*)0x8D5158;
int &CPostEffects::m_colourBottomVOffset = *(int*)0x8D515C;
float &CPostEffects::m_colour1Multiplier = *(float*)0x8D5160;
float &CPostEffects::m_colour2Multiplier = *(float*)0x8D5164;
float &CPostEffects::SCREEN_EXTRA_MULT_CHANGE_RATE = *(float*)0x8D5168;
float &CPostEffects::SCREEN_EXTRA_MULT_BASE_CAP = *(float*)0x8D516C;
float &CPostEffects::SCREEN_EXTRA_MULT_BASE_MULT = *(float*)0x8D5170;

bool &CPostEffects::m_bRadiosity = *(bool*)0xC402CC;
bool &CPostEffects::m_bRadiosityDebug = *(bool*)0xC402CD;
int &CPostEffects::m_RadiosityFilterPasses = *(int*)0x8D510C;
int &CPostEffects::m_RadiosityRenderPasses = *(int*)0x8D5110;
int &CPostEffects::m_RadiosityIntensityLimit = *(int*)0x8D5114;
int &CPostEffects::m_RadiosityIntensity = *(int*)0x8D5118;
bool &CPostEffects::m_bRadiosityBypassTimeCycleIntensityLimit = *(bool*)0xC402CE;
int &CPostEffects::m_RadiosityFilterUCorrection = *(int*)0x8D511C;
int &CPostEffects::m_RadiosityFilterVCorrection = *(int*)0x8D5120;

bool &CPostEffects::m_bDarknessFilter = *(bool*)0xC402C4;
int &CPostEffects::m_DarknessFilterAlpha = *(int*)0x8D5204;
int &CPostEffects::m_DarknessFilterAlphaDefault = *(int*)0x8D50F4;
int &CPostEffects::m_DarknessFilterRadiosityIntensityLimit = *(int*)0x8D50F8;

bool &CPostEffects::m_bCCTV = *(bool*)0xC402C5;
bool &CPostEffects::m_bFog = *(bool*)0xC402C6;
bool &CPostEffects::m_bNightVision = *(bool*)0xC402B8;
bool &CPostEffects::m_bHeatHazeFX = *(bool*)0xC402BA;
bool &CPostEffects::m_bHeatHazeMaskModeTest = *(bool*)0xC402BB;
bool &CPostEffects::m_bGrainEnable = *(bool*)0xC402B4;
bool &CPostEffects::m_waterEnable = *(bool*)0xC402D3;

bool &CPostEffects::m_bSpeedFX = *(bool*)0x8D5100;
bool &CPostEffects::m_bSpeedFXTestMode = *(bool*)0xC402C7;
uint8 &CPostEffects::m_SpeedFXAlpha = *(uint8*)0x8D5104;

/* My own */
bool CPostEffects::m_bBlurColourFilter = true;
bool CPostEffects::m_bYCbCrFilter = false;
float CPostEffects::m_lumaScale = 219.0f/255.0f;
float CPostEffects::m_lumaOffset = 16.0f/255.0f;
float CPostEffects::m_cbScale = 1.23f;
float CPostEffects::m_cbOffset = 0.0f;
float CPostEffects::m_crScale = 1.23f;
float CPostEffects::m_crOffset = 0.0f;



/////
///// Im2D overrides
/////


int overrideColorMod = -1;
int overrideAlphaMod = -1;
void *overrideIm2dPixelShader;

void Im2DColorModulationHook(RwUInt32 stage, RwUInt32 type, RwUInt32 value)
{
	if(overrideColorMod >= 0)
		RwD3D9SetTextureStageState(stage, type, overrideColorMod);
	else
		RwD3D9SetTextureStageState(stage, type, value);
}
void Im2DAlphaModulationHook(RwUInt32 stage, RwUInt32 type, RwUInt32 value)
{
	if(overrideAlphaMod >= 0)
		RwD3D9SetTextureStageState(stage, type, overrideAlphaMod);
	else
		RwD3D9SetTextureStageState(stage, type, value);
}

void
Im2dSetPixelShader_hook(void*)
{
	RwD3D9SetPixelShader(overrideIm2dPixelShader);
}


/////
/////
/////

// Credits: much of the code in this file was originally written by NTAuthority
// there's not a lot of that left now

void *iiiTrailsPS, *vcTrailsPS;
RwRaster *grainRaster;


// Mobile stuff
struct Grade
{
	float r, g, b, a;
};
void *gradingPS, *contrastPS;
#define NUMHOURS 8
#define NUMWEATHERS 23
#define EXTRASTART 21

struct GradeColorset
{
	Grade red;
	Grade green;
	Grade blue;

	GradeColorset(void) {}
	GradeColorset(int h, int w);
	void Interpolate(GradeColorset *a, GradeColorset *b, float fa, float fb);
};


struct Colorcycle
{
	static bool initialised;
	static Grade redGrade[24][NUMWEATHERS];
	static Grade greenGrade[24][NUMWEATHERS];
	static Grade blueGrade[24][NUMWEATHERS];

	static void Initialise(void);
	static void Update(GradeColorset *colorset);
};




void
CPostEffects::UpdateFrontBuffer(void)
{
	RwCameraEndUpdate(Scene.camera);
	RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
	RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
	RwRasterPopContext();
	RwCameraBeginUpdate(Scene.camera);
}

RwRaster *vcs_radiosity_target1, *vcs_radiosity_target2;
static RwIm2DVertex vcsVertices[24];
RwRect vcsRect;
RwImVertexIndex vcsIndices1[] = {
	0, 1, 2, 1, 2, 3,
		4, 5, 2, 5, 2, 3,
	4, 5, 6, 5, 6, 7,
		8, 9, 6, 9, 6, 7,
	8, 9, 10, 9, 10, 11,
		12, 13, 10, 13, 10, 11,
	12, 13, 14, 13, 14, 15,
};

RwImVertexIndex radiosityIndices[] = {
	0, 1, 2, 1, 2, 3
};

RwD3D9Vertex radiosity_vcs_vertices[44];

//#define LIMIT (config->trailsLimit)
//#define INTENSITY (config->trailsIntensity)

void
makequad(RwD3D9Vertex *v, int width, int height, int texwidth = 0, int texheight = 0)
{
	float w, h, tw, th;
	w = width;
	h = height;
	tw = texwidth > 0 ? texwidth : w;
	th = texheight > 0 ? texheight : h;
	v[0].x = 0;
	v[0].y = 0;
	v[0].z = 0.0f;
	v[0].rhw = 1.0f;
	v[0].u = 0.5f / tw;
	v[0].v = 0.5f / th;
	v[0].emissiveColor = 0xFFFFFFFF;
	v[1].x = 0;
	v[1].y = h;
	v[1].z = 0.0f;
	v[1].rhw = 1.0f;
	v[1].u = 0.5f / tw;
	v[1].v = (h + 0.5f) / th;
	v[1].emissiveColor = 0xFFFFFFFF;
	v[2].x = w;
	v[2].y = 0;
	v[2].z = 0.0f;
	v[2].rhw = 1.0f;
	v[2].u = (w + 0.5f) / tw;
	v[2].v = 0.5f / th;
	v[2].emissiveColor = 0xFFFFFFFF;
	v[3].x = w;
	v[3].y = h;
	v[3].z = 0.0f;
	v[3].rhw = 1.0f;
	v[3].u = (w + 0.5f) / tw;
	v[3].v = (h + 0.5f) / th;
	v[3].emissiveColor = 0xFFFFFFFF;
}

void
CPostEffects::Radiosity_VCS_init(void)
{
	static float uOffsets[] = { -1.0f, 1.0f, 0.0f, 0.0f,   -1.0f, 1.0f, -1.0f, 1.0f };
	static float vOffsets[] = { 0.0f, 0.0f, -1.0f, 1.0f,   -1.0f, -1.0f, 1.0f, 1.0f };
	int i;
	int resMult = config->trailsResolution;
	RwUInt32 c;
	float w, h;

	if(vcs_radiosity_target1)
		RwRasterDestroy(vcs_radiosity_target1);
	vcs_radiosity_target1 = RwRasterCreate(256 * resMult, 128 * resMult, RwCameraGetRaster(Scene.camera)->depth, rwRASTERTYPECAMERATEXTURE);
	if(vcs_radiosity_target2)
		RwRasterDestroy(vcs_radiosity_target2);
	vcs_radiosity_target2 = RwRasterCreate(256 * resMult, 128 * resMult, RwCameraGetRaster(Scene.camera)->depth, rwRASTERTYPECAMERATEXTURE);
//	RwD3D9CreateVertexBuffer(stride, size, &vbuf, &offset);

	w = 256 * resMult;
	h = 128 * resMult;

	// TODO: tex coords correct?
	makequad(radiosity_vcs_vertices, 256 * resMult, 128 * resMult);
	makequad(radiosity_vcs_vertices+4, RwCameraGetRaster(Scene.camera)->width, RwCameraGetRaster(Scene.camera)->height);

	// black vertices; at 8
	for(i = 0; i < 4; i++){
		radiosity_vcs_vertices[i+8] = radiosity_vcs_vertices[i];
		radiosity_vcs_vertices[i+8].emissiveColor = 0;
	}

	// two sets blur vertices; at 12
	c = D3DCOLOR_ARGB(0xFF, 36, 36, 36);
	for(i = 0; i < 2*4*4; i++){
		radiosity_vcs_vertices[i+12] = radiosity_vcs_vertices[i%4];
		radiosity_vcs_vertices[i+12].emissiveColor = c;
		switch(i%4){
		case 0:
			radiosity_vcs_vertices[i+12].u = (uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (vOffsets[i/4] + 0.5f) / h;
			break;
		case 1:
			radiosity_vcs_vertices[i+12].u = (uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (h + vOffsets[i/4] + 0.5f) / h;
			break;
		case 2:
			radiosity_vcs_vertices[i+12].u = (w + uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (vOffsets[i/4] + 0.5f) / h;
			break;
		case 3:
			radiosity_vcs_vertices[i+12].u = (w + uOffsets[i/4] + 0.5f) / w;
			radiosity_vcs_vertices[i+12].v = (h + vOffsets[i/4] + 0.5f) / h;
			break;
		}
	}
}

void
CPostEffects::Radiosity_VCS(int limit, int intensity)
{
	static int lastWidth, lastHeight, lastConfigRes;
	int i;
	int resMult = config->trailsResolution;
	RwRaster *fb;
	RwRaster *fb1, *fb2, *tmp;

	fb = RwCameraGetRaster(Scene.camera);
	if(lastWidth != fb->width || lastHeight != fb->height || lastConfigRes != resMult){
		Radiosity_VCS_init();
		lastWidth = fb->width;
		lastHeight = fb->height;
		lastConfigRes = resMult;
	}

	RwRect r;
	r.x = 0;
	r.y = 0;
	r.w = 256 * resMult;
	r.h = 128 * resMult;

	CPostEffects::ImmediateModeRenderStatesStore();
	CPostEffects::ImmediateModeRenderStatesSet();
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwD3D9SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	RwCameraEndUpdate(Scene.camera);

	RwRasterPushContext(vcs_radiosity_target2);
	RwRasterRenderScaled(fb, &r);
	RwRasterPopContext();

	RwCameraSetRaster(Scene.camera, vcs_radiosity_target2);
	RwCameraBeginUpdate(Scene.camera);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, limit/2, limit/2, limit/2));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices, 4, radiosityIndices, 6);

	fb1 = vcs_radiosity_target1;
	fb2 = vcs_radiosity_target2;
	for(i = 0; i < 4; i++){
		RwD3D9SetRenderTarget(0, fb1);

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+8, 4, radiosityIndices, 6);

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, fb2);
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSCLAMP);
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSCLAMP);
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		if((i % 2) == 0)
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+12, 4*4, vcsIndices1, 6*7);
		else
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+28, 4*4, vcsIndices1, 6*7);

		tmp = fb1;
		fb1 = fb2;
		fb2 = tmp;
	}

	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, fb);
	RwCameraBeginUpdate(Scene.camera);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, fb2);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSCLAMP);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSCLAMP);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, intensity*4, intensity*4, intensity*4));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+4, 4, radiosityIndices, 6);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, radiosity_vcs_vertices+4, 4, radiosityIndices, 6);

	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	CPostEffects::ImmediateModeRenderStatesReStore();
}

RwD3D9Vertex blur_vcs_vertices[24];
RwImVertexIndex blur_vcs_Indices[] = {
	0, 1, 2, 2, 1, 3,
	4, 5, 6, 6, 5, 7,
	8, 9, 10, 10, 9, 11,
};
RwRaster *lastFrameBuffer;
RwRGBA vcsblurrgb;
RwRGBA rgbTweak;

#define BLUROFFSET (2.1f)
#define BLURINTENSITY (39.0f)

void
CPostEffects::Blur_VCS(void)
{
	static int lastWidth, lastHeight;
	static int justInitialized;
	int i;
	int bufw, bufh;
	int screenw, screenh;
	int intensity;
	bufw = CPostEffects::pRasterFrontBuffer->width;
	bufh = CPostEffects::pRasterFrontBuffer->height;

	/*if(GetAsyncKeyState(VK_F7) & 0x8000){
		justInitialized = 1;
		return;
	}*/

	if(lastWidth != bufw || lastHeight != bufh){
		if(lastFrameBuffer)
			RwRasterDestroy(lastFrameBuffer);
		lastFrameBuffer = RwRasterCreate(bufw, bufh, CPostEffects::pRasterFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);
		justInitialized = 1;
		lastWidth = bufw;
		lastHeight = bufh;
	}

	screenw = RwCameraGetRaster(Scene.camera)->width;
	screenh = RwCameraGetRaster(Scene.camera)->height;

	makequad(blur_vcs_vertices, screenw, screenh, bufw, bufh);
	for(i = 0; i < 4; i++)
		blur_vcs_vertices[i].x += BLUROFFSET;
	makequad(blur_vcs_vertices+4, screenw, screenh, bufw, bufh);
	for(i = 4; i < 8; i++){
		blur_vcs_vertices[i].x += BLUROFFSET;
		blur_vcs_vertices[i].y += BLUROFFSET;
	}
	makequad(blur_vcs_vertices+8, screenw, screenh, bufw, bufh);
	for(i = 8; i < 12; i++)
		blur_vcs_vertices[i].y += BLUROFFSET;
	makequad(blur_vcs_vertices+12, screenw, screenh, bufw, bufh);
	for(i = 12; i < 16; i++)
		blur_vcs_vertices[i].emissiveColor = D3DCOLOR_ARGB(0xff, vcsblurrgb.red, vcsblurrgb.green, vcsblurrgb.blue);
	makequad(blur_vcs_vertices+16, screenw, screenh, bufw, bufh);
	makequad(blur_vcs_vertices+20, screenw, screenh, bufw, bufh);
	for(i = 20; i < 24; i++)
		blur_vcs_vertices[i].emissiveColor = 0;

	CPostEffects::ImmediateModeRenderStatesStore();
	CPostEffects::ImmediateModeRenderStatesSet();
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	// get current frame
	RwCameraEndUpdate(Scene.camera);
	RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
	RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
	RwRasterPopContext();
	RwCameraBeginUpdate(Scene.camera);

	// blur frame
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, CPostEffects::pRasterFrontBuffer);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
	intensity = BLURINTENSITY*0.8f;
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, intensity, intensity, intensity));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices, 12, blur_vcs_Indices, 3*6);

	// add colour filter color
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices+12, 4, blur_vcs_Indices, 6);

	// blend with last frame
	if(justInitialized)
		justInitialized = 0;
	else{
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, lastFrameBuffer);
		RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
		RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
		RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, 32, 32, 32));
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices+16, 4, blur_vcs_Indices, 6);
	}

	// blend with black. Is this real?
if(0){
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
	RwD3D9SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0xFF, 32, 32, 32));
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, blur_vcs_vertices+20, 4, blur_vcs_Indices, 6);
}

	RwCameraEndUpdate(Scene.camera);
	RwRasterPushContext(lastFrameBuffer);
	RwRasterRenderFast(RwCameraGetRaster(Scene.camera), 0, 0);
	RwRasterPopContext();
	RwCameraBeginUpdate(Scene.camera);

	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	CPostEffects::ImmediateModeRenderStatesReStore();
}

/* quad format:
 * 0--3
 * |\ |
 * | \|
 * 1--2 */
void
quadSetXY(RwIm2DVertex *verts, float x0, float y0, float x1, float y1)
{
	RwIm2DVertexSetScreenX(&verts[0], x0);
	RwIm2DVertexSetScreenY(&verts[0], y0);
	RwIm2DVertexSetScreenX(&verts[1], x0);
	RwIm2DVertexSetScreenY(&verts[1], y1);
	RwIm2DVertexSetScreenX(&verts[2], x1);
	RwIm2DVertexSetScreenY(&verts[2], y1);
	RwIm2DVertexSetScreenX(&verts[3], x1);
	RwIm2DVertexSetScreenY(&verts[3], y0);
}

void
quadSetUV(RwIm2DVertex *verts, float u0, float v0, float u1, float v1)
{
	RwIm2DVertexSetU(&verts[0], u0, 1.0f);
	RwIm2DVertexSetV(&verts[0], v0, 1.0f);
	RwIm2DVertexSetU(&verts[1], u0, 1.0f);
	RwIm2DVertexSetV(&verts[1], v1, 1.0f);
	RwIm2DVertexSetU(&verts[2], u1, 1.0f);
	RwIm2DVertexSetV(&verts[2], v1, 1.0f);
	RwIm2DVertexSetU(&verts[3], u1, 1.0f);
	RwIm2DVertexSetV(&verts[3], v0, 1.0f);
}

void
CPostEffects::DrawQuadSetUVs(float utl, float vtl, float utr, float vtr, float ubr, float vbr, float ubl, float vbl)
{
	RwIm2DVertexSetU(&ms_imf.quad_verts[0], utl, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[0], vtl, ms_imf.recipZ);
	RwIm2DVertexSetU(&ms_imf.quad_verts[1], utr, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[1], vtr, ms_imf.recipZ);
	RwIm2DVertexSetU(&ms_imf.quad_verts[2], ubl, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[2], vbl, ms_imf.recipZ);
	RwIm2DVertexSetU(&ms_imf.quad_verts[3], ubr, ms_imf.recipZ);
	RwIm2DVertexSetV(&ms_imf.quad_verts[3], vbr, ms_imf.recipZ);
}

void
CPostEffects::DrawQuadSetDefaultUVs(void)
{
	DrawQuadSetUVs(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
}

void *blurPS, *radiosityPS;

void
CPostEffects::Radiosity_shader(int intensityLimit, int filterPasses, int renderPasses, int intensity)
{
	static RwRaster *workBuffer;
	if(workBuffer)
		if(workBuffer->width != pRasterFrontBuffer->width ||
		   workBuffer->height != pRasterFrontBuffer->height ||
		   workBuffer->depth != pRasterFrontBuffer->depth){
			RwRasterDestroy(workBuffer);
			workBuffer = nil;
		}
	if(workBuffer == nil)
		workBuffer = RwRasterCreate(pRasterFrontBuffer->width, pRasterFrontBuffer->height, pRasterFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);

	RwRaster *drawBuffer = RwCameraGetRaster(Scene.camera);




	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, workBuffer);
	RwCameraBeginUpdate(Scene.camera);

	float params[4];
	params[2] = 1<<filterPasses;
	params[2] *= drawBuffer->width/640.0f;

	overrideIm2dPixelShader = blurPS;
	// Blur vertically
	params[0] = 0;
	params[1] = 1.0f/RwRasterGetHeight(pRasterFrontBuffer);
	RwD3D9SetPixelShaderConstant(0, params, 1);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	UpdateFrontBuffer();
	// Blur horizontally
	params[0] = 1.0f/RwRasterGetWidth(pRasterFrontBuffer);
	params[1] = 0;
	RwD3D9SetPixelShaderConstant(0, params, 1);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	UpdateFrontBuffer();
	overrideIm2dPixelShader = nil;


	/* Restore original FB */
	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, drawBuffer);
	RwCameraBeginUpdate(Scene.camera);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);

	/* Add to framebuffer */
	params[0] = intensityLimit/255.0f;
	params[1] = intensity/255.0f;
	params[2] = renderPasses;
	RwD3D9SetPixelShaderConstant(0, params, 1);

	float off = ((1<<filterPasses)-1);
	// only for upper left corner actually
	// other one has 2,2 harcoded but since these are 2 by default, we'll reuse them
	float offu = off*m_RadiosityFilterUCorrection;
	float offv = off*m_RadiosityFilterVCorrection;

	float minu = offu;
	float minv = offv;
	float maxu = drawBuffer->width - offu; //off*2;
	float maxv = drawBuffer->height - offv; //off*2;
	float cu = (offu*(drawBuffer->width+0.5f) + offu/*off*2*/*0.5f) / drawBuffer->width;
	float cv = (offv*(drawBuffer->height+0.5f) + offv/*off*2*/*0.5f) / drawBuffer->height;

	params[0] = cu / pRasterFrontBuffer->width;
	params[1] = cv / pRasterFrontBuffer->height;
	params[2] = (maxu-minu) / drawBuffer->width;
	params[3] = (maxv-minv) / drawBuffer->height;
	RwD3D9SetPixelShaderConstant(1, params, 1);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)!m_bRadiosityDebug);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	overrideIm2dPixelShader = radiosityPS;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	UpdateFrontBuffer();
}

void
CPostEffects::Radiosity(int intensityLimit, int filterPasses, int renderPasses, int intensity)
{
/*
	{
		static bool keystate = false;
		if(GetAsyncKeyState(VK_F5) & 0x8000){
			if(!keystate){
				keystate = true;
				config->radiosity = !config->radiosity;
			}
		}else
			keystate = false;
	}
*/

	if (config->vcsTrails) {
		CPostEffects::Radiosity_VCS(config->trailsLimit, config->trailsIntensity);
		if (config->colorFilter == COLORFILTER_VCS)
			CPostEffects::Blur_VCS();
		return;
	}

	if(!config->doRadiosity)
		return;

	if(config->radiosity == 1){
		Radiosity_shader(intensityLimit, filterPasses, renderPasses, intensity);
		return;
	}

	static RwRaster *workBuffer;
	if(workBuffer)
		if(workBuffer->width != pRasterFrontBuffer->width ||
		   workBuffer->height != pRasterFrontBuffer->height ||
		   workBuffer->depth != pRasterFrontBuffer->depth){
			RwRasterDestroy(workBuffer);
			workBuffer = nil;
		}
	if(workBuffer == nil)
		workBuffer = RwRasterCreate(pRasterFrontBuffer->width, pRasterFrontBuffer->height, pRasterFrontBuffer->depth, rwRASTERTYPECAMERATEXTURE);

	RwRaster *renderBuffer, *textureBuffer;

	RwRaster *drawBuffer = RwCameraGetRaster(Scene.camera);

	RwInt32 w = RwRasterGetWidth(drawBuffer);
	RwInt32 h = RwRasterGetHeight(drawBuffer);
	RwReal width = RwRasterGetWidth(pRasterFrontBuffer);
	RwReal height = RwRasterGetHeight(pRasterFrontBuffer);
	float umin, umax, vmin, vmax;

	static RwIm2DVertex verts[4];

	float nearscreen = RwIm2DGetNearScreenZ();
	float nearcam = RwCameraGetNearClipPlane(Scene.camera);
	float recipz = 1.0f/nearcam;
	for(int i = 0; i < 4; i++){
		RwIm2DVertexSetScreenZ(&verts[i], nearscreen);
		RwIm2DVertexSetCameraZ(&verts[i], nearcam);
		RwIm2DVertexSetRecipCameraZ(&verts[i], recipz);
		RwIm2DVertexSetIntRGBA(&verts[i], 255, 255, 255, 255);
	}


	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	renderBuffer = workBuffer;
	textureBuffer  = pRasterFrontBuffer;
	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, renderBuffer);
	RwCameraBeginUpdate(Scene.camera);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)textureBuffer);

	int downsampledwidth = w;
	int downsampledheight = h;

	// First step: Downsample
	for(int i = 0; i < filterPasses; i++){
		umin = (m_RadiosityFilterUCorrection + 0.5f)/width;
		umax = (downsampledwidth + 0.5f)/width;
		vmin = (m_RadiosityFilterVCorrection + 0.5f)/height;
		vmax = (downsampledheight + 0.5f)/height;

		downsampledwidth /= 2;
		downsampledheight /= 2;

		quadSetUV(verts, umin, vmin, umax, vmax);
		quadSetXY(verts, 0.0f, 0.0f, downsampledwidth+1, downsampledheight+1);

		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

		// Switch buffers
		RwRaster *tmp = renderBuffer;
		renderBuffer = textureBuffer;
		textureBuffer = tmp;
		RwD3D9SetRenderTarget(0, renderBuffer);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)textureBuffer);
	}

	// Second step: Subtract intensity value
	umin = (0 + 0.5f)/width;
	umax = (downsampledwidth+1 + 0.5f)/width;
	vmin = (0 + 0.5f)/height;
	vmax = (downsampledheight+1 + 0.5f)/height;

	quadSetUV(verts, umin, vmin, umax, vmax);
	quadSetXY(verts, 0.0f, 0.0f, downsampledwidth+1, downsampledheight+1);

	// D = 2*D - limit
	// We do 2*(D - limit/2) because the fixed function combiners can't do the above
	int limit = intensityLimit*128/255;
	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	RwD3D9SetTextureStageState(1, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, limit, limit, limit));
	RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_ADD);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_CURRENT);
	RwD3D9SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);

	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, drawBuffer);
	RwCameraBeginUpdate(Scene.camera);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)renderBuffer);

	// Third step: add to framebuffer
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)!m_bRadiosityDebug);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	umin = (0 + 0.5f)/width;
	umax = (downsampledwidth + 0.5f)/width;
	vmin = (0 + 0.5f)/height;
	vmax = (downsampledheight + 0.5f)/height;
	quadSetUV(verts, umin, vmin, umax, vmax);
	quadSetXY(verts, 0.0f, 0.0f, w, h);
	RwIm2DVertexSetIntRGBA(&verts[0], 255, 255, 255, intensity);
	RwIm2DVertexSetIntRGBA(&verts[1], 255, 255, 255, intensity);
	RwIm2DVertexSetIntRGBA(&verts[2], 255, 255, 255, intensity);
	RwIm2DVertexSetIntRGBA(&verts[3], 255, 255, 255, intensity);
	for(int i = 0; i < renderPasses; i++)
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);


	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	UpdateFrontBuffer();
}

void
CPostEffects::DarknessFilter_fix(uint8 alpha)
{
	DarknessFilter(alpha);
	UpdateFrontBuffer();
}

void
CPostEffects::ColourFilter_Generic(RwRGBA rgb1, RwRGBA rgb2, void *ps)
{
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	RwRGBAReal color, color2;
	RwRGBARealFromRwRGBA(&color, &rgb1);
	RwRGBARealFromRwRGBA(&color2, &rgb2);
	RwD3D9SetPixelShaderConstant(0, &color, 1);
	RwD3D9SetPixelShaderConstant(1, &color2, 1);

	overrideIm2dPixelShader = ps;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
}

void
CPostEffects::ColourFilter_Mobile(RwRGBA rgba1, RwRGBA rgba2)
{
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);

	if(!Colorcycle::initialised)
		Colorcycle::Initialise();

	GradeColorset cset;
	Colorcycle::Update(&cset);
	Grade red, green, blue;
	red = cset.red;
	green = cset.green;
	blue = cset.blue;

	// Mobile colors
	float r = rgba1.red + rgba2.red;
	float g = rgba1.green + rgba2.green;
	float b = rgba1.blue + rgba2.blue;
	float invsqrt = 1.0f/sqrt(r*r + g*g + b*b);
	r *= invsqrt;
	g *= invsqrt;
	b *= invsqrt;
	red.r = (1.5f + r*1.732f)*0.4f*red.r;
	green.g = (1.5f + g*1.732f)*0.4f*green.g;
	blue.b = (1.5f + b*1.732f)*0.4f*blue.b;

/*	// Fun trick: PS2 colour filter:
	float a = rgba2.alpha/128.0f;
	red.r = rgba1.red/128.0f + a*rgba2.red/128.0f;
	green.g = rgba1.green/128.0f + a*rgba2.green/128.0f;
	blue.b = rgba1.blue/128.0f + a*rgba2.blue/128.0f;
	red.g = red.b = red.a = 0.0f;
	green.r = green.b = green.a = 0.0f;
	blue.r = blue.g = blue.a = 0.0f;
*/
/*	// Also fun: PC colour filter:
	float a1 = rgba1.alpha/128.0f;
	float a2 = rgba2.alpha/128.0f;
	red.r = 1.0f + a1*rgba1.red/255.0f + a2*rgba2.red/255.0f;
	green.g = 1.0f + a1*rgba1.green/255.0f + a2*rgba2.green/255.0f;
	blue.b = 1.0f + a1*rgba1.blue/255.0f + a2*rgba2.blue/255.0f;
	red.g = red.b = red.a = 0.0f;
	green.r = green.b = green.a = 0.0f;
	blue.r = blue.g = blue.a = 0.0f;
*/


	RwD3D9SetPixelShaderConstant(0, &red, 1);
	RwD3D9SetPixelShaderConstant(1, &green, 1);
	RwD3D9SetPixelShaderConstant(2, &blue, 1);

	// contrast
	float mult[4];
	float add[4];
	mult[0] = red.r + red.g + red.b;
	mult[1] = green.r + green.g + green.b;
	mult[2] = blue.r + blue.g + blue.b;
	mult[3] = 1.0f;
	add[0] = red.a;
	add[1] = green.a;
	add[2] = blue.a;
	add[3] = 0.0f;

	RwD3D9SetPixelShaderConstant(3, mult, 1);
	RwD3D9SetPixelShaderConstant(4, add, 1);

	if(!(GetAsyncKeyState(VK_F5) & 0x8000))
		overrideIm2dPixelShader = gradingPS;
	else
		overrideIm2dPixelShader = contrastPS;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
}

void
CPostEffects::ColourFilter_PS2(RwRGBA rgba1, RwRGBA rgba2)
{
	RwIm2DVertex *verts;

	verts = colorfilterVerts;
	// Setup state
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);

	// Make Im2D use PS2 color range
	overrideColorMod = D3DTOP_MODULATE2X;
	overrideAlphaMod = D3DTOP_MODULATE2X;

	// First color - replace
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwIm2DVertexSetIntRGBA(&verts[0], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba1.red, rgba1.green, rgba1.blue, 255);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	if(m_bBlurColourFilter){
		static RwIm2DVertex blurVerts[4];
		float rasterWidth = RwRasterGetWidth(CPostEffects::pRasterFrontBuffer);
		float rasterHeight = RwRasterGetHeight(CPostEffects::pRasterFrontBuffer);
		float scale = RwRasterGetWidth(RwCameraGetRaster(Scene.camera))/640.0f;
		float leftOff   = m_colourLeftUOffset*scale   / 16.0f / rasterWidth;
		float rightOff  = m_colourRightUOffset*scale  / 16.0f / rasterWidth;
		float topOff    = m_colourTopVOffset*scale    / 16.0f / rasterHeight;
		float bottomOff = m_colourBottomVOffset*scale / 16.0f / rasterHeight;
		memcpy(blurVerts, verts, sizeof(blurVerts));
		/* These are our vertices:
		 * 0--3
		 * |\ |
		 * | \|
		 * 1--2 */
		// We can get away without setting zrecip on D3D
		RwIm2DVertexSetU(&blurVerts[0], RwIm2DVertexGetU(&blurVerts[0]) + leftOff, 1.0f);
		RwIm2DVertexSetU(&blurVerts[1], RwIm2DVertexGetU(&blurVerts[1]) + leftOff, 1.0f);
		RwIm2DVertexSetU(&blurVerts[2], RwIm2DVertexGetU(&blurVerts[2]) + rightOff, 1.0f);
		RwIm2DVertexSetU(&blurVerts[3], RwIm2DVertexGetU(&blurVerts[3]) + rightOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[0], RwIm2DVertexGetV(&blurVerts[0]) + topOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[3], RwIm2DVertexGetV(&blurVerts[3]) + topOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[1], RwIm2DVertexGetV(&blurVerts[1]) + bottomOff, 1.0f);
		RwIm2DVertexSetV(&blurVerts[2], RwIm2DVertexGetV(&blurVerts[2]) + bottomOff, 1.0f);
		verts = blurVerts;
	}

	// Second color - add
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwIm2DVertexSetIntRGBA(&verts[0], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
 	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	// Restore state
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	overrideColorMod = -1;
	overrideAlphaMod = -1;
}

/* For reference only */
#if 0
void
CPostEffects::ColourFilter_PC(RwRGBA rgba1, RwRGBA rgba2)
{
	RwIm2DVertex *verts;

	verts = colorfilterVerts;
	// Setup state
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)CPostEffects::pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);

	// First color
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwIm2DVertexSetIntRGBA(&verts[0], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba1.red, rgba1.green, rgba1.blue, rgba1.alpha);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	// Second color
	RwIm2DVertexSetIntRGBA(&verts[0], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[1], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[2], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DVertexSetIntRGBA(&verts[3], rgba2.red, rgba2.green, rgba2.blue, rgba2.alpha);
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, verts, 4, colorfilterIndices, 6);

	// Restore state
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
}
#endif

void
CPostEffects::SetFilterMainColour_PS2(RwRaster *raster, RwRGBA color)
{
//	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, pRasterFrontBuffer);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[0], color.red, color.green, color.blue, color.alpha);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[1], color.red, color.green, color.blue, color.alpha);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[2], color.red, color.green, color.blue, color.alpha);
	RwIm2DVertexSetIntRGBA(&colorfilterVerts[3], color.red, color.green, color.blue, color.alpha);
	overrideColorMod = D3DTOP_MODULATE2X;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideColorMod = -1;

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nil);
}

void
CPostEffects::InfraredVision_PS2(RwRGBA c1, RwRGBA c2)
{
	if(config->infraredVision != 0){
		InfraredVision(c1, c2);
		return;
	}

	CPostEffects::ImmediateModeRenderStatesStore();
	ImmediateModeRenderStatesSet();

	float r = m_fInfraredVisionFilterRadius;
	// not sure this scales correctly, but it looks ok (need better brain)
	float ru = r * RsGlobal->MaximumWidth  / RwRasterGetWidth(ms_imf.frontBuffer)  * 1024.0f / 640.0f;
	float rv = r * RsGlobal->MaximumHeight / RwRasterGetHeight(ms_imf.frontBuffer) * 512.0f  / 448.0f;
	float uoff[4] = { -ru, ru, ru, -ru };
	float voff[4] = { -rv, -rv, rv, rv };

	// PS2 draws the filter triangle triangle...we draw the quad
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	for(int i = 0; i < 4; i++){
		DrawQuadSetUVs(ms_imf.tri_umin + uoff[i], ms_imf.tri_vmin + voff[i],
		               ms_imf.tri_umax + uoff[i], ms_imf.tri_vmin + voff[i],
		               ms_imf.tri_umax + uoff[i], ms_imf.tri_vmax + voff[i],
		               ms_imf.tri_umin + uoff[i], ms_imf.tri_vmax + voff[i]);
		DrawQuad(0, 0, RwRasterGetWidth(ms_imf.frontBuffer)*2, RwRasterGetHeight(ms_imf.frontBuffer)*2,
		                       c1.red, c1.green, c1.blue, 0xFFu, ms_imf.frontBuffer);

		UpdateFrontBuffer();
	}
	DrawQuadSetDefaultUVs();
	ImmediateModeRenderStatesReStore();

	SetFilterMainColour_PS2(ms_imf.frontBuffer, c2);
	UpdateFrontBuffer();
}

void
CPostEffects::NightVision_PS2(RwRGBA color)
{
	if(config->nightVision != 0){
		CPostEffects::NightVision(color);
		return;
	}

	if(CPostEffects::m_fNightVisionSwitchOnFXCount > 0.0f){
		CPostEffects::m_fNightVisionSwitchOnFXCount -= CTimer__ms_fTimeStep;
		if(CPostEffects::m_fNightVisionSwitchOnFXCount <= 0.0f)
			CPostEffects::m_fNightVisionSwitchOnFXCount = 0.0f;
		CPostEffects::ImmediateModeRenderStatesStore();
		CPostEffects::ImmediateModeRenderStatesSet();
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
		int n = CPostEffects::m_fNightVisionSwitchOnFXCount;
		while(n--)
		        CPostEffects::DrawQuad(0.0f, 0.0f,
				RwRasterGetWidth(ms_imf.frontBuffer), RwRasterGetHeight(ms_imf.frontBuffer),
				8, 8, 8, 255, ms_imf.frontBuffer);
		CPostEffects::ImmediateModeRenderStatesReStore();
	}

	UpdateFrontBuffer();
	CPostEffects::SetFilterMainColour_PS2(ms_imf.frontBuffer, color);
	UpdateFrontBuffer();
}


// VU style random number generator -- taken from pcsx2
uint R;
void vrinit(uint x){ R = 0x3F800000 | x & 0x007FFFFF; }
void vradvance(void){
	int x = (R >> 4) & 1;
	int y = (R >> 22) & 1;
	R <<= 1;
	R ^= x ^ y;
	R = (R&0x7fffff)|0x3f800000;
}
inline uint vrget(void){ return R; }
inline uint vrnext(void){ vradvance(); return R; }

void
CPostEffects::Grain_PS2(int strength, bool generate)
{
	if(config->grainFilter != 0){
		CPostEffects::Grain(strength, generate);
		return;
	}

	if(generate){
		RwUInt8 *pixels = RwRasterLock(grainRaster, 0, 1);
		vrinit(rand());
		int x = vrget();
		for(int i = 0; i < 64*64; i++){
			*pixels++ = x;
			*pixels++ = x;
			*pixels++ = x;
			*pixels++ = x & strength;
			x = vrnext();
		}
		RwRasterUnlock(grainRaster);
	}

	ImmediateModeRenderStatesStore();
	ImmediateModeRenderStatesSet();
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	float umin = 0.0f;
	float vmin = 0.0f;
	float umax = 5.0f * RsGlobal->MaximumWidth/640.0f;
	float vmax = 7.0f * RsGlobal->MaximumHeight/448.0f;

	DrawQuadSetUVs(umin, vmin,
		umax, vmin,
		umax, vmax,
		umin, vmax);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)D3DBLEND_DESTCOLOR);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)D3DBLEND_SRCALPHA);

	overrideColorMod = D3DTOP_SELECTARG2;	// ignore texture color
	overrideAlphaMod = D3DTOP_MODULATE2X;
	CPostEffects::DrawQuad(0.0, 0.0, RsGlobal->MaximumWidth, RsGlobal->MaximumHeight,
	                       0xFFu, 0xFFu, 0xFFu, 0xFF, grainRaster);
	overrideColorMod = -1;
	overrideAlphaMod = -1;

	DrawQuadSetDefaultUVs();
	CPostEffects::ImmediateModeRenderStatesReStore();
}

void
CPostEffects::ColourFilter_switch(RwRGBA rgb1, RwRGBA rgb2)
{
	{
		static bool keystate = false;
		if(GetAsyncKeyState(config->keys[0]) & 0x8000){
			if(!keystate){
				keystate = true;
				if(numConfigs){
					currentConfig = (currentConfig+1) % numConfigs;
					CMessages__AddMessageJumpQWithNumber("skygfx~1~.ini", 500, 0, currentConfig + 1, -1, -1, -1, -1, -1, false);
					setConfig();
				}
			}
		}else
			keystate = false;
	}

	{
		static bool keystate = false;
		if(GetAsyncKeyState(config->keys[1]) & 0x8000){
			if(!keystate){
				keystate = true;
				reloadAllInis();
			}
		}else
			keystate = false;
	}

	RwRGBA rgb1pc = rgb1;
	RwRGBA rgb2pc = rgb2;

	if(config->usePCTimecyc){
		// Gotta fix alpha for effects that assume PS2 alpha range
		rgb1.alpha /= 2;
		rgb2.alpha /= 2;
	}else{
		// Gotta fix alpha for effects that assume PC alpha range
		// clamping this is important!
		if(rgb1pc.alpha >= 128)
			rgb1pc.alpha = 255;
		else
			rgb1pc.alpha *= 2;
		if(rgb2pc.alpha >= 128)
			rgb2pc.alpha = 255;
		else
			rgb2pc.alpha *= 2;
	}

	rgb1.red *= config->rgb1Mult;
	rgb1.green *= config->rgb1Mult;
	rgb1.blue *= config->rgb1Mult;

	rgb2.red *= config->rgb2Mult;
	rgb2.green *= config->rgb2Mult;
	rgb2.blue *= config->rgb2Mult;

	vcsblurrgb = rgb2;

	int colorFilter = config->colorFilter;

	// VCS trails isn't compatible with PC/PS2 color filter, falls of to VCS color filter
	if (config->vcsTrails) {
		if (colorFilter == COLORFILTER_PC || colorFilter == COLORFILTER_PS2) {
			colorFilter = COLORFILTER_VCS;
		}
	}

	switch(colorFilter){
	case COLORFILTER_PS2:
		CPostEffects::ColourFilter_PS2(rgb1, rgb2);
		break;
	case COLORFILTER_PC:
		// this effects expects PC alphas
		CPostEffects::ColourFilter(rgb1pc, rgb2pc);
		break;
	case COLORFILTER_MOBILE:
		// this effects ignores alphas
		if(!UG_mod)
			CPostEffects::ColourFilter_Mobile(rgb1, rgb2);
		break;
	case COLORFILTER_III:
		// this effects expects PC alphas
		CPostEffects::ColourFilter_Generic(rgb1pc, rgb2pc, iiiTrailsPS);
		break;
	case COLORFILTER_VC:
		// this effects ignores alphas
		CPostEffects::ColourFilter_Generic(rgb1, rgb2, vcTrailsPS);
		break;
	case COLORFILTER_VCS:
		// this effects ignores alphas
		CPostEffects::ColourFilter_Generic(rgb1, rgb2, vcTrailsPS);
		break;
	default:
		return;
	}
	UpdateFrontBuffer();

	//static int doramp = 0;
	//{
	//	static bool keystate = false;
	//	if(GetAsyncKeyState(VK_F4) & 0x8000){
	//		if(!keystate){
	//			doramp = !doramp;
	//			keystate = true;
	//		}
	//	}else
	//		keystate = false;
	//}
	//if(doramp)
	//	renderRamp();
}

static RwMatrix RGB2YUV = {
	{  0.299f,	-0.168736f,	 0.500f }, 0,
	{  0.587f,	-0.331264f,	-0.418688f }, 0,
	{  0.114f,	 0.500f,	-0.081312f }, 0,
	{  0.000f,	 0.000f,	 0.000f }, 0,
};

static RwMatrix YUV2RGB = {
	{  1.000f,	 1.000f,	 1.000f }, 0,
	{  0.000f,	-0.344136f,	 1.772f }, 0,
	{  1.402f,	-0.714136f,	 0.000f }, 0,
	{  0.000f,	 0.000f,	 0.000f }, 0,
};

////
//// Bloom / exposure / tone map / PS2 dither
////

void *brightPS, *bloomBlurPS, *finalPS;

RwRaster *bloomRasterA, *bloomRasterB;
RwTexture *bloomTextureA, *bloomTextureB;
RwRaster *ditherRaster;
RwTexture *ditherTexture;
static int bloomLastW, bloomLastH, ditherLastW, ditherLastH;

static uint32 ditherRngState = 0x12345678;
static uint32
ditherRng(void)
{
	ditherRngState ^= ditherRngState << 13;
	ditherRngState ^= ditherRngState >> 17;
	ditherRngState ^= ditherRngState << 5;
	return ditherRngState;
}

// 8x8 blue noise via Munch's algorithm: start from a random permutation and
// repeatedly swap the value closest to the mean with the one farthest from it
static void
makeBlueNoise8x8(uint8 *out)
{
	uint8 v[64];
	int i;
	for(i = 0; i < 64; i++)
		v[i] = (uint8)i;
	for(i = 63; i > 0; i--){
		int j = ditherRng() % (i + 1);
		uint8 t = v[i]; v[i] = v[j]; v[j] = t;
	}
	for(int iter = 0; iter < 4096; iter++){
		int iclose = 0, ifar = 0;
		float dclose = 1e9f, dfar = -1e9f;
		for(i = 0; i < 64; i++){
			float d = fabsf((float)v[i] - 31.5f);
			if(d < dclose){ dclose = d; iclose = i; }
			if(d > dfar){ dfar = d; ifar = i; }
		}
		if(iclose != ifar){
			uint8 t = v[iclose]; v[iclose] = v[ifar]; v[ifar] = t;
		}
	}
	for(i = 0; i < 64; i++)
		out[i] = (uint8)((v[i] * 255) / 63);
}

static bool
ensureBloomBuffers(int w, int h)
{
	if(bloomRasterA && bloomLastW == w && bloomLastH == h)
		return true;
	// envmap.cpp pattern: the RwTexture persists, only the raster is
	// recreated and re-attached (RwTextureDestroy isn't wrapped)
	if(!bloomTextureA){
		bloomTextureA = RwTextureCreate(nil);
		bloomTextureB = RwTextureCreate(nil);
	}
	if(bloomRasterA) RwRasterDestroy(bloomRasterA);
	if(bloomRasterB) RwRasterDestroy(bloomRasterB);
	int depth = CPostEffects::pRasterFrontBuffer->depth;
	bloomRasterA = RwRasterCreate(w, h, depth, rwRASTERTYPECAMERATEXTURE);
	bloomRasterB = RwRasterCreate(w, h, depth, rwRASTERTYPECAMERATEXTURE);
	if(!bloomRasterA || !bloomRasterB)
		return false;
	RwTextureSetRaster(bloomTextureA, bloomRasterA);
	RwTextureSetRaster(bloomTextureB, bloomRasterB);
	bloomLastW = w;
	bloomLastH = h;
	return true;
}

// Render targets are switched the way the proven Radiosity_shader code does
// it: by swapping the camera raster. Never point RwD3D9SetRenderTarget at the
// camera raster itself - in this RW build its D3D render target surface is
// managed by the game, not the raster, and that code path dereferences a NULL
// surface (access violation inside RwD3D9SetRenderTarget, reported by users).
static void
setSceneRaster(RwRaster *r)
{
	RwCameraEndUpdate(Scene.camera);
	RwCameraSetRaster(Scene.camera, r);
	RwCameraBeginUpdate(Scene.camera);
}

// dither pattern: one 8x8 blue noise tile per 8x8 screen pixels, so sampling
// the raster with screen-space UVs gives per-pixel noise without any extra
// constants or texture transforms
static bool
ensureDitherTexture(int w, int h)
{
	int dw = (w + 7) / 8;
	int dh = (h + 7) / 8;
	if(ditherRaster && ditherLastW == w && ditherLastH == h)
		return true;
	if(!ditherTexture)
		ditherTexture = RwTextureCreate(nil);
	if(ditherRaster) RwRasterDestroy(ditherRaster);
	ditherRaster = RwRasterCreate(dw, dh, 32, rwRASTERTYPECAMERATEXTURE);
	if(!ditherRaster)
		return false;
	uint8 noise[64];
	makeBlueNoise8x8(noise);
	RwUInt8 *pixels = RwRasterLock(ditherRaster, 0, 1);
	for(int y = 0; y < dh; y++)
		for(int x = 0; x < dw; x++){
			uint8 v = noise[(y & 7) * 8 + (x & 7)];
			*pixels++ = v;
			*pixels++ = v;
			*pixels++ = v;
			*pixels++ = 0xFF;
		}
	RwRasterUnlock(ditherRaster);
	RwTextureSetRaster(ditherTexture, ditherRaster);
	ditherLastW = w;
	ditherLastH = h;
	return true;
}

////
//// Screen FX 2: PS2 grain+scanlines, auto exposure, night bloom boost,
//// renderScale (internal resolution)
////

// 128x128 texture with a per-pixel grain and a 2px scanline pattern pre-baked:
// g in [-1,1] = (noise-0.5)*1.2 (grain, range [-0.6, 0.6])
//               + (row%2 ? -0.5 : 0) (scanline darkens every other 2px row)
// stored as (g+1)/2 in 8-bit. 128 is even, so tiling keeps the 2px row parity
// aligned on screen at any resolution.
RwRaster *sfxGrainRaster;
RwTexture *sfxGrainTexture;
static int sfxGrainLastW, sfxGrainLastH;

// PS2-style grain + 2px scanlines, baked per screen pixel (same pattern as
// the dither texture: full screen size, rebuilt on resolution change).
static uint32 sfxGrainRng = 0x9E3779B9;
static uint32
sfxGrainRngNext(void)
{
	sfxGrainRng ^= sfxGrainRng << 13;
	sfxGrainRng ^= sfxGrainRng >> 17;
	sfxGrainRng ^= sfxGrainRng << 5;
	return sfxGrainRng;
}

static bool
ensureSfxGrainTexture(int w, int h)
{
	if(sfxGrainRaster && sfxGrainLastW == w && sfxGrainLastH == h)
		return true;
	if(!sfxGrainTexture)
		sfxGrainTexture = RwTextureCreate(nil);
	if(sfxGrainRaster) RwRasterDestroy(sfxGrainRaster);
	sfxGrainRaster = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE);
	if(!sfxGrainRaster)
		return false;
	sfxGrainRng = 0x9E3779B9;
	uint8 *px = RwRasterLock(sfxGrainRaster, 0, 1);
	for(int y = 0; y < h; y++)
		for(int x = 0; x < w; x++){
			float n = (float)((sfxGrainRngNext() >> 8) & 0xFF) / 255.0f;
			float g = (n - 0.5f) * 1.2f;
			if(y & 1)
				g -= 0.5f;
			if(g < -1.0f) g = -1.0f;
			if(g > 1.0f) g = 1.0f;
			uint8 v = (uint8)((g + 1.0f) * 0.5f * 255.0f + 0.5f);
			*px++ = v;
			*px++ = v;
			*px++ = v;
			*px++ = 0xFF;
		}
	RwRasterUnlock(sfxGrainRaster);
	RwTextureSetRaster(sfxGrainTexture, sfxGrainRaster);
	sfxGrainLastW = w;
	sfxGrainLastH = h;
	return true;
}

// renderScale: render the scene into a smaller camera texture and upscale it
// in the final composite (softer PS2-like look + fewer scene pixels).
// RenderScale_Begin() is called from RenderScene_before() in main.cpp before
// the scene is drawn; DrawFinalEffects() stretches the scene into the front
// buffer again.
//
// v9 design: the scene raster, the depth surface and the camera are left
// EXACTLY as the game set them up - nothing is swapped. v8 proved that
// rendering into a swapped-in smaller camera raster is what breaks the frame
// under D3D9On12: its log showed raster and viewport perfectly in sync
// (1440x810/1440x810), yet every depth-tested mesh vanished while sky and
// ground haze, which render without z-test, survived - the depth surface
// that comes with the swapped raster does not work there. So the downscale
// is done with the D3D viewport alone: a 1440x810 viewport on the untouched
// 1600x900 raster remaps the whole NDC cube into that rectangle - same
// projection, same FOV, same image, just fewer pixels (fill rate is still
// saved, rasterization is bounded by the viewport). DrawFinalEffects() then
// stretches exactly that rectangle into the front buffer (UVs 0..scale), so
// nothing is cropped and nothing can zoom. While the scene renders, every
// SetViewport that does not match the scaled size is rewritten through the
// hooked vtable slot, and the scaled viewport is re-asserted after each
// in-scene projection change.
static uint8 sfxScaleActive, sfxScaleVtPatched, sfxScaleVtTried, sfxVpScaled;
static uint8 sfxScaleApplied; // frame uses the scaled viewport (DrawFinalEffects)
static unsigned int sfxScaleW, sfxScaleH; // scaled viewport size (even)
static unsigned int sfxSceneW, sfxSceneH; // full scene raster size (log only)

struct SfxD3DViewport
{
	long x, y;
	unsigned int width, height;
	float minz, maxz;
}; // D3DVIEWPORT9 is 24 bytes - GetViewport() writes all of it
static struct SfxD3DViewport sfxVpFull;

// D3D9 device vtable slots (IDirect3DDevice9 layout, d3d9.h order:
// ... EndScene=42, Clear=43, then):
//   44 = SetTransform(int type, const D3DMATRIX *)
//   45 = GetTransform(int type, D3DMATRIX *)
//   47 = SetViewport(const D3DVIEWPORT9 *)
//   48 = GetViewport(D3DVIEWPORT9 *)
// Earlier revisions hooked 8/9/15/16, which are GetDisplayMode /
// GetCreationParameters / GetNumberOfSwapChains / Reset - that is why the
// v7 log showed "install: OK" but never a single viewport/transform event.
// Device methods are declared without D3D9 SDK types (the plugin SDK
// include chain does not reliably provide the interfaces in this
// translation unit): opaque pointers + x86 __stdcall only.
// D3DMATRIX layout: { float m[4][4]; }
// D3DTS_VIEW = 2, D3DTS_PROJECTION = 3, D3D_OK = 0
typedef int (__stdcall *sfxD3D2ArgFn)(void *, void *);
typedef int (__stdcall *sfxD3D3ArgFn)(void *, int, void *);
static sfxD3D2ArgFn d3dSetViewportOrig;
static sfxD3D2ArgFn d3dGetViewport;
static sfxD3D3ArgFn d3dSetTransformOrig;

// does this viewport differ from the scaled scene viewport we want?
static bool
sfxVpNotScaled(const struct SfxD3DViewport *vp)
{
	return vp->x != 0 || vp->y != 0
		|| vp->width != sfxScaleW || vp->height != sfxScaleH;
}

// --- diagnostics (renderScaleDebugLog=1): append D3D state events to
// skygfx_renderScale.log in the game folder (first 20000 lines) ---
static FILE *sfxLog;
static int sfxLogCount;
static int sfxLogV0, sfxLogT0, sfxLogOther; // caps for off-window events
static int sfxLogT, sfxLogC, sfxLogS, sfxLogP; // caps for in-window events
static void
sfxLogLine(const char *fmt, ...)
{
	if(!config->renderScaleDebugLog || sfxLogCount >= 20000)
		return;
	if(sfxLog == nil){
		sfxLog = fopen("skygfx_renderScale.log", "a");
		if(sfxLog == nil)
			return;
		fprintf(sfxLog, "==== skygfx renderScale diagnostics (build v9) ====\n");
	}
	va_list ap;
	va_start(ap, fmt);
	vfprintf(sfxLog, fmt, ap);
	va_end(ap);
	if((++sfxLogCount % 32) == 0)
		fflush(sfxLog);
}

// diagnostics only: the projection matrix itself is never modified (the
// viewport clamp below is the entire fix) - we just record what the game
// issues and while the scale window is open. D3DTS_PROJECTION = 3; the
// old code tested for 2, which is D3DTS_VIEW.
// diagnostics only for transforms - the projection matrix itself is never
// modified (the viewport is the entire fix). But right after each in-scene
// projection change we re-assert the scaled viewport, in case anything in
// between restored the full one. D3DTS_PROJECTION = 3.
static int __stdcall
sfxSetTransformHook(void *dev, int type, void *m)
{
	if(type == 3 /* D3DTS_PROJECTION */){
		if(sfxScaleActive){
			if(sfxLogT++ < 8)
				sfxLogLine("T proj win=1\n");
			if(sfxVpScaled && d3dSetViewportOrig && sfxLogP++ < 8){
				struct SfxD3DViewport c = {0, 0, sfxScaleW, sfxScaleH, 0.0f, 1.0f};
				d3dSetViewportOrig(dev, &c);
				sfxLogLine("P reassert %ux%u\n", sfxScaleW, sfxScaleH);
			}
		}else if(sfxLogT0++ < 8)
			sfxLogLine("T0 proj win=0\n");
	}else if(sfxLogOther++ < 6)
		sfxLogLine("t type=%d win=%d\n", type, (int)sfxScaleActive);
	return d3dSetTransformOrig(dev, type, m);
}

// the whole fix: while the scale window is open, every SetViewport that does
// not match the scaled viewport is rewritten to it before it reaches the
// device - full FOV in fewer pixels, no zoom, no matrix tricks, no matter
// who resets the viewport and when
static int __stdcall
sfxSetViewportHook(void *dev, void *vp)
{
	struct SfxD3DViewport *v = (struct SfxD3DViewport*)vp;
	if(sfxScaleActive){
		if(sfxVpNotScaled(v)){
			struct SfxD3DViewport c = {0, 0, sfxScaleW, sfxScaleH, 0.0f, 1.0f};
			if(sfxLogC++ < 16)
				sfxLogLine("C vp=%ux%u -> %ux%u\n",
					v->width, v->height, c.width, c.height);
			return d3dSetViewportOrig(dev, &c);
		}
		if(sfxLogS++ < 8)
			sfxLogLine("S vp=%ux%u\n", v->width, v->height);
	}else if(sfxLogV0++ < 8)
		sfxLogLine("V0 vp=%ux%u\n", v->width, v->height);
	return d3dSetViewportOrig(dev, vp);
}

static void
sfxScaleInstallVtableHook(void)
{
	if(sfxScaleVtPatched || sfxScaleVtTried || d3d9device == nil)
		return;
	// the vtable is the FIRST member of the device object; the object
	// itself must never be written (earlier revisions corrupted it)
	void **vt = (void**)(*(void**)d3d9device);
	sfxLogLine("install: dev=%08x vt=%08x\n",
		(unsigned int)(void*)d3d9device, (unsigned int)vt);
	if(vt == nil){
		sfxScaleVtTried = 1;
		return;
	}
	d3dSetTransformOrig = (sfxD3D3ArgFn)vt[44];
	d3dSetViewportOrig = (sfxD3D2ArgFn)vt[47];
	d3dGetViewport = (sfxD3D2ArgFn)vt[48];
	sfxLogLine("install: orig SetTransform44=%08x SetViewport47=%08x GetViewport48=%08x\n",
		(unsigned int)(void*)d3dSetTransformOrig,
		(unsigned int)(void*)d3dSetViewportOrig,
		(unsigned int)(void*)d3dGetViewport);
	// plausibility check: the "original" methods must be normal 32-bit
	// code pointers. No module is assumed here: a d3d9-on-d12 style
	// wrapper may implement the device in a different module.
	if((unsigned int)(void*)d3dSetViewportOrig < 0x10000
		|| (unsigned int)(void*)d3dSetViewportOrig >= 0x80000000
		|| (unsigned int)(void*)d3dGetViewport < 0x10000
		|| (unsigned int)(void*)d3dGetViewport >= 0x80000000
		|| (unsigned int)(void*)d3dSetTransformOrig < 0x10000
		|| (unsigned int)(void*)d3dSetTransformOrig >= 0x80000000){
		sfxLogLine("install: ABORT - original pointers not plausible\n");
		sfxScaleVtTried = 1;
		return;
	}
	// the vtable lives in a read-only section, so its page has to be
	// made writable just for the patch and restored afterwards
	size_t span = (size_t)((char*)&vt[48] - (char*)&vt[44]);
	DWORD oldProt = 0;
	if(!VirtualProtect(&vt[44], span, PAGE_READWRITE, &oldProt)){
		sfxLogLine("install: ABORT - VirtualProtect failed (err=%u)\n",
			(unsigned int)GetLastError());
		sfxScaleVtTried = 1;
		return;
	}
	Patch((void*)(&vt[47]), (void*)sfxSetViewportHook);
	Patch((void*)(&vt[44]), (void*)sfxSetTransformHook);
	VirtualProtect(&vt[44], span, oldProt, &oldProt);
	// verify the entries really took (read them back through the table)
	if(vt[47] != (void*)sfxSetViewportHook || vt[44] != (void*)sfxSetTransformHook){
		sfxLogLine("install: ABORT - readback mismatch s44=%08x s47=%08x\n",
			(unsigned int)(void*)vt[44], (unsigned int)(void*)vt[47]);
		sfxScaleVtTried = 1;
		return;
	}
	sfxLogLine("install: OK - vtable hooked SetTransform44=%08x SetViewport47=%08x\n",
		(unsigned int)(void*)sfxSetTransformHook,
		(unsigned int)(void*)sfxSetViewportHook);
	sfxScaleVtPatched = 1;
	sfxScaleVtTried = 1;
}

void
RenderScale_Begin(void)
{
	float s = config->renderScale;
	if(s < 0.5f)
		s = 0.5f;
	if(s >= 1.0f){
		sfxScaleActive = 0;
		sfxScaleApplied = 0;
		return;
	}
	RwRaster *camR = RwCameraGetRaster(Scene.camera);
	if(camR == nil || camR->width < 64 || camR->height < 64){
		sfxScaleActive = 0;
		sfxScaleApplied = 0;
		return;
	}
	int w = ((int)(camR->width * s) + 1) & ~1;
	int h = ((int)(camR->height * s) + 1) & ~1;
	if(w > camR->width) w = camR->width & ~1;
	if(h > camR->height) h = camR->height & ~1;
	sfxScaleInstallVtableHook();
	if(!sfxScaleVtPatched || !d3dSetViewportOrig || !d3dGetViewport){
		// without the vtable hooks we cannot control the viewport
		sfxScaleActive = 0;
		sfxScaleApplied = 0;
		return;
	}
	sfxScaleW = w;
	sfxScaleH = h;
	sfxSceneW = camR->width;
	sfxSceneH = camR->height;
	memset(&sfxVpFull, 0, sizeof(sfxVpFull));
	if(d3dGetViewport(d3d9device, &sfxVpFull) != 0)
		memset(&sfxVpFull, 0, sizeof(sfxVpFull)); // unknown - don't restore garbage
	// open the window and put the scaled viewport on the device; the scene
	// raster and depth surface are NOT touched (see the v9 design note above)
	sfxScaleActive = 1;
	sfxScaleApplied = 1;
	struct SfxD3DViewport vp = {0, 0, (unsigned int)w, (unsigned int)h, 0.0f, 1.0f};
	d3dSetViewportOrig(d3d9device, &vp);
	sfxVpScaled = 1;
	sfxLogLine("B begin: scale=%.2f raster=%ux%u vp=%ux%u -> force %dx%d\n",
		s, sfxSceneW, sfxSceneH, sfxVpFull.width, sfxVpFull.height, w, h);
}

// Called from RenderScene_after() once the 3D scene pass is done: hand the
// full-size viewport back so the reflection/env passes and the game's own
// post effects keep normal D3D state. DrawFinalEffects() may already have
// restored it (it needs the full viewport for the composite) - then this is
// a no-op.
void
RenderScale_EndOfScene(void)
{
	if(sfxVpScaled && sfxVpFull.width != 0 && d3dSetViewportOrig){
		d3dSetViewportOrig(d3d9device, &sfxVpFull);
		sfxLogLine("R vp restored %ux%u\n", sfxVpFull.width, sfxVpFull.height);
	}
	sfxVpScaled = 0;
	sfxScaleActive = 0;
}

void
CPostEffects::DrawFinalEffects(void)
{
	bool doYCbCr = m_bYCbCrFilter;
	bool doBloom = config->doBloom;
	bool doToneMap = config->doToneMap;
	bool doDither = config->ps2Dither;
	float exposure = config->exposure;

	bool doVignette = config->vignetteStrength > 0.0f;
	bool doCA = config->chromaticAberration > 0.0f;
	bool doGrain = config->ps2Grain != 0;
	float grainStrength = config->ps2GrainStrength;
	bool doAutoExp = config->doAutoExposure != 0;

	// renderScale: the scene was drawn with a scaled viewport into the
	// top-left sfxScaleW x sfxScaleH of the original raster - stretch exactly
	// that region into the front buffer below (taking the full-size viewport
	// back first)
	bool scaled = sfxScaleApplied != 0;
	sfxScaleApplied = 0;

	if(!doYCbCr && !doBloom && !doToneMap && !doDither && !doVignette && !doCA
			&& !doGrain && !doAutoExp && exposure == 1.0f && !scaled)
		return;
	if(finalPS == nil)
		return;

	int w = RwRasterGetWidth(pRasterFrontBuffer);
	int h = RwRasterGetHeight(pRasterFrontBuffer);
	if(w <= 0 || h <= 0)
		return;

	if(doBloom && !ensureBloomBuffers(w, h))
		doBloom = false;
	if(doDither && !ensureDitherTexture(w, h))
		doDither = false;
	if(doGrain && !ensureSfxGrainTexture(w, h))
		doGrain = false;

	RwRaster *drawBuffer = RwCameraGetRaster(Scene.camera);
	if(drawBuffer == nil)
		return;

	// scene, after all game post effects, into the front buffer
	if(scaled){
		// the scene lives in the top-left sfxScaleW x sfxScaleH of the full
		// raster (scaled viewport) - stretch exactly that rectangle. First
		// take the full-size viewport back: this quad and the final
		// composite below cover the whole raster
		if(sfxVpScaled && sfxVpFull.width != 0 && d3dSetViewportOrig){
			d3dSetViewportOrig(d3d9device, &sfxVpFull);
			sfxVpScaled = 0;
			sfxLogLine("F vp restored %ux%u\n", sfxVpFull.width, sfxVpFull.height);
		}
		// like UpdateFrontBuffer(), but with a textured quad whose UVs stop
		// at the scaled rectangle instead of a 1:1 RwRasterRenderFast blit
		static RwIm2DVertex sv[4];
		float nearscreen = RwIm2DGetNearScreenZ();
		float nearcam = RwCameraGetNearClipPlane(Scene.camera);
		float recipz = 1.0f/nearcam;
		float uw = sfxScaleW / (float)drawBuffer->width;
		float vh = sfxScaleH / (float)drawBuffer->height;
		quadSetUV(sv, 0.0f, 0.0f, uw, vh);
		quadSetXY(sv, 0.0f, 0.0f, (float)w, (float)h);
		for(int i = 0; i < 4; i++){
			RwIm2DVertexSetScreenZ(&sv[i], nearscreen);
			RwIm2DVertexSetCameraZ(&sv[i], nearcam);
			RwIm2DVertexSetRecipCameraZ(&sv[i], recipz);
			RwIm2DVertexSetIntRGBA(&sv[i], 255, 255, 255, 255);
		}
		RwCameraEndUpdate(Scene.camera);
		RwRasterPushContext(pRasterFrontBuffer);
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
		RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
		RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)drawBuffer);
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, sv, 4, colorfilterIndices, 6);
		RwRasterPopContext();
		RwCameraBeginUpdate(Scene.camera);
	}else
		UpdateFrontBuffer();

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// ---- auto exposure + night detection (smoothed ambient light level) ----
	// CTimeCycle_GetAmbient* give the current timecycle ambient color (0..1);
	// their average is ~0.3-0.5 in daylight and ~0.05-0.15 at night.
	static float sfxNight = 0.0f;
	static float sfxAutoExpCur = 1.0f;
	{
		float amb = ((float)CTimeCycle_GetAmbientRed()
				+ (float)CTimeCycle_GetAmbientGreen()
				+ (float)CTimeCycle_GetAmbientBlue()) / 3.0f;
		float night = 1.0f - amb / 0.45f;
		if(night < 0.0f) night = 0.0f;
		if(night > 1.0f) night = 1.0f;
		sfxNight += (night - sfxNight) * 0.05f;
		if(doAutoExp){
			float target = 1.0f + sfxNight * config->autoExposureGain;
			sfxAutoExpCur += (target - sfxAutoExpCur) * 0.05f;
		}else
			sfxAutoExpCur = 1.0f;
	}
	if(doAutoExp)
		exposure *= sfxAutoExpCur;
	float bloomIntensity = doBloom
			? config->bloomIntensity * (1.0f + config->bloomNightBoost * sfxNight)
			: 0.0f;

	// ---- bloom: bright pass + separable blur iterations, ping-pong A/B ----
	// Each (vertical, horizontal) pair starts and ends in `src`, so after every
	// iteration `src` holds the newest result - no pointer swapping needed.
	RwRaster *bloomResult = nil;
	RwTexture *bloomResultTex = nil;
	if(doBloom){
		RwRaster *src, *dst;
		float th[4];
		float off[4];
		float invw = 1.0f / w;
		float invh = 1.0f / h;
		int i;

		// bright pass: front buffer -> A
		th[0] = config->bloomThreshold;
		th[1] = th[2] = th[3] = 0.0f;
		RwD3D9SetPixelShaderConstant(0, th, 1);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);
		setSceneRaster(bloomRasterA);
		overrideIm2dPixelShader = brightPS;
		RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
		overrideIm2dPixelShader = nil;

		src = bloomRasterA; dst = bloomRasterB;
		for(i = 0; i < config->bloomIterations; i++){
			// vertical: read src, write dst
			off[0] = 0.0f; off[1] = invh; off[2] = off[3] = 0.0f;
			RwD3D9SetPixelShaderConstant(0, off, 1);
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)src);
			setSceneRaster(dst);
			overrideIm2dPixelShader = bloomBlurPS;
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
			overrideIm2dPixelShader = nil;
			// horizontal: read dst, write back to src
			off[0] = invw; off[1] = 0.0f;
			RwD3D9SetPixelShaderConstant(0, off, 1);
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)dst);
			setSceneRaster(src);
			overrideIm2dPixelShader = bloomBlurPS;
			RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
			overrideIm2dPixelShader = nil;
		}
		// each (vertical, horizontal) pair ends in src, so src holds the
		// newest result after the last iteration
		bloomResult = src;
		bloomResultTex = bloomTextureA;
	}

	// ---- final composite: scene (+bloom), exposure, tone map, grading,
	//      vignette, chromatic aberration, grain/scanlines, dither ----
	if(doBloom)
		setSceneRaster(drawBuffer); // restore the scene raster as render target
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERNEAREST);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pRasterFrontBuffer);

	// grading matrix: YCbCr tweak, or identity when it's not enabled
	Grade red, green, blue;
	if(doYCbCr){
		RwMatrix m = RGB2YUV;
		RwMatrix m2;
		m2.right.x = m_lumaScale;
		m2.up.x = 0.0f;
		m2.at.x = 0.0f;
		m2.pos.x = m_lumaOffset;
		m2.right.y = 0.0f;
		m2.up.y = m_cbScale;
		m2.at.y = 0.0f;
		m2.pos.y = m_cbOffset;
		m2.right.z = 0.0f;
		m2.up.z = 0.0f;
		m2.at.z = m_crScale;
		m2.pos.z = m_crOffset;

		RwMatrixOptimize(&m2, nil);

		RwMatrixTransform(&m, &m2, rwCOMBINEPOSTCONCAT);
		RwMatrixTransform(&m, &YUV2RGB, rwCOMBINEPOSTCONCAT);
		red.r = m.right.x;
		red.g = m.up.x;
		red.b = m.at.x;
		red.a = m.pos.x;
		green.r = m.right.y;
		green.g = m.up.y;
		green.b = m.at.y;
		green.a = m.pos.y;
		blue.r = m.right.z;
		blue.g = m.up.z;
		blue.b = m.at.z;
		blue.a = m.pos.z;
	}else{
		red.r = 1.0f; red.g = red.b = red.a = 0.0f;
		green.g = 1.0f; green.r = green.b = green.a = 0.0f;
		blue.b = 1.0f; blue.r = blue.g = blue.a = 0.0f;
	}
	RwD3D9SetPixelShaderConstant(0, &red, 1);
	RwD3D9SetPixelShaderConstant(1, &green, 1);
	RwD3D9SetPixelShaderConstant(2, &blue, 1);

	{
		float params[4];
		params[0] = exposure;
		params[1] = doToneMap ? config->whitePoint : 0.0f;
		params[2] = bloomResult ? bloomIntensity : 0.0f;
		params[3] = doDither ? 1.0f : 0.0f;
		RwD3D9SetPixelShaderConstant(3, params, 1);
	}
	{
		float fx[4];
		fx[0] = doVignette ? config->vignetteStrength : 0.0f;
		fx[1] = doCA ? config->chromaticAberration : 0.0f;
		fx[2] = doGrain ? grainStrength : 0.0f;
		fx[3] = 0.0f;
		RwD3D9SetPixelShaderConstant(4, fx, 1);
	}

	RwD3D9SetTexture(bloomResultTex, 1);
	RwD3D9SetTexture(doDither ? ditherTexture : nil, 2);
	RwD3D9SetTexture(doGrain ? sfxGrainTexture : nil, 3);

	overrideIm2dPixelShader = finalPS;
	RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, colorfilterVerts, 4, colorfilterIndices, 6);
	overrideIm2dPixelShader = nil;

	RwD3D9SetTexture(nil, 1);
	RwD3D9SetTexture(nil, 2);
	RwD3D9SetTexture(nil, 3);

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
}

void (*CPostEffects::Initialise_orig)(void);
void
CPostEffects::Initialise(void)
{
	Initialise_orig();
	Initialise_skygfx(nil);
}

bool
CPostEffects::Initialise_skygfx(void*)
{
	InjectHook(0x7FB824, Im2dSetPixelShader_hook);
	InjectHook(0x7FB885, Im2DColorModulationHook);
	InjectHook(0x7FB8A6, Im2DAlphaModulationHook);

	CreateShaders();

	grainRaster = RwRasterCreate(64, 64, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
	return true;
}


// Colorcycle stuff, partly taken from NTAuthority...at least originally

class CFileMgr
{
public:
	static void* OpenFile(const char* filename, const char* mode);

	static void  CloseFile(void* file);
};

class CFileLoader
{
public:
	static char* LoadLine(void* file);
};

WRAPPER void* CFileMgr::OpenFile(const char* filename, const char* mode) { EAXJMP(0x538900); }
WRAPPER void  CFileMgr::CloseFile(void* file) { EAXJMP(0x5389D0); }
WRAPPER char* CFileLoader::LoadLine(void* file) { EAXJMP(0x536F80); }

static int &CTimeCycle__m_ExtraColourWeatherType = *(int*)0xB79E40;
static int &CTimeCycle__m_ExtraColour = *(int*)0xB79E44;
static int &CTimeCycle__m_bExtraColourOn = *(int*)0xB7C484;
static float &CTimeCycle__m_ExtraColourInter = *(float*)0xB79E3C;
static float &CWeather__UnderWaterness = *(float*)0xC8132C;
static float &CWeather__InTunnelness = *(float*)0xC81334;
static int &tunnelWeather = *(int*)0x8CDEE0;


// 24 instead of NUMHOURS because we might be using timecycle_24h with extended extra colour hours
Grade Colorcycle::redGrade[24][NUMWEATHERS];
Grade Colorcycle::greenGrade[24][NUMWEATHERS];
Grade Colorcycle::blueGrade[24][NUMWEATHERS];
bool Colorcycle::initialised;

GradeColorset::GradeColorset(int h, int w)
{
	this->red = Colorcycle::redGrade[h][w];
	this->green = Colorcycle::greenGrade[h][w];
	this->blue = Colorcycle::blueGrade[h][w];
}

void
GradeColorset::Interpolate(GradeColorset *a, GradeColorset *b, float fa, float fb)
{
	this->red.r = fa * a->red.r + fb * b->red.r;
	this->red.g = fa * a->red.g + fb * b->red.g;
	this->red.b = fa * a->red.b + fb * b->red.b;
	this->red.a = fa * a->red.a + fb * b->red.a;
	this->green.r = fa * a->green.r + fb * b->green.r;
	this->green.g = fa * a->green.g + fb * b->green.g;
	this->green.b = fa * a->green.b + fb * b->green.b;
	this->green.a = fa * a->green.a + fb * b->green.a;
	this->blue.r = fa * a->blue.r + fb * b->blue.r;
	this->blue.g = fa * a->blue.g + fb * b->blue.g;
	this->blue.b = fa * a->blue.b + fb * b->blue.b;
	this->blue.a = fa * a->blue.a + fb * b->blue.a;
}

static int timecycleHours[] = { 0, 5, 6, 7, 12, 19, 20, 22, 24 };

void
Colorcycle::Update(GradeColorset *colorset)
{
	float time;
	int curHourSel, nextHourSel;
	int curHour, nextHour;
	float timeInterp, invTimeInterp, weatherInterp, invWeatherInterp;

	time = CClock__ms_nGameClockMinutes / 60.0f
	     + CClock__ms_nGameClockSeconds / 3600.0f
	     + CClock__ms_nGameClockHours;
	if(time >= 23.999f)
		time = 23.999f;

	for(curHourSel = 0; time >= timecycleHours[curHourSel+1]; curHourSel++);
	nextHourSel = (curHourSel + 1) % NUMHOURS;
	curHour = timecycleHours[curHourSel];
	nextHour = timecycleHours[curHourSel+1];
	timeInterp = (time - curHour) / (float)(nextHour - curHour);
	invTimeInterp = 1.0f - timeInterp;
	weatherInterp = CWeather__InterpolationValue;
	invWeatherInterp = 1.0f - weatherInterp;
	GradeColorset curold(curHourSel, CWeather__OldWeatherType);
	GradeColorset nextold(nextHourSel, CWeather__OldWeatherType);
	GradeColorset curnew(curHourSel, CWeather__NewWeatherType);
	GradeColorset nextnew(nextHourSel, CWeather__NewWeatherType);

	// Skipping smog weather handling
	GradeColorset oldInterp, newInterp;
	oldInterp.Interpolate(&curold, &nextold, invTimeInterp, timeInterp);
	newInterp.Interpolate(&curnew, &nextnew, invTimeInterp, timeInterp);
	colorset->Interpolate(&oldInterp, &newInterp, invWeatherInterp, weatherInterp);

	float inc = CTimer__ms_fTimeStep/120.0f;
	if(CTimeCycle__m_bExtraColourOn){
		CTimeCycle__m_ExtraColourInter += inc;
		if(CTimeCycle__m_ExtraColourInter > 1.0f)
			CTimeCycle__m_ExtraColourInter = 1.0f;
	}else{
		CTimeCycle__m_ExtraColourInter -= inc;
		if(CTimeCycle__m_ExtraColourInter < 0.0f)
			CTimeCycle__m_ExtraColourInter = 0.0f;
	}
	if(CTimeCycle__m_ExtraColourInter > 0.0f){
		GradeColorset extraset(CTimeCycle__m_ExtraColour, CTimeCycle__m_ExtraColourWeatherType);
		colorset->Interpolate(colorset, &extraset, 1.0f-CTimeCycle__m_ExtraColourInter, CTimeCycle__m_ExtraColourInter);
	}

	if(CWeather__UnderWaterness > 0.0f){
		GradeColorset curuwset(curHourSel, 20);
		GradeColorset nextuwset(nextHourSel, 20);
		GradeColorset tmpset;
		tmpset.Interpolate(&curuwset, &nextuwset, invTimeInterp, timeInterp);
		colorset->Interpolate(colorset, &tmpset, 1.0f-CWeather__UnderWaterness, CWeather__UnderWaterness);
	}

	if(CWeather__InTunnelness > 0.0f){
		GradeColorset tunnelset(tunnelWeather % NUMHOURS, tunnelWeather / NUMHOURS + EXTRASTART);
		colorset->Interpolate(colorset, &tunnelset, 1.0f-CWeather__InTunnelness, CWeather__InTunnelness);
	}

}

void
Colorcycle::Initialise(void)
{
	int have24h = ModuleList().Get(L"timecycle24") != 0;
	for(int i = 0; i < 24; i++)
		for(int j = 0; j < NUMHOURS; j++){
			redGrade[j][i].r = 1.0f;
			redGrade[j][i].g = 0.0f;
			redGrade[j][i].b = 0.0f;
			redGrade[j][i].a = 0.0f;
			greenGrade[j][i].r = 0.0f;
			greenGrade[j][i].g = 1.0f;
			greenGrade[j][i].b = 0.0f;
			greenGrade[j][i].a = 0.0f;
			blueGrade[j][i].r = 0.0f;
			blueGrade[j][i].g = 0.0f;
			blueGrade[j][i].b = 1.0f;
			blueGrade[j][i].a = 0.0f;
		}
	void *f = CFileMgr::OpenFile("data/colorcycle.dat", "r");
	if(f){
		char *line;
		for(int i = 0; i < NUMWEATHERS; i++){
			for(int j = 0; j < NUMHOURS; j++){
				line = CFileLoader::LoadLine(f);
				sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f",
				       &redGrade[j][i].r, &redGrade[j][i].g,
				       &redGrade[j][i].b, &redGrade[j][i].a,
				       &greenGrade[j][i].r, &greenGrade[j][i].g,
				       &greenGrade[j][i].b, &greenGrade[j][i].a,
				       &blueGrade[j][i].r, &blueGrade[j][i].g,
				       &blueGrade[j][i].b, &blueGrade[j][i].a);
				float sum;
				sum = redGrade[j][i].r + redGrade[j][i].g + redGrade[j][i].b;
				if(sum > 1.7f)
					redGrade[j][i].a -= (sum - 1.7f)*0.13f;
				sum = greenGrade[j][i].r + greenGrade[j][i].g + greenGrade[j][i].b;
				if(sum > 1.7f)
					greenGrade[j][i].a -= (sum - 1.7f)*0.13f;
				sum = blueGrade[j][i].r + blueGrade[j][i].g + blueGrade[j][i].b;
				if(sum > 1.7f)
					blueGrade[j][i].a -= (sum - 1.7f)*0.13f;


				redGrade[j][i].r /= 1.5f;
				redGrade[j][i].g /= 1.5f;
				redGrade[j][i].b /= 1.5f;
				redGrade[j][i].a /= 1.5f;
				greenGrade[j][i].r /= 1.5f;
				greenGrade[j][i].g /= 1.5f;
				greenGrade[j][i].b /= 1.5f;
				greenGrade[j][i].a /= 1.5f;
				blueGrade[j][i].r /= 1.5f;
				blueGrade[j][i].g /= 1.5f;
				blueGrade[j][i].b /= 1.5f;
				blueGrade[j][i].a /= 1.5f;
				//printf("%f %f %f %f X %f %f %f %f X %f %f %f %f\n",
				//	redGrade[j][i].r, redGrade[j][i].g, redGrade[j][i].b, redGrade[j][i].a,
				//	greenGrade[j][i].r, greenGrade[j][i].g, greenGrade[j][i].b, greenGrade[j][i].a,
				//	blueGrade[j][i].r, blueGrade[j][i].g, blueGrade[j][i].b, blueGrade[j][i].a);
			}
		}
		if(have24h)
			for(int j = 0; j < NUMHOURS; j++){
				redGrade[j+8][21] = redGrade[j][22];
				greenGrade[j+8][21] = greenGrade[j][22];
				blueGrade[j+8][21] = blueGrade[j][22];
			}
		CFileMgr::CloseFile(f);
	}
	initialised = true;
}
