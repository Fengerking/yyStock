#include <jni.h>
#include <string>

/*
#include "./android/bitmap.h"

	// for bitmap 
	jclass				m_pClsBitmap;
	jmethodID			m_fBmpCreate;
	jobject 			m_pBmpVideo; 
*/

extern "C" JNIEXPORT jstring JNICALL
Java_com_yystock_stockinfo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

/*
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