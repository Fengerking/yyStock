#include <jni.h>
#include <string>
#include "./android/bitmap.h"

extern "C" JNIEXPORT int JNICALL
Java_com_yystock_yang_MainActivity_updateBitmap (JNIEnv *env, jobject obj, jobject bmp, jint type) {
    AndroidBitmapInfo 	bmpInfo;
    char *             	pPixels = NULL;
    int *               pColor = NULL;
    int                 nColor = 0;

    AndroidBitmap_getInfo(env, bmp, &bmpInfo);
    AndroidBitmap_lockPixels(env, bmp, (void **)&pPixels);

    for (int i = 1; i < bmpInfo.height - 1; i++) {
        pColor = (int *)(pPixels + (i * bmpInfo.stride));
        for (int j = 1; j < bmpInfo.width - 1; j++) {
            nColor = *pColor;
            if (nColor == 0XFFFFFFFF) // Background color
                *pColor = 0XFF485700;
            if (nColor == 0XFFFDE9DD) // FenShi background
                *pColor = 0XFF485700;
            if (nColor == 0XFF0A0AFE) // FenShi background
                *pColor = 0XFFFFFFFF;
            if (nColor == 0XFF5B5FEE) // KX tu Red block
                *pColor = 0XFFFFFFFF;
            pColor++;
        }
    }

    AndroidBitmap_unlockPixels(env, bmp);

    return 1;
}

/*
jclass				m_pClsBitmap;
jmethodID			m_fBmpCreate;
jobject 			m_pBmpVideo;
int	CNDKVDecRnd::SendBitmap (JNIEnv* pEnv, QC_DATA_BUFF * pBuff)
{
	if (m_pClsBitmap == NULL) {
		m_pClsBitmap = pEnv->FindClass("android/graphics/Bitmap");
		if (m_pClsBitmap == NULL) {
			QCLOGW ("It can't find the bitmap class!");
			return QC_ERR_FAILED;
		}
		m_fBmpCreate = pEnv->GetStaticMethodID(m_pClsBitmap,
                            "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
		if (m_fBmpCreate == NULL) {
			QCLOGW ("It can't get the createBitmap method ID!");
			return QC_ERR_FAILED;
		}

		jstring cfgName 	= pEnv->NewStringUTF("ARGB_8888");
    	jclass cfgClass 	= pEnv->FindClass("android/graphics/Bitmap$Config");
    	jmethodID cfgFunc 	= pEnv->GetStaticMethodID(cfgClass, "valueOf",
            										"(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    	jobject cfgBitmap 	= pEnv->CallStaticObjectMethod(cfgClass, cfgFunc, cfgName);
		m_pBmpVideo = pEnv->CallStaticObjectMethod(m_pClsBitmap,
                            m_fBmpCreate, 540, 960, cfgBitmap);
	}
	if (m_pBmpVideo == NULL) {
		QCLOGW ("The bitmap handle is NULL!");
		return QC_ERR_FAILED;
	}

    AndroidBitmapInfo 	bmpInfo;
    void *             	pPixels = 0;

	AndroidBitmap_getInfo(pEnv, m_pBmpVideo, &bmpInfo);
	AndroidBitmap_lockPixels(pEnv, m_pBmpVideo, &pPixels);
	memset (pPixels, 0Xf, 540 * 960);
	AndroidBitmap_unlockPixels(pEnv, m_pBmpVideo);

	pEnv->CallStaticVoidMethod(m_pjCls, m_fPostEvent, m_pjObj, QC_MSG_SNKV_NEW_FORMAT, -2, -2, m_pBmpVideo);

	return QC_ERR_NONE;
}
*/